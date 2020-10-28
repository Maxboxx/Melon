#include "AssignNode.h"

#include "NewVariableNode.h"
#include "RefNode.h"
#include "StackNode.h"
#include "NameNode.h"
#include "TypeNode.h"
#include "EmptyNode.h"
#include "StatementsNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

AssignNode::AssignNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

AssignNode::~AssignNode() {

}

List<Pair<ScopeList, NodePtr>> AssignNode::Values() const {
	List<Pair<ScopeList, NodePtr>> types;

	for (UInt i = 0; i < vars.Size(); i++) {
		if (i + 1 >= values.Size()) {
			List<ScopeList> returnTypes = values[i]->Types();

			for (UInt u = 0; u < vars.Size() - i; u++) {
				types.Add(Pair<ScopeList, NodePtr>(returnTypes[u], values[i]));
			}

			break;
		}
		else {
			types.Add(Pair<ScopeList, NodePtr>(values[i]->Type(), values[i]));
		}
	}

	return types;
}

UInt AssignNode::GetSize() const {
	return newVars ? newVars->GetSize() : 0;
}

CompiledNode AssignNode::Compile(CompileInfo& info) {
	CompiledNode c;

	UInt varSize = 0;

	if (newVars) {
		c.AddInstructions(newVars->Compile(info).instructions);

		if (newVars->attributes[0].Contains(SymbolAttribute::Ref)) {
			varSize += info.stack.ptrSize;
		}
		else {
			varSize += Symbol::Find(newVars->GetType(0), file).size;
		}

		for (UInt i = 1; i < newVars->names.Size(); i++) {
			if (!vars[i]) continue;

			if (newVars->attributes[i].Contains(SymbolAttribute::Ref)) {
				varSize += info.stack.ptrSize;
			}
			else {
				varSize += Symbol::Find(newVars->GetType(i), file).size;
			}
		}
	}

	List<Pair<ScopeList, NodePtr>> values = Values();
	List<UInt> returnOffsets;
	const UInt frame = info.stack.frame;

	for (UInt i = 0; i < vars.Size(); i++) {
		const UInt regIndex = info.index; 

		if (i < this->values.Size()) {
			if (vars[i]) {
				info.important = true;
				c.AddInstructions(CompileAssignment(vars[i], values[i].value, info, vars[i]->file).instructions);
				info.important = false;
			}
			else {
				c.AddInstructions(values[i].value->Compile(info).instructions);
			}

			if (i + 1 >= this->values.Size()) {
				UInt size = info.stack.top + Symbol::Find(values[i].key, values[i].value->file).size;

				for (UInt u = i + 1; u < values.Size(); u++) {
					size += Symbol::Find(values[u].key, values[i].value->file).size;
					returnOffsets.Add(size);
				}
			}
			else {
				info.stack.PopExpr(frame, c);
			}
		}
		else if (vars[i]) {
			Pointer<StackNode> sn = new StackNode(info.stack.Offset(returnOffsets[i - this->values.Size()]));
			sn->type = values[i].key;

			info.important = true;
			c.AddInstructions(CompileAssignment(vars[i], sn, info, vars[i]->file).instructions);
			info.important = false;
		}

		info.index = regIndex;
	}

	info.stack.PopExpr(frame, c);

	return c;
}

void AssignNode::IncludeScan(ParsingInfo& info) {
	if (newVars) newVars->IncludeScan(info);
	
	for (NodePtr var : vars) {
		if (var) var->IncludeScan(info);
	}

	for (NodePtr value : values) {
		value->IncludeScan(info);
	}
}

Set<ScanType> AssignNode::Scan(ScanInfoStack& info) {
	info.Get().assign = true;

	UInt errorCount = ErrorLog::ErrorCount();

	Set<ScanType> scanSet = newVars ? newVars->Scan(info) : Set<ScanType>();

	if (info.Get().init && scanSet.Contains(ScanType::Self) && !info.Get().symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, newVars->file));
	}

	List<Pair<ScopeList, NodePtr>> values = Values();

	bool errors = errorCount < ErrorLog::ErrorCount();

	for (UInt i = 0; i < vars.Size(); i++) {
		if (!vars[i]) continue;

		NodePtr node = vars[i];

		if (!newVars && node->GetSymbol().attributes.Contains(SymbolAttribute::Const)) {
			ErrorLog::Error(SymbolError(SymbolError::ConstAssign, node->file));
		}

		if (!newVars && !node.Is<NameNode>()) {
			for (const ScanType type : node->Scan(info)) {
				scanSet.Add(type);

				if (type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
					ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
				}
			}
		}

		if (info.Get().init) {
			if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
				if (nn->name == Scope::Self) {
					Symbol::Find(nn->Type(), file).AssignAll();
					scanSet.Remove(ScanType::Self);
				}
			}
		}

		if (!errors) {
			ScanAssignment(node, new TypeNode(values[i].key), info, node->file);
		}
	}

	info.Get().assign = false;

	for (const NodePtr& node : this->values) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}
	}

	return scanSet;
}

ScopeList AssignNode::FindSideEffectScope(const bool assign) {
	ScopeList list = vars[0] ? vars[0]->GetSideEffectScope(true) : scope;

	for (UInt i = 1; i < vars.Size(); i++) {
		if (vars[i]) {
			list = CombineSideEffects(list, vars[i]->GetSideEffectScope(true));
		}
	}

	for (UInt i = 0; i < values.Size(); i++) {
		list = CombineSideEffects(list, values[i]->GetSideEffectScope(assign));
	}

	return list;
}

NodePtr AssignNode::Optimize(OptimizeInfo& info) {
	bool removed = false;

	for (UInt i = 0; i < vars.Size(); i++) {
		if (vars[i]->GetSymbol().IsVariable() && !vars[i]->HasSideEffects(scope) && !info.usedVariables.Contains(vars[i]->GetSymbol().scope)) {
			vars[i] = nullptr;
			info.optimized = true;
			removed = true;
		}
	}

	if (removed) for (UInt i = values.Size(); i > 0;) {
		i--;

		if (!vars[i] && !values[i]->HasSideEffects(scope)) {
			bool isNull = true;

			for (UInt u = i + 1; u < vars.Size(); u++) {
				if (vars[u]) {
					isNull = false;
					break;
				}
			}

			if (!isNull) values[i] = nullptr;
		}
	}

	if (removed) for (UInt i = vars.Size(); i > 0;) {
		i--;

		if (!vars[i]) {
			if (i >= values.Size()) {
				if (i == vars.Size() - 1 || !values.Last()) {
					vars.RemoveAt(i);
				}
			}
			else if (!values[i]) {
				vars.RemoveAt(i);
				values.RemoveAt(i);
			}
		}
	}

	if (removed && vars.IsEmpty()) {
		return new EmptyNode();
	}

	for (NodePtr& var : vars) {
		if (var) if (NodePtr node = var->Optimize(info)) var = node;
	}

	for (NodePtr& value : values) {
		if (value) if (NodePtr node = value->Optimize(info)) value = node;
	}

	return nullptr;
}

Mango AssignNode::ToMango() const {
	Mango assign = Mango("assign", MangoType::List);

	if (newVars)
		assign.Add(newVars->ToMango());

	for (UInt i = 0; i < vars.Size(); i++) {
		List<ScopeList> args;

		if (values.Size() > i) {
			args.Add(values[i]->Type());
		}
		else {
			args.Add(values.Last()->Types()[i - values.Size() + 1]);
		}

		if (vars[i]) {
			const ScopeList op = Symbol::FindFunction(vars[i]->Type().Add(Scope::Assign), args, vars[i]->file).scope;

			Mango mango = Mango(op.ToString(), MangoType::List);
			mango.Add(vars[i]->ToMango());

			if (values.Size() > i) {
				mango.Add(values[i]->ToMango());
			}

			assign.Add(mango);
		}
		else {
			if (values.Size() > i) {
				assign.Add(values[i]->ToMango());
			}
			else {
				assign.Add(Mango(MangoType::Nil));
			}
		}
	}

	return assign;
}

StringBuilder AssignNode::ToMelon(const UInt indent) const {
	StringBuilder sb;
	
	if (newVars) {
		sb = newVars->ToMelon(indent);
	}
	else for (UInt i = 0; i < vars.Size(); i++) {
		if (i > 0) sb += ", ";

		if (vars[i]) sb += vars[i]->ToMelon(indent);
	}

	sb += " = ";

	for (UInt i = 0; i < values.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += values[i]->ToMelon(indent);
	}

	return sb;
}