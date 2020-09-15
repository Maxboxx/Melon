#include "OptionalAssignValueNode.h"

#include "Melon/Symbols/ScopeList.h"
#include "Melon/Symbols/Symbols.h"

#include "Melon/Nodes/StackNode.h"
#include "Melon/Nodes/BooleanNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CompiledNode OptionalAssignValueNode::Compile(const Boxx::List<NodePtr>& nodes, CompileInfo& info) const {
	bool important = info.important;
	info.important = false;

	CompiledNode c1 = nodes[0]->Compile(info);

	const ScopeList type = nodes[0]->Type();
	const bool isNil = nodes[1]->Type() == ScopeList().Add(Scope::Nil);

	Symbol s = Symbol::Find(type, nodes[0]->file);
	UInt offset = s.size;

	for (UInt i = 0; i < s.args.Size(); i++) {
		if (i > 0 && isNil) break;

		Symbol argSym = s.Get(s.args[i], nodes[0]->file);
		Symbol argType = argSym.GetType(nodes[0]->file);
		offset -= argType.size;

		const ScopeList typeName = argType.scope;

		List<ScopeList> typeArgs;
		typeArgs.Add(typeName);
		Symbol argAssign = Symbol::FindFunction(typeName.Add(Scope::Assign), typeArgs, nodes[0]->file);

		List<NodePtr> args;
		
		Pointer<StackNode> sn1 = new StackNode(c1.argument.mem.offset + offset);
		sn1->type = typeName;

		if (c1.argument.mem.reg.type == RegisterType::Register) {
			sn1->regIndex = c1.argument.mem.reg.index;
		}

		args.Add(sn1);

		if (i == 1) {
			args.Add(nodes[1]);
		}
		else {
			Pointer<BooleanNode> bn = new BooleanNode(nodes[0]->file);
			bn->boolean = !isNil;
			args.Add(bn);
		}

		info.important = important;
		c1.AddInstructions(argAssign.symbolNode->Compile(args, info).instructions);
		info.important = false;
	}

	info.important = important;

	return c1;
}
