#include "CallNode.h"

#include "RefNode.h"
#include "StackNode.h"
#include "TypeNode.h"
#include "CustomInitNode.h"

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
	return Types()[0];
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
	else for (const ScopeList& type : s.returnValues) {
		const Symbol s2 = Symbol::FindNearestInNamespace(s.scope.Pop(), type, node->file);

		if (s2.type != SymbolType::None) {
			types.Add(s2.scope);
		}
		else {
			types.Add(ScopeList::undefined);
		}
	}

	if (types.IsEmpty()) {
		types.Add(ScopeList::undefined);
	}

	return types;
}

CompiledNode CallNode::Compile(CompileInfo& info) { // TODO: more accurate arg error lines
	Symbol s = GetFunc();

	StackPtr stack = info.stack;
	Int retSize = 0;
	Int argSize = 0;

	CompiledNode c;

	// Calculate return size
	for (const ScopeList& type : s.returnValues)
		retSize += Symbol::FindNearestInNamespace(s.scope.Pop(), type, FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces)).size;

	if (IsInit()) retSize = Symbol::Find(Type(), node->file).size;

	// Calculate arg size
	for (UInt u = 0; u < s.arguments.Size(); u++) {
		Symbol type = Symbol::FindNearestInNamespace(scope, s.arguments[u], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces));

		if (Symbol::Find(s.scope.Add(s.names[u]), node->file).attributes.Contains(SymbolAttribute::Ref)) {
			argSize += info.stack.ptrSize;
		}
		else {
			argSize += type.size;
		}
	}

	// Calculate temp size
	UInt tempSize = 0;
	List<UInt> memoryOffsets;
	List<bool> assignFirst;
	CompileInfo infoCpy = info;

	for (UInt i = 0; i < args.Size(); i++) {
		memoryOffsets.Add(0);
		assignFirst.Add(false);

		if (Symbol::Find(s.scope.Add(s.names[IsInit() ? i + 1 : i]), node->file).attributes.Contains(SymbolAttribute::Ref)) {
			UInt top = infoCpy.stack.top;
			CompiledNode n = args[i]->Compile(infoCpy);

			if (
				n.argument.type != ArgumentType::Memory || 
				(n.argument.mem.reg == RegisterType::Stack && n.argument.mem.offset >= top) ||
				!Symbol::IsOfType(args[i]->Type(), s.arguments[i], args[i]->file) ||
				noRefs[i]
			) {
				if (!noRefs[i] && !args[i]->IsImmediate() && !args[i].Is<CustomInitNode>()) {
					bool error = true;

					if (Pointer<CallNode> call = args[i].Cast<CallNode>()) {
						error = !call->IsInit();
					}

					if (error) {
						ErrorLog::Warning(WarningError(WarningError::NoRefArg(s.scope.ToString(), i), args[i]->file));
					}
				}

				assignFirst[assignFirst.Size() - 1] = true;

				tempSize += Symbol::Find(s.scope.Add(s.names[IsInit() ? i + 1 : i]), node->file).GetType(node->file).size;
				memoryOffsets[memoryOffsets.Size() - 1] = tempSize;
			}
		}
	}

	UInt pushSize = 0;

	if (info.stack.top + tempSize + retSize + argSize <= info.stack.frame) {
		pushSize = 0;
		info.stack.top = (Long)info.stack.frame - tempSize - retSize - argSize;
	}
	else {
		Long diff = ((Long)tempSize + retSize + argSize) - info.stack.Offset();

		if (diff >= 0) {
			pushSize = diff;
		}
		else {
			info.stack.top = info.stack.frame;
			pushSize = tempSize + retSize + argSize;
		}
	}

	const UInt tempStack = info.stack.top;

	info.stack.Push(tempSize + retSize);
	info.stack.PushFrame(pushSize);
	
	const UInt frame = info.stack.frame;

	UInt stackIndex = info.stack.Offset();

	if (pushSize > 0) {
		c.instructions.Add(Instruction(InstructionType::Push, pushSize));
	}

	// Calculate compile arguments
	for (UInt u = 0; u < s.arguments.Size(); u++) {
		Symbol type = Symbol::FindNearestInNamespace(scope, s.arguments[u], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces));

		if (Symbol::Find(s.scope.Add(s.names[u]), node->file).attributes.Contains(SymbolAttribute::Ref)) {
			NodePtr r = nullptr;
			Int i = IsInit() ? u - 1 : u;
			bool isCompiled = false;

			if (i == -1) {
				Pointer<StackNode> sn = new StackNode(stackIndex);
				r = new RefNode(sn);
			}
			else if (IsSelfPassing()) {
				if (i == 0) {
					isCompiled = true;
					r = new RefNode(node);
				}
				else {
					if (assignFirst[i - 1]) {
						Pointer<StackNode> sn = new StackNode(info.stack.Offset((Long)tempStack + memoryOffsets[i - 1]));
						sn->type = type.scope;

						UInt top = info.stack.top;
						c.AddInstructions(CompileAssignment(sn, args[i - 1], info, args[i - 1]->file).instructions);
						info.stack.top = top;

						r = new RefNode(sn);
					}
					else {
						r = new RefNode(args[i - 1]);
					}
				}
			}
			else {
				if (assignFirst[i]) {
					Pointer<StackNode> sn = new StackNode(info.stack.Offset((Long)tempStack + memoryOffsets[i]));
					sn->type = type.scope;

					UInt top = info.stack.top;
					c.AddInstructions(CompileAssignment(sn, args[i], info, args[i]->file).instructions);
					info.stack.top = top;

					r = new RefNode(sn);
				}
				else {
					r = new RefNode(args[i]);
				}
			}

			UInt regIndex = info.index;
			stackIndex -= info.stack.ptrSize;

			info.stack.Push(info.stack.ptrSize);

			CompiledNode n;
			
			if (isCompiled) {
				StackPtr temp = info.stack;
				info.stack = stack;

				n = r->Compile(info);

				info.stack = temp;

				for (UInt i = 0; i < n.instructions.Size(); i++) {
					c.instructions.Insert(i, n.instructions[i]);
				}
			}
			else {
				n = r->Compile(info);
				c.AddInstructions(n.instructions);
			}
			
			Instruction in = Instruction(InstructionType::Mov, info.stack.ptrSize);
			in.arguments.Add(Argument(MemoryLocation(stackIndex)));
			in.arguments.Add(n.argument);

			c.instructions.Add(in);

			info.index = regIndex;
		}
		else {
			Int i = IsInit() ? u - 1 : u;

			stackIndex -= type.size;

			Pointer<StackNode> sn = new StackNode(stackIndex);
			sn->type = type.scope;

			info.stack.Push(type.size);

			if (isMethod) {
				c.AddInstructions(CompileAssignment(sn, this->args[i - 1], info, node->file).instructions);
			}
			else {
				c.AddInstructions(CompileAssignment(sn, this->args[i], info, node->file).instructions);
			}
		}
	}

	if (!s.returnValues.IsEmpty()) {
		c.size = Symbol::FindNearestInNamespace(s.symbolNamespace, s.returnValues[0], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces)).size;
	}
	else {
		c.size = info.stack.ptrSize;
	}

	info.stack.PopExpr(frame, c);

	Instruction inst = Instruction(InstructionType::Call);
	inst.arguments.Add(Argument(ArgumentType::Function, s.size));
	c.instructions.Add(inst);

	if (isStatement) {
		UInt popSize = info.stack.frame - stack.frame;

		if (popSize > 0) {
			c.instructions.Add(Instruction(InstructionType::Pop, popSize));
		}

		info.stack = stack;
		return c;
	}
	else {
		info.stack.Pop(argSize);

		retSize -= Symbol::FindNearestInNamespace(s.scope.Pop(), s.returnValues[0], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces)).size;
		c.argument = Argument(MemoryLocation(info.stack.Offset() + retSize));

		info.stack.top = stack.top;
		return c;
	}
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

	for (UInt i = 0; i < args.Size(); i++) {
		if (s.names.Size() > i && !Symbol::Find(s.scope.Add(s.names[i]), node->file).attributes.Contains(SymbolAttribute::Ref)) {
			if (noRefs[i]) {
				ErrorLog::Error(CompileError(CompileError::InvalidNoRef, args[i]->file));
			}
		}

		for (const ScanType type : args[i]->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, args[i]->file));
			}
		}
	}

	for (UInt u = 0; u < s.arguments.Size(); u++) {
		Int i = IsInit() ? u - 1 : u;
		Symbol type = Symbol::FindNearestInNamespace(s.symbolNamespace, s.arguments[u], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces));
		Symbol::Find(s.scope.Add(s.names[u]), node->file);
		NodePtr arg;

		if (i < 0) continue;

		if (IsSelfPassing()) {
			if (i == 0) {
				arg = node;
			}
			else {
				arg = this->args[i - 1];
			}
		}
		else {
			arg = this->args[i];
		}

		ScanAssignment(new TypeNode(type.scope), arg, info, node->file);
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

StringBuilder CallNode::ToMelon(const UInt indent) const {
	StringBuilder sb = node->ToMelon(indent);
	sb += "(";

	for (UInt i = 0; i < args.Size(); i++) {
		if (i > 0) sb += ", ";
		if (noRefs[i]) sb += "noref ";
		sb += args[i]->ToMelon(indent);
	}

	sb += ")";

	return sb;
}