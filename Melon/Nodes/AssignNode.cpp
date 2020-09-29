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

		//info.stack.Pop(varSize);
	}

	List<Pair<ScopeList, NodePtr>> values = Values();
	List<UInt> returnSizes;

	for (UInt i = 0; i < vars.Size(); i++) {
		const UInt regIndex = info.index; 

		if (i < values.Size()) {
			List<NodePtr> nodes;

			bool refAssign = false;

			if (newVars && vars[i]->GetSymbol().attributes.Contains(SymbolAttribute::Ref)) {
				Pointer<RefNode> r = new RefNode(values[i].value);

				refAssign = true;

				CompiledNode n1 = vars[i]->Compile(info);
				CompiledNode n2 = r->Compile(info);

				c.AddInstructions(n1.instructions);
				c.AddInstructions(n2.instructions);

				OptimizerInstruction in = Instruction(InstructionType::Mov, info.stack.ptrSize);
				in.important = true;
				in.instruction.arguments.Add(n1.argument);
				in.instruction.arguments.Add(n2.argument);

				c.instructions.Add(in);
				c.size = info.stack.ptrSize;
			}
			else {
				nodes.Add(vars[i]);
				nodes.Add(values[i].value);
			}

			List<ScopeList> args;
			args.Add(values[i].key);

			if (!refAssign) {
				info.important = true;
				c.AddInstructions(CompileAssignment(vars[i], values[i].value, info, vars[i]->file).instructions);
				info.important = false;

				if (i + 1 >= values.Size()) {
					UInt size = info.stack.top + Symbol::Find(values[i].key, values[i].value->file).size;

					for (UInt u = i + 1; u < values.Size(); u++) {
						size += Symbol::Find(values[u].key, values[i].value->file).size;
						returnSizes.Add(size);
					}
				}
			}
		}
		else {
			Pointer<StackNode> sn = new StackNode(info.stack.Offset(returnSizes[i - values.Size()]));
			sn->type = values[i].key;

			info.important = true;
			c.AddInstructions(CompileAssignment(vars[i], sn, info, vars[i]->file).instructions);
			info.important = false;
		}

		info.index = regIndex;
	}

	if (newVars) {
		//info.stack.Push(varSize);
	}

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
			// TODO: check for valid ref
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