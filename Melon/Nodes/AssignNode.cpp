#include "AssignNode.h"

#include "NewVariableNode.h"
#include "RefNode.h"
#include "MemoryNode.h"
#include "NameNode.h"
#include "TypeNode.h"
#include "EmptyNode.h"
#include "StatementsNode.h"
#include "DiscardNode.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/NamespaceSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

AssignNode::AssignNode(Symbol* const scope, const FileInfo& file) : StatementNode(scope, file) {

}

AssignNode::~AssignNode() {

}

UInt AssignNode::GetSize() const {
	UInt size = 0;

	for (UInt i = 0; i < assignableValues.Size(); i++) {
		if (types[i] == NameList::Discard || assignableValues[i]->Is<DiscardNode>()) continue;

		if (VariableSymbol* const var = assignableValues[i]->Symbol<VariableSymbol>()) {
			if ((var->attributes & VariableAttributes::Ref) != VariableAttributes::None) {
				size += StackPtr::ptrSize;
			}
			else {
				size += var->Type()->Size();
			}
		}
	}

	return size;
}

void AssignNode::IncludeScan(ParsingInfo& info) {
	// Check type names for includes
	for (const NameList& type : types) {
		if (type == NameList::Discard) continue;

		Include(type, info);
	}

	// Scan assignable values
	for (ExpressionNode* const value : assignableValues) {
		value->IncludeScan(info);
	}

	// Scan values
	for (ExpressionNode* const value : values) {
		value->IncludeScan(info);
	}
}

ScanResult AssignNode::Scan(ScanInfoStack& info) {
	ScanResult result = ScanAssignableValues(info);

	// Scan values
	for (ExpressionNode* const node : this->values) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->File());
		result |= r;
	}

	return result;
}

CompiledNode AssignNode::Compile(CompileInfo& info) {
	CompiledNode c;

	UInt varSize = 0;

	// Calculate size and stack index of variables
	for (UInt i = 0; i < assignableValues.Size(); i++) {
		if (types[i] == NameList::Discard || assignableValues[i]->Is<DiscardNode>()) continue;
		VariableSymbol* const var = assignableValues[i]->Symbol<VariableSymbol>();

		// Ref variables
		if (var->HasAttribute(VariableAttributes::Ref)) {
			varSize += info.stack.ptrSize;
			info.stack.Push(info.stack.ptrSize);
		}
		// Regular variables
		else {
			UInt size = assignableValues[i]->Type()->Size();
			varSize += size;
			info.stack.Push(size);
		}

		var->stackIndex = info.stack.top;
	}

	// Setup
	List<Value> values = Values();
	List<UInt> returnOffsets;
	const UInt frame = info.stack.frame;

	// Compile assignments
	for (UInt i = 0; i < assignableValues.Size(); i++) {
		const UInt regIndex = info.index; 

		// Assign values normally
		if (i < this->values.Size()) {
			// Regular assignment
			if (!assignableValues[i]->Is<DiscardNode>()) {
				info.important = true;
				c.AddInstructions(CompileAssignment(assignableValues[i], values[i].value, info, assignableValues[i]->File()).instructions);
				info.important = false;
			}
			// Discard assignment
			else {
				// TODO: Cast to type
				c.AddInstructions(values[i].value->Compile(info).instructions);
			}

			// Calculate offsets for extra return values
			if (i + 1 >= this->values.Size()) {
				UInt size = info.stack.top + values[i].type->Size();

				for (UInt u = i + 1; u < values.Size(); u++) {
					size += values[u].type->Size();
					returnOffsets.Add(size);
				}
			}
			else {
				info.stack.PopExpr(frame, c);
			}
		}
		// Assign extra return values
		else if (!assignableValues[i]->Is<DiscardNode>()) {
			MemoryNode* const mn = new MemoryNode(info.stack.Offset(returnOffsets[i - this->values.Size()]));
			mn->type = values[i].type->AbsoluteName();

			info.important = true;
			c.AddInstructions(CompileAssignment(assignableValues[i], mn, info, assignableValues[i]->File()).instructions);
			info.important = false;

			delete mn;
		}

		// TODO: compile values for discard vars

		info.index = regIndex;
	}

	info.stack.PopExpr(frame, c);

	return c;
}

NameList AssignNode::FindSideEffectScope(const bool assign) {
	NameList list = assignableValues[0] ? assignableValues[0]->GetSideEffectScope(true) : scope->AbsoluteName();

	for (UInt i = 1; i < assignableValues.Size(); i++) {
		if (assignableValues[i]) {
			list = CombineSideEffects(list, assignableValues[i]->GetSideEffectScope(true));
		}
	}

	for (UInt i = 0; i < values.Size(); i++) {
		list = CombineSideEffects(list, values[i]->GetSideEffectScope(assign));
	}

	return list;
}

StatementNode* AssignNode::Optimize(OptimizeInfo& info) {
	bool removed = false;

	// Check if removal of values can be done
	for (ExpressionNode*& value : assignableValues) {
		if (value->Is<DiscardNode>()) {
			removed = true;
			continue;
		}

		// Remove unused vars
		if (VariableSymbol* const sym = value->Symbol<VariableSymbol>()) {
			if (!value->HasSideEffects(scope->AbsoluteName()) && !info.usedVariables.Contains(sym)) {
				delete value;
				value = new DiscardNode(value->scope, value->File());
				info.optimized = true;
				removed = true;
			}
		}
	}

	// Remove values
	if (removed) for (UInt i = values.Size(); i > 0;) {
		i--;

		// Remove values without side effects
		if (assignableValues[i]->Is<DiscardNode>() && !values[i]->HasSideEffects(scope->AbsoluteName())) {
			// Check if the value is part of a multiple return
			if (i >= values.Size() - 1) {
				bool isDiscard = true;
				
				// Only remove value if multiple return is not affected
				for (UInt u = i + 1; u < assignableValues.Size(); u++) {
					if (!assignableValues[u]->Is<DiscardNode>()) {
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
	if (removed) for (UInt i = assignableValues.Size(); i > 0;) {
		i--;

		if (assignableValues[i]->Is<DiscardNode>()) {
			// Remove multiple return vars
			if (i >= values.Size()) {
				if (i == assignableValues.Size() - 1 || !values.Last()) {
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
		return new EmptyNode();
	}

	// Optimize assignable values
	for (ExpressionNode*& value : assignableValues) {
		Node::Optimize(value, info);
	}

	// Optimize values
	for (ExpressionNode*& value : values) {
		Node::Optimize(value, info);
	}

	return nullptr;
}

StringBuilder AssignNode::ToMelon(const UInt indent) const {
	StringBuilder sb;

	Optional<NameList> type = types[0];

	// Check if all types are equal
	for (UInt i = 1; i < types.Size(); i++) {
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
		for (UInt i = 0; i < types.Size(); i++) {
			if (i > 0) sb += ", "; 
			sb += types[i].ToSimpleString();
		}

		sb += ": ";
	}
	
	// Vars
	for (UInt i = 0; i < assignableValues.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += assignableValues[i]->ToMelon(indent);
	}

	sb += " = ";

	// Values
	for (UInt i = 0; i < values.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += values[i]->ToMelon(indent);
	}

	return sb;
}

ScanResult AssignNode::ScanAssignableValues(ScanInfoStack& info) {
	ScanResult result;
	info->assign = true;

	// Check for errors among values
	UInt errorCount = ErrorLog::ErrorCount();
	List<Value> values = Values();
	bool errors = errorCount < ErrorLog::ErrorCount();

	// Scan all vars
	for (UInt i = 0; i < assignableValues.Size(); i++) {
		if (assignableValues[i]->Is<DiscardNode>()) continue;

		ExpressionNode* const node = assignableValues[i];
		node->Type();

		// Find variable
		VariableSymbol* const var = node->Symbol<VariableSymbol>();

		// Check for const assign
		if (types[i] == NameList::Discard && var && var->HasAttribute(VariableAttributes::Const)) {
			ErrorLog::Error(LogMessage("error.scan.assign.const"), node->File());
		}

		// Scan node
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->File());
		result |= r;

		// Check for completed init
		if (info->init) {
			if (NameNode* const nn = node->Cast<NameNode>()) {
				if (nn->name == Name::Self) {
					info->type->CompleteInit();
					result.selfUsed = false;
				}
			}
		}

		// Scan assignment
		if (!errors) {
			TypeNode* const type = new TypeNode(values[i].type->AbsoluteName());
			ScanAssignment(node, type, info, node->File());
			delete type;
		}
	}

	info->assign = false;
	return result;
}

List<AssignNode::Value> AssignNode::Values() const {
	List<Value> types;

	for (UInt i = 0; i < assignableValues.Size(); i++) {
		if (i + 1 >= values.Size()) {
			List<TypeSymbol*> returnTypes = values[i]->Types();

			for (UInt u = 0; u < assignableValues.Size() - i; u++) {
				types.Add(Value(returnTypes[u], values[i]));
			}

			break;
		}
		else {
			types.Add(Value(values[i]->Type(), values[i]));
		}
	}

	return types;
}
