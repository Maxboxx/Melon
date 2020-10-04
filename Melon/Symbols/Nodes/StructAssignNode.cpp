#include "StructAssignNode.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/Symbols.h"

#include "Melon/Nodes/StackNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode StructAssignNode::Compile(const Boxx::List<NodePtr>& nodes, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = nodes[0]->Compile(info);
	const UInt frame = info.stack.frame;
	CompiledNode c2 = nodes[1]->Compile(info);
	c1.AddInstructions(c2.instructions);
	c1.argument = OffsetArgument(c1.argument, frame, info);

	const ScopeList type = nodes[0]->Type();

	Symbol s = Symbol::Find(type, nodes[0]->file);

	for (UInt i = 0; i < s.arguments.Size(); i++) {
		Symbol argSym = s.Get(s.arguments[i], nodes[0]->file);
		Symbol argType = argSym.GetType(nodes[0]->file);

		const ScopeList typeName = argType.scope;

		List<ScopeList> typeArgs;
		typeArgs.Add(typeName);
		Symbol argAssign = Symbol::FindFunction(typeName.Add(Scope::Assign), typeArgs, nodes[0]->file);

		List<NodePtr> args;
		Pointer<StackNode> sn1 = new StackNode(c1.argument.mem.offset + argSym.offset);
		sn1->type = typeName;

		if (c1.argument.mem.reg.type == RegisterType::Register) {
			sn1->regIndex = c1.argument.mem.reg.index;
		}

		Pointer<StackNode> sn2 = new StackNode(c2.argument.mem.offset + argSym.offset);
		sn2->type = typeName;

		if (c2.argument.mem.reg.type == RegisterType::Register) {
			sn2->regIndex = c2.argument.mem.reg.index;
		}

		args.Add(sn1);
		args.Add(sn2);

		info.important = important;
		c1.AddInstructions(argAssign.symbolNode->Compile(args, info).instructions);
		info.important = false;
	}

	info.important = important;

	return c1;
}
