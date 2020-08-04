#include "AssignNode.h"

#include "NewVariableNode.h"
#include "RefNode.h"
#include "StackNode.h"
#include "NameNode.h"

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
		types.Add(Pair<ScopeList, NodePtr>(values[i]->Type(), values[i]));

		if (i + 1 >= values.Size()) {
			List<ScopeList> returnTypes = values[i]->Types();

			for (UInt u = 0; u < vars.Size() - i; u++) {
				types.Add(Pair<ScopeList, NodePtr>(returnTypes[u], values[i]));
			}

			break;
		}
	}

	return types;
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
			varSize += Symbol::FindInNamespace(newVars->types[0], file).size;
		}

		for (UInt i = 1; i < newVars->names.Size(); i++) {
			if (newVars->attributes[i].Contains(SymbolAttribute::Ref)) {
				varSize += info.stack.ptrSize;
			}
			else {
				if (newVars->types.Size() == 1) {
					varSize += Symbol::FindInNamespace(newVars->types[0], file).size;
				}
				else {
					varSize += Symbol::FindInNamespace(newVars->types[i], file).size;
				}
			}
		}

		info.stack.Pop(varSize);
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
				Symbol s = Symbol::FindFunction(vars[i]->Type().Add(Scope::Assign), args, vars[i]->file);

				info.important = true;
				CompiledNode c1 = s.node->Compile(nodes, info);
				info.important = false;
				c.AddInstructions(c1.instructions);

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

			List<NodePtr> nodes;
			nodes.Add(vars[i]);
			nodes.Add(sn);

			List<ScopeList> args;
			args.Add(values[i].key);

			Symbol s = Symbol::FindFunction(vars[i]->Type().Add(Scope::Assign), args, vars[i]->file);
			info.important = true;
			CompiledNode c1 = s.node->Compile(nodes, info);
			info.important = false;
			c.AddInstructions(c1.instructions);
		}

		info.index = regIndex;
	}

	if (newVars) {
		info.stack.Push(varSize);
	}

	return c;
}

void AssignNode::IncludeScan(ParsingInfo& info) {

}

Set<ScanType> AssignNode::Scan(ScanInfo& info) const {
	info.assign = true;

	UInt errorCount = ErrorLog::ErrorCount();

	Set<ScanType> scanSet = newVars ? newVars->Scan(info) : Set<ScanType>();

	if (info.init && scanSet.Contains(ScanType::Self) && !info.symbol.IsAssigned()) {
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

			if (info.init) {
				if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
					if (nn->name == Scope::Self) {
						Symbol::Find(nn->Type(), file).AssignAll();
						scanSet.Remove(ScanType::Self);
					}
				}
				else if (type == ScanType::Self && !info.symbol.IsAssigned()) {
					ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
				}
			}
		}

		if (!errors) {
			List<ScopeList> args; //TODO: check for valid ref
			args.Add(values[i].key);
			Symbol::FindFunction(node->Type().Add(Scope::Assign), args, node->file);
		}
	}

	info.assign = false;

	for (const NodePtr& node : this->values) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.init && type == ScanType::Self && !info.symbol.IsAssigned()) {
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