#include "CallNode.h"

#include "RefNode.h"
#include "MemoryNode.h"
#include "TypeNode.h"
#include "ObjectInitNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/StructSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

#include "NewVariableNode.h"
#include "NameNode.h"
#include "DotNode.h"

#include "Boxx/Optional.h"

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
	Optional<List<TypeSymbol*>> templateArgs = nullptr;

	Scope name;

	if (Pointer<NameNode> nn = node.Cast<NameNode>()) {
		name = nn->name;
	}
	else if (Pointer<DotNode> nn = node.Cast<DotNode>()) {
		name = nn->name;
	}
	else if (isMethod) {
		name = methodName;
	}

	if (name.types) {
		List<TypeSymbol*> args;
		bool found = true;

		for (const ScopeList& s : name.types.Get()) {
			TypeSymbol* const t = SymbolTable::Find<TypeSymbol>(s, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

			if (t) {
				args.Add(t);
			}
			else {
				found = false;
			}
		}

		if (!found) {
			return nullptr;
		}

		templateArgs = args;
	}

	bool found = true;

	for (NodePtr node : args) {
		if (TypeSymbol* const type = node->Type()) {
			argTypes.Add(type);
		}
		else {
			found = false;
		}
	}

	if (!found) return nullptr;

	FunctionSymbol* s = nullptr;
	
	if (op) {
		if (TypeSymbol* const t = node->Type()) {
			if (FunctionSymbol* const f = t->Find<FunctionSymbol>(Scope::Call, node->file)) {
				if (templateArgs) {
					s = f->FindMethodOverload(templateArgs.Get(), argTypes, node->file);
				}
				else {
					s = f->FindMethodOverload(argTypes, node->file);
				}
			}
		}
	}
	else if (IsInit()) {
		if (TypeSymbol* const t = node->GetSymbol()->Cast<TypeSymbol>()) {
			if (FunctionSymbol* const f = t->Find<FunctionSymbol>(Scope::Init, node->file)) {
				s = f->FindMethodOverload(argTypes, node->file);
			}
		}
	}
	else if (!isMethod) {
		if (FunctionSymbol* const f = node->GetSymbol()->Cast<FunctionSymbol>()) {
			if (templateArgs) {
				s = f->FindStaticOverload(templateArgs.Get(), argTypes, node->file);
			}
			else {
				s = f->FindStaticOverload(argTypes, node->file);
			}
		}
	}
	else {
		if (TypeSymbol* const t = node->Type()) {
			if (FunctionSymbol* const f = t->Find<FunctionSymbol>(methodName, node->file)) {
				if (templateArgs) {
					s = f->FindMethodOverload(templateArgs.Get(), argTypes, node->file);
				}
				else {
					s = f->FindMethodOverload(argTypes, node->file);
				}
			}
		}
	}

	if (s) {
		return s;
	}

	List<String> argStr;

	if (FunctionSymbol* const f = node->GetSymbol()->Cast<FunctionSymbol>()) {
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
	FunctionSymbol* const func = GetFunc();
	const bool isInit = IsInit();

	StackPtr stack = info.stack;
	Int retSize = 0;
	Int argSize = 0;

	CompiledNode c;

	// Calculate return size
	for (UInt i = 0; i < func->returnValues.Size(); i++) {
		retSize += func->ReturnType(i)->Size();
	}

	if (isInit) retSize = Type()->Size();

	// Calculate arg size
	for (UInt i = 0; i < func->arguments.Size(); i++) {
		if (VariableSymbol* const arg = func->Argument(i)) {
			if (arg->HasAttribute(VariableAttributes::Ref)) {
				argSize += info.stack.ptrSize;
			}
			else {
				argSize += arg->Type()->Size();
			}
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

		if (func->Argument(isInit ? i + 1 : i)->HasAttribute(VariableAttributes::Ref)) {
			StackPtr stack = infoCpy.stack;
			CompiledNode n = args[i]->Compile(infoCpy);

			if (
				n.argument.type != ArgumentType::Memory || 
				(n.argument.mem.memptr.IsLeft() && n.argument.mem.memptr.GetLeft().type == RegisterType::Stack && stack.Offset(n.argument.mem.offset) >= stack.top) ||
				!args[i]->Type()->IsOfType(func->ArgumentType(i)) ||
				noRefs[i]
			) {
				if (!noRefs[i] && !args[i]->IsImmediate() && !args[i].Is<ObjectInitNode>()) {
					bool error = true;

					if (Pointer<CallNode> call = args[i].Cast<CallNode>()) {
						error = !call->IsInit();
					}

					if (error) {
						ErrorLog::Warning(WarningError(WarningError::NoRefArg(func->AbsoluteName().ToString(), i), args[i]->file));
					}
				}

				assignFirst[assignFirst.Size() - 1] = true;

				tempSize += func->ArgumentType(isInit ? i + 1 : i)->Size();
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
	for (UInt u = 0; u < func->arguments.Size(); u++) {
		TypeSymbol* const type = func->ArgumentType(u);

		if (func->Argument(u)->HasAttribute(VariableAttributes::Ref)) {
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
						sn->type = type->AbsoluteName();

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
					sn->type = type->AbsoluteName();

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

			stackIndex -= type->Size();

			Pointer<MemoryNode> sn = new MemoryNode(stackIndex);
			sn->type = type->AbsoluteName();

			info.stack.Push(type->Size());

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

	if (!func->returnValues.IsEmpty()) {
		c.size = func->ReturnType(0)->Size();
	}
	else {
		c.size = info.stack.ptrSize;
	}

	info.stack.PopExpr(frame, c);

	Instruction inst = Instruction(InstructionType::Call);
	inst.arguments.Add(Argument(ArgumentType::Name, func->AbsoluteName().ToString()));
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
			retSize -= func->ReturnType(0)->Size();
		}

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

	GetFunc();
}

ScanResult CallNode::Scan(ScanInfoStack& info) {
	ScanResult result = node->Scan(info);
	result.SelfUseCheck(info, node->file);

	FunctionSymbol* const func = GetFunc();
	if (func == nullptr) return ScanResult();

	if (info.UseFunction()) {
		if (!info.usedFunctions.Contains(func)) {
			info.usedFunctions.Add(func);
			info.functions.Add(func->node);
		}
	}

	for (UInt i = 0; i < args.Size(); i++) {
		VariableSymbol* const arg = func->Argument(i);

		if (arg && (arg->attributes & VariableAttributes::Ref) != VariableAttributes::None) {
			if (noRefs[i]) {
				ErrorLog::Error(CompileError(CompileError::InvalidNoRef, args[i]->file));
			}
		}

		ScanResult r = args[i]->Scan(info);
		r.SelfUseCheck(info, args[i]->file);
		result |= r;
	}

	const bool init = IsInit();

	for (UInt u = 0; u < func->arguments.Size(); u++) {
		Int i = init ? u - 1 : u;

		TypeSymbol* const type = func->ArgumentType(u);
		
		if (i < 0) continue;

		NodePtr arg;

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

		if (type) {
			ScanAssignment(new TypeNode(type->AbsoluteName()), arg, info, node->file);
		}
	}

	return result;
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

	if (isMethod) {
		sb += ":";
		sb += methodName.ToSimpleString();
	}

	sb += "(";

	for (UInt i = 0; i < args.Size(); i++) {
		if (i > 0) sb += ", ";
		if (noRefs[i]) sb += "noref ";
		sb += args[i]->ToMelon(indent);
	}

	sb += ")";

	return sb;
}