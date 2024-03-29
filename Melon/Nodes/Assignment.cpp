#include "Assignment.h"

#include "NameExpression.h"
#include "TypeExpression.h"
#include "EmptyStatement.h"
#include "Statements.h"
#include "DiscardExpression.h"
#include "KiwiVariable.h"
#include "CallExpression.h"
#include "BaseCallNode.hpp"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/NamespaceSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

Assignment::Assignment(Symbol* const scope, const FileInfo& file) : Statement(scope, file) {

}

Assignment::~Assignment() {
	
}

void Assignment::IncludeScan(ParsingInfo& info) {
	for (const NameList& type : types) {
		if (type != NameList::Discard || type != NameList::Any) {
			Include(type, info);
		}
	}

	for (Weak<Expression> value : assignableValues) {
		value->IncludeScan(info);
	}

	for (Weak<Expression> value : values) {
		value->IncludeScan(info);
	}
}

ScanResult Assignment::Scan(ScanInfoStack& info) {
	ScanResult result = ScanAssignableValues(info);

	for (const Ptr<Expression>& node : values) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->File());
		result |= r;
	}

	return result;
}

Ptr<Kiwi::Value> Assignment::Compile(CompileInfo& info) {
	// Setup
	List<Value> values = Values();

	List<Ptr<KiwiVariable>> callValues;

	// Compile assignments
	for (UInt i = 0; i < assignableValues.Count(); i++) {
		// Assign values normally
		if (i < this->values.Count()) {
			if (i == this->values.Count() - 1) {
				if (Weak<CallExpression> call = this->values[i].As<CallExpression>()) {
					FunctionSymbol* func = call->GetFunc();

					if (func && func->returnValues.Count() > 1) {
						List<Optional<Kiwi::Type>> types;
						List<Ptr<Kiwi::Variable>> vars;

						for (UInt i = 0; i < func->returnValues.Count(); i++) {
							TypeSymbol* type = func->ReturnType(i);
							types.Add(type->KiwiType());
							vars.Add(new Kiwi::Variable(info.NewRegister()));
							callValues.Add(new KiwiVariable(vars.Last()->Copy(), type->AbsoluteName()));
						}

						List<Ptr<Kiwi::Expression>> expressions;
						expressions.Add(call->CompileCallExpression(info));

						info.AddInstruction(new Kiwi::MultiAssignInstruction(types, vars, expressions));
					}
				}
			}

			Weak<Expression> value = callValues.IsEmpty() ? values[i].value : callValues[0];

			if (!assignableValues[i].Is<DiscardExpression>()) {
				CompileAssignment(assignableValues[i], value, info, assignableValues[i]->File(), types.Count() > i && types[i] != NameList::Discard);
			}
			else {
				// TODO: Cast to type
				value->Compile(info);
			}
		}
		// Assign extra return values
		else if (!assignableValues[i].Is<DiscardExpression>()) {
			CompileAssignment(assignableValues[i], callValues[this->values.Count() - i + 1], info, assignableValues[i]->File(), types.Count() > i && types[i] != NameList::Discard);
		}

		// TODO: compile values for discard vars
	}

	return nullptr;
}

NameList Assignment::FindSideEffectScope(const bool assign) {
	NameList list = assignableValues[0] ? assignableValues[0]->GetSideEffectScope(true) : scope->AbsoluteName();

	for (UInt i = 1; i < assignableValues.Count(); i++) {
		if (assignableValues[i]) {
			list = CombineSideEffects(list, assignableValues[i]->GetSideEffectScope(true));
		}
	}

	for (UInt i = 0; i < values.Count(); i++) {
		list = CombineSideEffects(list, values[i]->GetSideEffectScope(assign));
	}

	return list;
}

Ptr<Statement> Assignment::Optimize(OptimizeInfo& info) {
	bool removed = false;

	// Check if removal of values can be done
	for (Ptr<Expression>& value : assignableValues) {
		if (value.Is<DiscardExpression>()) {
			removed = true;
			continue;
		}

		// Remove unused vars
		if (VariableSymbol* const sym = value->Symbol<VariableSymbol>()) {
			if (!value->HasSideEffects(scope->AbsoluteName()) && !info.usedVariables.Contains(sym)) {
				value = new DiscardExpression(value->scope, value->File());
				info.optimized = true;
				removed = true;
			}
		}
	}

	// Remove values
	if (removed) for (UInt i = values.Count(); i > 0;) {
		i--;

		// Remove values without side effects
		if (assignableValues[i].Is<DiscardExpression>() && !values[i]->HasSideEffects(scope->AbsoluteName())) {
			// Check if the value is part of a multiple return
			if (i >= values.Count() - 1) {
				bool isDiscard = true;
				
				// Only remove value if multiple return is not affected
				for (UInt u = i + 1; u < assignableValues.Count(); u++) {
					if (!assignableValues[u].Is<DiscardExpression>()) {
						isDiscard = false;
						break;
					}
				}

				if (isDiscard) values[i] = nullptr;
			}
			else {
				values[i] = nullptr;
			}
		}
	}

	// Remove vars and values
	if (removed) for (UInt i = assignableValues.Count(); i > 0;) {
		i--;

		if (assignableValues[i].Is<DiscardExpression>()) {
			// Remove multiple return vars
			if (i >= values.Count()) {
				if (i == assignableValues.Count() - 1 || !values.Last()) {
					assignableValues.RemoveAt(i);
					types.RemoveAt(i);
				}
			}
			// Remove regular vars/values
			else if (!values[i]) {
				assignableValues.RemoveAt(i);
				values.RemoveAt(i);
				types.RemoveAt(i);
			}
		}
	}

	// Remove assignment if everything is removed
	if (removed && assignableValues.IsEmpty()) {
		return new EmptyStatement();
	}

	// Optimize assignable values
	for (Ptr<Expression>& value : assignableValues) {
		Node::Optimize(value, info);
	}

	// Optimize values
	for (Ptr<Expression>& value : values) {
		Node::Optimize(value, info);
	}

	return nullptr;
}

void Assignment::OptimizeAsCondition(OptimizeInfo& info) {
	// Check if removal of values can be done
	for (Ptr<Expression>& value : assignableValues) {
		if (value.Is<DiscardExpression>()) {
			continue;
		}

		// Remove unused vars
		if (VariableSymbol* const sym = value->Symbol<VariableSymbol>()) {
			if (!value->HasSideEffects(scope->AbsoluteName()) && !info.usedVariables.Contains(sym)) {
				value = new DiscardExpression(value->scope, value->File());
				info.optimized = true;
			}
		}
	}

	// Optimize assignable values
	for (Ptr<Expression>& value : assignableValues) {
		Node::Optimize(value, info);
	}

	// Optimize values
	for (Ptr<Expression>& value : values) {
		Node::Optimize(value, info);
	}
}

StringBuilder Assignment::ToMelon(const UInt indent) const {
	StringBuilder sb;

	Optional<NameList> type = types[0];

	// Check if all types are equal
	for (UInt i = 1; i < types.Count(); i++) {
		if (types[0] != types[i]) {
			type = nullptr;
		}
	}

	// Single/No types
	if (type) {
		if (type != NameList::Discard) {
			sb += type->ToSimpleString();
			sb += ": ";
		}
	}
	// Multiple types
	else {
		for (UInt i = 0; i < types.Count(); i++) {
			if (i > 0) sb += ", "; 
			sb += types[i].ToSimpleString();
		}

		sb += ": ";
	}
	
	// Vars
	for (UInt i = 0; i < assignableValues.Count(); i++) {
		if (i > 0) sb += ", ";
		sb += assignableValues[i]->ToMelon(indent);
	}

	sb += " = ";

	// Values
	for (UInt i = 0; i < values.Count(); i++) {
		if (i > 0) sb += ", ";
		sb += values[i]->ToMelon(indent);
	}

	return sb;
}

ScanResult Assignment::ScanAssignableValues(ScanInfoStack& info) {
	ScanResult result;
	info->assign = true;

	// Infer types
	List<TypeSymbol*> infTypes;

	for (UInt i = 0; i < values.Count(); i++) {
		if (i + 1 == values.Count()) {
			for (TypeSymbol* type : values[i]->Types(values[i]->Type())) {
				infTypes.Add(type);
			}

			while (infTypes.Count() < assignableValues.Count()) {
				infTypes.Add(nullptr);
			}
		}
		else {
			infTypes.Add(values[i]->Type());
		}
	}

	List<NameList> newTypes;

	for (UInt i = 0; i < assignableValues.Count(); i++) {
		NameList current = types[types.Count() == 1 ? 0 : i];

		if (current != NameList::Any) {
			newTypes.Add(current);
			continue;
		}

		if (VariableSymbol* sym = assignableValues[i]->Symbol<VariableSymbol>()) {
			TypeSymbol* type = infTypes[i];
			
			if (type) {
				sym->type = type->AbsoluteName();
				newTypes.Add(sym->type);
			}
			else {
				// TODO: error
				ErrorLog::Error(LogMessage("cant infer type"), assignableValues[i]->File());
				newTypes.Add(current);
			}
		}
		else {
			// TODO: error?
			newTypes.Add(current);
		}
	}

	types = newTypes;

	// Check for errors among values
	UInt errorCount = ErrorLog::ErrorCount();
	List<Value> values = Values();
	bool errors = errorCount < ErrorLog::ErrorCount();

	// Scan all vars
	for (UInt i = 0; i < assignableValues.Count(); i++) {
		if (assignableValues[i].Is<DiscardExpression>()) continue;

		Weak<Expression> node = assignableValues[i];
		node->Type();

		// Find variable
		VariableSymbol* const var = node->Symbol<VariableSymbol>();

		// Check for const assign
		if (i < types.Count() && types[i] == NameList::Discard && var && var->HasAttribute(VariableModifiers::Const)) {
			ErrorLog::Error(LogMessage("error.scan.assign.const"), node->File());
		}

		// Scan node
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->File());
		result |= r;

		// Check for completed init
		if (info->init) {
			if (Weak<NameExpression> nn = node.As<NameExpression>()) {
				if (nn->name == Name::Self) {
					info->type->CompleteInit();
					result.selfUsed = false;
				}
			}
		}

		// Scan assignment
		if (!errors) {
			if (values[i].type) {
				Ptr<TypeExpression> type = new TypeExpression(values[i].type->AbsoluteName());
				ScanAssignment(node, type, info, node->File());
			}
			else {
				// TODO: error
				ErrorLog::Error(LogMessage("assign type error"), values[i].value->File());
			}
		}
	}

	info->assign = false;
	return result;
}

List<Assignment::Value> Assignment::Values() const {
	List<Value> types;

	for (UInt i = 0; i < assignableValues.Count(); i++) {
		if (i + 1 >= values.Count()) {
			List<TypeSymbol*> returnTypes = values[i]->Types(assignableValues[i]->Type());

			for (UInt u = 0; u < assignableValues.Count() - i; u++) {
				types.Add(Value(returnTypes[u], values[i]));
			}

			break;
		}
		else {
			types.Add(Value(values[i]->Type(assignableValues[i]->Type()), values[i]));
		}
	}

	return types;
}
