#include "CallNode.h"

#include "RefNode.h"
#include "MemoryNode.h"
#include "TypeNode.h"
#include "ObjectInitNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/StructSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

#include "NewVariableNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CallNode::CallNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

CallNode::~CallNode() {

}

FunctionSymbol* CallNode::GetFunc() const {
	List<TypeSymbol*> argTypes;

	for (NodePtr node : args) {
		argTypes.Add(node->Type());
	}

	FunctionSymbol* s = nullptr;

	if (op) {
		// TODO: fix
		//s = SymbolTable::FindFunction(type, argTypes, file);
	}
	else if (IsInit()) {
		// TODO: fix
		//s = Symbols::FindMethod(type.Add(Scope::Init).Add(Scope::Call), argTypes, this->node->file);
	}
	else if (!isMethod) {
		if (FunctionSymbol* const f = this->node->GetSymbol()->Cast<FunctionSymbol>()) {
			s = f->FindOverload(argTypes, this->node->file);

			if ((s->attributes & FunctionAttributes::Static) == FunctionAttributes::None) {
				s = nullptr;
			}
		}
	}
	else {
		// TODO: fix
		//s = Symbols::FindMethod(type.Add(methodName).Add(Scope::Call), argTypes, this->node->file);
	}

	if (s) {
		return s;
	}

	List<String> argStr;

	if (FunctionSymbol* const f = this->node->GetSymbol()->Cast<FunctionSymbol>()) {
		for (TypeSymbol* const type : argTypes) {
			argStr.Add(type->AbsoluteName().ToString());
		}

		ErrorLog::Error(SymbolError(SymbolError::Function(f->AbsoluteName().ToString(), argStr), file));
	}

	return nullptr;
}

bool CallNode::IsSelfPassing() const {
	if (isMethod) return true;
	return false;
}

bool CallNode::IsInit() const {
	Symbol* const s = node->GetSymbol();

	if (s->Is<StructSymbol>()) {
		return !isMethod;
	}

	return false;
}

TypeSymbol* CallNode::Type() const {
	return Types()[0];
}

List<TypeSymbol*> CallNode::Types() const {
	FunctionSymbol* const f = GetFunc();

	List<TypeSymbol*> types;

	if (f == nullptr) {
		types.Add(nullptr);
		return types;
	}

	if (IsInit()) {
		types.Add(node->Type());
	}
	else for (UInt i = 0; i < f->returnValues.Size(); i++) {
		types.Add(f->ReturnType(i));
	}

	if (types.IsEmpty()) {
		types.Add(nullptr);
	}

	return types;
}

CompiledNode CallNode::Compile(CompileInfo& info) { // TODO: more accurate arg error lines
	/* TODO: node
	Symbols s = GetFunc();

	StackPtr stack = info.stack;
	Int retSize = 0;
	Int argSize = 0;

	CompiledNode c;

	// Calculate return size
	for (const ScopeList& type : s.returnValues)
		retSize += Symbols::FindNearestInNamespace(s.scope.Pop(), type, FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces)).size;

	if (IsInit()) retSize = Symbols::Find(Type(), node->file).size;

	// Calculate arg size
	for (UInt u = 0; u < s.arguments.Size(); u++) {
		Symbols type = Symbols::FindNearestInNamespace(scope, s.arguments[u], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces));

		if (Symbols::Find(s.scope.Add(s.names[u]), node->file).attributes.Contains(SymbolAttribute::Ref)) {
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

		if (Symbols::Find(s.scope.Add(s.names[IsInit() ? i + 1 : i]), node->file).attributes.Contains(SymbolAttribute::Ref)) {
			StackPtr stack = infoCpy.stack;
			CompiledNode n = args[i]->Compile(infoCpy);

			if (
				n.argument.type != ArgumentType::Memory || 
				(n.argument.mem.memptr.IsLeft() && n.argument.mem.memptr.GetLeft().type == RegisterType::Stack && stack.Offset(n.argument.mem.offset) >= stack.top) ||
				!Symbols::IsOfType(args[i]->Type(), s.arguments[i], args[i]->file) ||
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

				tempSize += Symbols::Find(s.scope.Add(s.names[IsInit() ? i + 1 : i]), node->file).GetType(node->file).size;
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
		Symbols type = Symbols::FindNearestInNamespace(scope, s.arguments[u], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces));

		if (Symbols::Find(s.scope.Add(s.names[u]), node->file).attributes.Contains(SymbolAttribute::Ref)) {
			NodePtr r = nullptr;
			Int i = IsInit() ? u - 1 : u;
			bool isCompiled = false;

			if (i == -1) {
				Pointer<MemoryNode> sn = new MemoryNode(stackIndex);
				r = new RefNode(sn);
			}
			else if (IsSelfPassing()) {
				if (i == 0) {
					isCompiled = true;
					r = new RefNode(node);
				}
				else {
					if (assignFirst[i - 1]) {
						Pointer<MemoryNode> sn = new MemoryNode(info.stack.Offset((Long)tempStack + memoryOffsets[i - 1]));
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
					Pointer<MemoryNode> sn = new MemoryNode(info.stack.Offset((Long)tempStack + memoryOffsets[i]));
					sn->type = type.scope;

					StackPtr stack = info.stack;
					c.AddInstructions(CompileAssignment(sn, args[i], info, args[i]->file).instructions);
					info.stack.PopExpr(stack.frame, c);
					info.stack.top = stack.top;

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

			Pointer<MemoryNode> sn = new MemoryNode(stackIndex);
			sn->type = type.scope;

			info.stack.Push(type.size);

			UInt frame = info.stack.frame;

			if (isMethod) {
				c.AddInstructions(CompileAssignment(sn, this->args[i - 1], info, node->file).instructions);
			}
			else {
				c.AddInstructions(CompileAssignment(sn, this->args[i], info, node->file).instructions);
			}

			info.stack.PopExpr(frame, c);
		}
	}

	if (!s.returnValues.IsEmpty()) {
		c.size = Symbols::FindNearestInNamespace(s.symbolNamespace, s.returnValues[0], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces)).size;
	}
	else {
		c.size = info.stack.ptrSize;
	}

	info.stack.PopExpr(frame, c);

	Instruction inst = Instruction(InstructionType::Call);
	inst.arguments.Add(Argument(ArgumentType::Name, s.scope.ToString()));
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

		if (IsInit()) {
			retSize = 0;
		}
		else {
			retSize -= Symbols::FindNearestInNamespace(s.scope.Pop(), s.returnValues[0], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces)).size;
		}

		c.argument = Argument(MemoryLocation(info.stack.Offset() + retSize));

		info.stack.top = stack.top;
		return c;
	}
	*/

	return CompiledNode();
}

void CallNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);

	for (NodePtr arg : args) {
		arg->IncludeScan(info);
	}

	GetFunc();
}

Set<ScanType> CallNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = node->Scan(info);

	/* TODO: node
	if (info.Get().init && scanSet.Contains(ScanType::Self) && !info.Get().symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
	}

	Symbols s = GetFunc();

	if (info.Get().useFunction) {
		if (!info.usedFunctions.Contains(s.scope)) {
			info.usedFunctions.Add(s.scope);
			info.functions.Add(s.node);
		}
	}

	for (UInt i = 0; i < args.Size(); i++) {
		if (s.names.Size() > i && !Symbols::Find(s.scope.Add(s.names[i]), node->file).attributes.Contains(SymbolAttribute::Ref)) {
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
		Symbols type = Symbols::FindNearestInNamespace(s.symbolNamespace, s.arguments[u], FileInfo(node->file.filename, node->file.line, s.statementNumber, s.symbolNamespace, s.includedNamespaces));
		Symbols::Find(s.scope.Add(s.names[u]), node->file);
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
	*/

	return scanSet;
}

NodePtr CallNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	for (NodePtr& arg : args) {
		if (NodePtr node = arg->Optimize(info)) arg = node;
	}

	return nullptr;
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