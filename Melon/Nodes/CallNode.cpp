#include "CallNode.h"

#include "RefNode.h"
#include "StackNode.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CallNode::CallNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

CallNode::~CallNode() {

}

Symbol CallNode::GetFunc() const {
	List<ScopeList> argTypes;

	for (NodePtr node : args) {
		argTypes.Add(node->Type());
	}

	Symbol s;

	if (op) {
		s = Symbol::FindFunction(this->node->Type(), argTypes, file);
	}
	else if (IsInit()) {
		s = Symbol::FindMethod(this->node->Type().Add(Scope::Init).Add(Scope::Call), argTypes, this->node->file);
	}
	else if (!isMethod) {
		if (IsSelfPassing()) {
			argTypes.Add(this->node->Type());
		}

		s = Symbol::FindFunction(this->node->Type().Add(Scope::Call), argTypes, this->node->file);
	}
	else {
		s = Symbol::FindMethod(this->node->Type().Add(methodName).Add(Scope::Call), argTypes, this->node->file);
	}

	if ((IsInit() && s.type == SymbolType::Method) || (isMethod ? s.type == SymbolType::Method : s.type == SymbolType::Function)) {
		return s;
	}

	List<String> argStr;

	for (const ScopeList& type : argTypes) {
		argStr.Add(type.ToString());
	}

	ErrorLog::Error(SymbolError(SymbolError::Function(this->node->Type().ToString(), argStr), file));
	return Symbol();
}

bool CallNode::IsSelfPassing() const {
	if (isMethod) return true;
	if (op || IsInit()) return false;

	Symbol s = node->GetSymbol();

	if (s.type != SymbolType::Function) {
		return true;
	}

	return false;
}

bool CallNode::IsInit() const {
	Symbol s = node->GetSymbol();

	if (s.type == SymbolType::Struct) {
		return !isMethod;
	}

	return false;
}

ScopeList CallNode::Type() const {
	if (IsInit()) return node->Type();

	const Symbol s = GetFunc();

	if (s.type != SymbolType::None) {
		return ScopeList().Add(s.ret[0]);
	}

	return ScopeList::undefined;
}

List<ScopeList> CallNode::Types() const {
	Symbol s = GetFunc();

	List<ScopeList> types;

	if (s.type == SymbolType::None) {
		types.Add(ScopeList::undefined);
		return types;
	}

	if (IsInit()) {
		types.Add(node->Type());
	}
	else for (const ScopeList& type : s.ret) {
		const Symbol s2 = Symbol::FindNearest(s.scope.Pop(), type, node->file);

		if (s2.type != SymbolType::None) {
			types.Add(s2.scope);
		}
		else {
			types.Add(ScopeList::undefined);
		}
	}

	return types;
}

CompiledNode CallNode::Compile(CompileInfo& info) { //TODO: more accurate arg error lines
	Symbol s = GetFunc();

	StackPtr stack = info.stack;
	Int retSize = 0;
	Int argSize = 0;

	CompiledNode c;

	// Push registers to stack
	for (UInt i = 0; i < info.index; i++) {
		Instruction mov = Instruction(InstructionType::Mov, info.stack.ptrSize);

		info.stack.Push(info.stack.ptrSize);
		mov.arguments.Add(Argument(MemoryLocation(info.stack.Offset())));
		mov.arguments.Add(Argument(Register(i)));

		c.instructions.Add(mov);
	}

	// Calculate return size
	for (const ScopeList& type : s.ret)
		retSize += Symbol::FindNearest(s.scope.Pop(), type, node->file).size;

	if (IsInit()) retSize = Symbol::Find(Type(), node->file).size;

	info.stack.Push(retSize);

	// Calculate argument size and compile arguments
	for (UInt u = 0; u < s.args.Size(); u++) {
		Symbol type = Symbol::FindNearest(scope, s.args[u], node->file);

		if (Symbol::Find(s.scope.Add(s.names[u]), node->file).attributes.Contains(SymbolAttribute::Ref)) {
			Pointer<RefNode> r = nullptr;
			Int i = IsInit() ? u - 1 : u;

			if (i == -1) {
				Pointer<StackNode> sn = new StackNode(stack.Offset() - retSize);
				r = new RefNode(sn);
			}
			else if (IsSelfPassing()) {
				if (i == 0) {
					r = new RefNode(node);
				}
				else {
					r = new RefNode(args[i - 1]);
				}
			}
			else {
				r = new RefNode(args[i]);
			}

			CompiledNode n = r->Compile(info);

			c.AddInstructions(n.instructions);

			argSize += info.stack.ptrSize;

			Instruction in = Instruction(InstructionType::Mov, info.stack.ptrSize);
			in.arguments.Add(Argument(MemoryLocation(stack.Offset() - retSize - argSize)));
			in.arguments.Add(n.argument);

			c.instructions.Add(in);
		}
		else {
			Int i = IsInit() ? u - 1 : u;
			argSize += type.size;

			List<ScopeList> args;

			if (IsSelfPassing()) {
				args.Add(this->args[i - 1]->Type());
			}
			else {
				args.Add(this->args[i]->Type());
			}

			Symbol assign = Symbol::FindFunction(type.scope.Add(Scope::Assign), args, node->file);

			List<NodePtr> assignArgs;
			Pointer<StackNode> sn = new StackNode(stack.Offset() - retSize - argSize);
			sn->type = type.scope;
			assignArgs.Add(sn);

			if (isMethod) {
				assignArgs.Add(this->args[i - 1]);
			}
			else {
				assignArgs.Add(this->args[i]);
			}

			c.AddInstructions(assign.node->Compile(assignArgs, info).instructions);
		}
	}

	info.stack.Push(argSize);
	info.stack.base = info.stack.top;
	const UInt stackSize = info.stack.top - stack.base;
	info.stack.Push(info.stack.ptrSize);

	if (!s.ret.IsEmpty()) {
		c.size = Symbol::FindNearest(scope, s.ret[0], node->file).size;
	}
	else {
		c.size = info.stack.ptrSize;
	}

	Instruction sub = Instruction(InstructionType::Sub, info.stack.ptrSize);
	sub.arguments.Add(Argument(Register(RegisterType::Stack)));
	sub.arguments.Add(Argument(stackSize));
	c.instructions.Add(sub);

	Instruction inst = Instruction(InstructionType::Call);
	inst.arguments.Add(Argument(ArgumentType::Function, s.size));

	c.instructions.Add(inst);

	Instruction add = Instruction(InstructionType::Add, info.stack.ptrSize);
	add.arguments.Add(Argument(Register(RegisterType::Stack)));
	add.arguments.Add(Argument(stackSize));
	c.instructions.Add(add);

	c.argument = Argument(MemoryLocation(stack.Offset() - (s.ret.IsEmpty() ? retSize : Symbol::FindNearest(scope, s.ret[0], node->file).size)));

	info.stack = stack;
	return c;
}

Set<ScanType> CallNode::Scan(ScanInfo& info) const {
	Set<ScanType> scanSet = node->Scan(info);

	if (info.init && scanSet.Contains(ScanType::Self) && !info.symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
	}

	Symbol s = GetFunc();

	for (const NodePtr& node : args) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.init && type == ScanType::Self && !info.symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}
	}

	for (UInt u = 0; u < s.args.Size(); u++) {
		Int i = IsInit() ? u - 1 : u;
		Symbol type = Symbol::FindNearest(scope, s.args[u], node->file);
		Symbol::Find(s.scope.Add(s.names[u]), node->file);
		List<ScopeList> args;

		if (i < 0) continue;

		if (IsSelfPassing()) {
			if (i == 0) {
				args.Add(node->Type());
			}
			else {
				args.Add(this->args[i - 1]->Type());
			}
		}
		else {
			args.Add(this->args[i]->Type());
		}

		Symbol assign = Symbol::FindFunction(type.scope.Add(Scope::Assign), args, node->file);
	}

	return scanSet;
}

Mango CallNode::ToMango() const {
	Mango call = Mango(GetFunc().scope.ToString(), MangoType::Map);
	call.Add("value", node->ToMango());

	Mango args = Mango(MangoType::List);

	for (NodePtr node : this->args)
		args.Add(node->ToMango());

	call.Add("args", args);

	return call;
}