#include "CallNode.h"

#include "RefNode.h"
#include "StackNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
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

	UInt errorCount = ErrorLog::ErrorCount();

	ScopeList type = this->node->Type();

	if (errorCount < ErrorLog::ErrorCount()) {
		return Symbol();
	}

	if (op) {
		s = Symbol::FindFunction(type, argTypes, file);
	}
	else if (IsInit()) {
		s = Symbol::FindMethod(type.Add(Scope::Init).Add(Scope::Call), argTypes, this->node->file);
	}
	else if (!isMethod) {
		s = Symbol::FindFunction(type.Add(Scope::Call), argTypes, this->node->file);
	}
	else {
		s = Symbol::FindMethod(type.Add(methodName).Add(Scope::Call), argTypes, this->node->file);
	}

	if ((IsInit() && s.type == SymbolType::Method) || (isMethod ? s.type == SymbolType::Method : s.type == SymbolType::Function)) {
		return s;
	}

	List<String> argStr;

	for (const ScopeList& type : argTypes) {
		argStr.Add(type.ToString());
	}

	ErrorLog::Error(SymbolError(SymbolError::Function(type.ToString(), argStr), file));
	return Symbol();
}

bool CallNode::IsSelfPassing() const {
	if (isMethod) return true;
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

	UInt errorCount = ErrorLog::ErrorCount();

	const Symbol s = GetFunc();

	if (errorCount < ErrorLog::ErrorCount()) {
		return ScopeList::undefined;
	}

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

	// Calculate return size
	for (const ScopeList& type : s.ret)
		retSize += Symbol::FindNearestInNamespace(s.scope.Pop(), type, FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces)).size;

	if (IsInit()) retSize = Symbol::Find(Type(), node->file).size;

	info.stack.Push(retSize);

	// Calculate argument size and compile arguments
	for (UInt u = 0; u < s.args.Size(); u++) {
		Symbol type = Symbol::FindNearestInNamespace(scope, s.args[u], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces));

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

			UInt regIndex = info.index;

			CompiledNode n = r->Compile(info);

			c.AddInstructions(n.instructions);

			argSize += info.stack.ptrSize;

			Instruction in = Instruction(InstructionType::Mov, info.stack.ptrSize);
			in.arguments.Add(Argument(MemoryLocation(stack.Offset() - retSize - argSize)));
			in.arguments.Add(n.argument);

			c.instructions.Add(in);

			info.stack.Push(info.stack.ptrSize);
			info.index = regIndex;
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

			c.AddInstructions(assign.symbolNode->Compile(assignArgs, info).instructions);

			info.stack.Push(type.size);
		}
	}

	info.stack.base = info.stack.top;
	const UInt stackSize = info.stack.top - stack.base;
	info.stack.Push(info.stack.ptrSize);

	if (!s.ret.IsEmpty()) {
		c.size = Symbol::FindNearestInNamespace(s.symbolNamespace, s.ret[0], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces)).size;
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

	c.argument = Argument(MemoryLocation(stack.Offset() - (s.ret.IsEmpty() ? retSize : Symbol::FindNearestInNamespace(s.symbolNamespace, s.ret[0], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces)).size)));

	info.stack = stack;
	return c;
}

void CallNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);

	for (NodePtr arg : args) {
		arg->IncludeScan(info);
	}
}

Set<ScanType> CallNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = node->Scan(info);

	if (info.Get().init && scanSet.Contains(ScanType::Self) && !info.Get().symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
	}

	Symbol s = GetFunc();

	for (const NodePtr& node : args) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}
	}

	for (UInt u = 0; u < s.args.Size(); u++) {
		Int i = IsInit() ? u - 1 : u;
		Symbol type = Symbol::FindNearestInNamespace(s.symbolNamespace, s.args[u], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces));
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