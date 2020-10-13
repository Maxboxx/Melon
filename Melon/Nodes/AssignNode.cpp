#include "AssignNode.h"

#include "NewVariableNode.h"
#include "RefNode.h"
#include "StackNode.h"
#include "NameNode.h"
#include "TypeNode.h"

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
			info.important = true;
			c.AddInstructions(CompileAssignment(vars[i], values[i].value, info, vars[i]->file).instructions);
			info.important = false;

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
		else {
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
		var->IncludeScan(info);
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
		NodePtr node = vars[i];

		if (!newVars && node->GetSymbol().attributes.Contains(SymbolAttribute::Const)) {
			ErrorLog::Error(SymbolError(SymbolError::ConstAssign, node->file));
		}

		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init) {
				if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
					if (nn->name == Scope::Self) {
						Symbol::Find(nn->Type(), file).AssignAll();
						scanSet.Remove(ScanType::Self);
					}
				}
				else if (type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
					ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
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

		const ScopeList op = Symbol::FindFunction(vars[i]->Type().Add(Scope::Assign), args, vars[i]->file).scope;

		Mango mango = Mango(op.ToString(), MangoType::List);
		mango.Add(vars[i]->ToMango());

		if (values.Size() > i) {
			mango.Add(values[i]->ToMango());
		}

		assign.Add(mango);
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
		sb += vars[i]->ToMelon(indent);
	}

	sb += " = ";

	for (UInt i = 0; i < values.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += values[i]->ToMelon(indent);
	}

	return sb;
}