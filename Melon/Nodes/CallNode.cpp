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

Scope CallNode::GetFuncName() const {
	if (Pointer<NameNode> nn = node.Cast<NameNode>()) {
		return nn->name;
	}
	else if (Pointer<DotNode> nn = node.Cast<DotNode>()) {
		return nn->name;
	}
	else if (isMethod) {
		return methodName;
	}
	else {
		return Scope();
	}
}

Optional<List<TypeSymbol*>> CallNode::GetTemplateTypes(const Optional<List<ScopeList>>& types) const {
	if (!types) return nullptr;

	List<TypeSymbol*> args;

	for (const ScopeList& s : *types) {
		TypeSymbol* const type = SymbolTable::Find<TypeSymbol>(s, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

		if (!type) return nullptr;

		args.Add(type);
	}

	return args;
}

Optional<List<TypeSymbol*>> CallNode::GetArgumentTypes() const {
	List<TypeSymbol*> argTypes;

	for (NodePtr node : args) {
		if (TypeSymbol* const type = node->Type()) {
			argTypes.Add(type);
		}
		else {
			return nullptr;
		}
	}

	return argTypes;
}

FunctionSymbol* CallNode::GetOperatorFunction(const Optional<List<TypeSymbol*>>& templateTypes, const List<TypeSymbol*>& argTypes) const {
	if (TypeSymbol* const t = node->Type()) {
		if (FunctionSymbol* const f = t->Find<FunctionSymbol>(Scope::Call, node->file)) {
			if (templateTypes) {
				return f->FindMethodOverload(*templateTypes, argTypes, node->file);
			}
			else {
				return f->FindMethodOverload(argTypes, node->file);
			}
		}
	}

	return nullptr;
}

FunctionSymbol* CallNode::GetInitFunction(const Optional<List<TypeSymbol*>>& templateTypes, const List<TypeSymbol*>& argTypes) const {
	if (TypeSymbol* const t = node->GetSymbol()->Cast<TypeSymbol>()) {
		if (FunctionSymbol* const f = t->Find<FunctionSymbol>(Scope::Init, node->file)) {
			return f->FindMethodOverload(argTypes, node->file);
		}
	}

	return nullptr;
}

FunctionSymbol* CallNode::GetStaticFunction(const Optional<List<TypeSymbol*>>& templateTypes, const List<TypeSymbol*>& argTypes) const {
	if (FunctionSymbol* const f = node->GetSymbol()->Cast<FunctionSymbol>()) {
		if (templateTypes) {
			return f->FindStaticOverload(*templateTypes, argTypes, node->file);
		}
		else {
			return f->FindStaticOverload(argTypes, node->file);
		}
	}

	return nullptr;
}

FunctionSymbol* CallNode::GetMethod(const Optional<List<TypeSymbol*>>& templateTypes, const List<TypeSymbol*>& argTypes) const {
	if (TypeSymbol* const t = node->Type()) {
		if (FunctionSymbol* const f = t->Find<FunctionSymbol>(methodName, node->file)) {
			if (templateTypes) {
				return f->FindMethodOverload(*templateTypes, argTypes, node->file);
			}
			else {
				return f->FindMethodOverload(argTypes, node->file);
			}
		}
	}

	return nullptr;
}

FunctionSymbol* CallNode::GetFunctionSymbol(const Optional<List<TypeSymbol*>>& templateTypes, const List<TypeSymbol*>& argTypes) const {
	if (op) {
		return GetOperatorFunction(templateTypes, argTypes);
	}
	else if (IsInit()) {
		return GetInitFunction(templateTypes, argTypes);
	}
	else if (!isMethod) {
		return GetStaticFunction(templateTypes, argTypes);
	}
	else {
		return GetMethod(templateTypes, argTypes);
	}
}

FunctionSymbol* CallNode::GetFunc() const {
	List<TypeSymbol*> argTypes;
	Optional<List<TypeSymbol*>> templateTypes = nullptr;

	// Get function name
	Scope name = GetFuncName();

	// Get template types
	templateTypes = GetTemplateTypes(name.types);

	if (name.types && !templateTypes) return nullptr;

	// Get argument types
	if (Optional<List<TypeSymbol*>> types = GetArgumentTypes()) {
		argTypes = *types;
	}
	else {
		return nullptr;
	}

	// Get function symbol
	if (FunctionSymbol* const f = GetFunctionSymbol(templateTypes, argTypes)) {
		return f;
	}

	// Handle error
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
	return isMethod;
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

UInt CallNode::CalculateReturnSize(CallInfo& callInfo) {
	if (callInfo.isInit) return Type()->Size();

	UInt retSize = 0; 

	for (UInt i = 0; i < callInfo.func->returnValues.Size(); i++) {
		retSize += callInfo.func->ReturnType(i)->Size();
	}

	return retSize;
}

UInt CallNode::CalculateArgumentSize(CallInfo& callInfo, CompileInfo& info) {
	UInt argSize = 0;

	for (UInt i = 0; i < callInfo.func->arguments.Size(); i++) {
		if (VariableSymbol* const arg = callInfo.func->Argument(i)) {
			if (arg->HasAttribute(VariableAttributes::Ref)) {
				argSize += info.stack.ptrSize;
			}
			else {
				argSize += arg->Type()->Size();
			}
		}
	}

	return argSize;
}

UInt CallNode::CalculateTemporarySize(CallInfo& callInfo, CompileInfo info) {
	UInt tempSize = 0;

	// Check all arguments
	for (UInt i = 0; i < args.Size(); i++) {
		callInfo.memoryOffsets.Add(0);
		callInfo.assignFirst.Add(false);

		// Check if the argument is a reference
		if (callInfo.func->Argument(callInfo.isInit ? i + 1 : i)->HasAttribute(VariableAttributes::Ref)) {
			StackPtr stack = info.stack;
			CompiledNode n = args[i]->Compile(info);

			// Is value not stored in memory
			const bool notMemory = n.argument.type != ArgumentType::Memory;

			// Is the value stored above the top of the stack
			const bool aboveTop = (
				n.argument.mem.memptr.IsLeft() && 
				n.argument.mem.memptr.GetLeft().type == RegisterType::Stack &&
				stack.Offset(n.argument.mem.offset) >= stack.top
			);

			// Does the value need to be converted to a different type
			const bool needsConversion = !args[i]->Type()->IsOfType(callInfo.func->ArgumentType(i));

			// Check if the value needs to be assigned to temporary memory
			if (notMemory || aboveTop || needsConversion || noRefs[i]) {
				// Checks if a warning should be logged
				if (!noRefs[i] && !args[i]->IsImmediate() && !args[i].Is<ObjectInitNode>()) {
					bool error = true;

					if (Pointer<CallNode> call = args[i].Cast<CallNode>()) {
						error = !call->IsInit();
					}

					if (error) {
						ErrorLog::Warning(WarningError(WarningError::NoRefArg(callInfo.func->AbsoluteName().ToString(), i), args[i]->file));
					}
				}

				// Reserves space in temporary memory
				callInfo.assignFirst[callInfo.assignFirst.Size() - 1] = true;
				tempSize += callInfo.func->ArgumentType(callInfo.isInit ? i + 1 : i)->Size();
				callInfo.memoryOffsets[callInfo.memoryOffsets.Size() - 1] = tempSize;
			}
		}
	}

	return tempSize;
}

UInt CallNode::CalculatePushSize(CallInfo& callInfo, CompileInfo& info) {
	UInt pushSize = 0;

	// Changes stack top if push is not needed
	if (info.stack.top + callInfo.tempSize + callInfo.retSize + callInfo.argSize <= info.stack.frame) {
		pushSize = 0;
		info.stack.top = (Long)info.stack.frame - callInfo.tempSize - callInfo.retSize - callInfo.argSize;
	}
	// Calculates push size
	else {
		Long diff = ((Long)callInfo.tempSize + callInfo.retSize + callInfo.argSize) - info.stack.Offset();

		if (diff >= 0) {
			pushSize = diff;
		}
		else {
			info.stack.top = info.stack.frame;
			pushSize = callInfo.tempSize + callInfo.retSize + callInfo.argSize;
		}
	}

	return pushSize;
}

void CallNode::SetupStackFrame(CallInfo& callInfo, CompileInfo& info) {
	// Calculate sizes
	callInfo.retSize  = CalculateReturnSize(callInfo);
	callInfo.argSize  = CalculateArgumentSize(callInfo, info);
	callInfo.tempSize = CalculateTemporarySize(callInfo, info);
	callInfo.pushSize = CalculatePushSize(callInfo, info);

	// Set up stack frame
	callInfo.initialTop = info.stack.top;

	info.stack.Push(callInfo.tempSize + callInfo.retSize);
	info.stack.PushFrame(callInfo.pushSize);

	callInfo.frame      = info.stack.frame;
	callInfo.stackIndex = info.stack.Offset();

	// Add push instruction
	if (callInfo.pushSize > 0) {
		callInfo.cn.instructions.Add(Instruction(InstructionType::Push, callInfo.pushSize));
	}
}

NodePtr CallNode::GetRefArgument(CallInfo& callInfo, CompileInfo& info, TypeSymbol* const type, Int index) {
	// Create self for constructor
	if (index == -1) {
		Pointer<MemoryNode> sn = new MemoryNode(callInfo.stackIndex);
		return new RefNode(sn);
	}
	// Create reference to self
	else if (index == 0 && IsSelfPassing()) {
		return new RefNode(node);
	}
	// Create reference for regular argument
	else {
		if (IsSelfPassing()) index--;

		// Assign argument to temporary memory
		if (callInfo.assignFirst[index]) {
			Pointer<MemoryNode> sn = new MemoryNode(info.stack.Offset((Long)callInfo.initialTop + callInfo.memoryOffsets[index]));
			sn->type = type->AbsoluteName();

			UInt top = info.stack.top;
			callInfo.cn.AddInstructions(CompileAssignment(sn, args[index], info, args[index]->file).instructions);
			info.stack.top = top;

			return new RefNode(sn);
		}
		// Reference the argument normally
		else {
			return new RefNode(args[index]);
		}
	}
}

void CallNode::CompileRefArgument(CallInfo& callInfo, CompileInfo& info, TypeSymbol* const type, Int index) {
	NodePtr refArg = GetRefArgument(callInfo, info, type, index);

	UInt regIndex = info.index;
	callInfo.stackIndex -= info.stack.ptrSize;

	info.stack.Push(info.stack.ptrSize);

	CompiledNode n;

	// Compile self argument
	if (index == 0 && IsSelfPassing()) {
		StackPtr temp = info.stack;
		info.stack = callInfo.initialStack;

		n = refArg->Compile(info);

		info.stack = temp;

		for (UInt i = 0; i < n.instructions.Size(); i++) {
			callInfo.cn.instructions.Insert(i, n.instructions[i]);
		}
	}
	// Compile regular argument
	else {
		n = refArg->Compile(info);
		callInfo.cn.AddInstructions(n.instructions);
	}

	// Add argument pointer to stack
	Instruction in = Instruction(InstructionType::Mov, info.stack.ptrSize);
	in.arguments.Add(Argument(MemoryLocation(callInfo.stackIndex)));
	in.arguments.Add(n.argument);

	callInfo.cn.instructions.Add(in);

	info.index = regIndex;
}

void CallNode::CompileCopyArgument(CallInfo& callInfo, CompileInfo& info, TypeSymbol* const type, Int index) {
	callInfo.stackIndex -= type->Size();

	Pointer<MemoryNode> mn = new MemoryNode(callInfo.stackIndex);
	mn->type = type->AbsoluteName();

	info.stack.Push(type->Size());

	UInt frame = info.stack.frame;

	// Compile method argument
	if (isMethod) {
		callInfo.cn.AddInstructions(CompileAssignment(mn, this->args[index - 1], info, node->file).instructions);
	}
	// Compile regular argument
	else {
		callInfo.cn.AddInstructions(CompileAssignment(mn, this->args[index], info, node->file).instructions);
	}

	info.stack.PopExpr(frame, callInfo.cn);
}

void CallNode::CompileArguments(CallInfo& callInfo, CompileInfo& info) {
	for (UInt i = 0; i < callInfo.func->arguments.Size(); i++) {
		TypeSymbol* const type = callInfo.func->ArgumentType(i);

		// Compile ref argument
		if (callInfo.func->Argument(i)->HasAttribute(VariableAttributes::Ref)) {
			CompileRefArgument(callInfo, info, type, callInfo.isInit ? i - 1 : i);
		}
		// Compile copy argument
		else {
			CompileCopyArgument(callInfo, info, type, callInfo.isInit ? i - 1 : i);
		}
	}
}

void CallNode::CompileCall(CallInfo& callInfo, CompileInfo& info) {
	// Create call instruction
	Instruction inst = Instruction(InstructionType::Call);
	inst.arguments.Add(Argument(ArgumentType::Name, callInfo.func->AbsoluteName().ToString()));
	callInfo.cn.instructions.Add(inst);

	// Compile call statement cleanup
	if (isStatement) {
		UInt popSize = info.stack.frame - callInfo.initialStack.frame;

		if (popSize > 0) {
			callInfo.cn.instructions.Add(Instruction(InstructionType::Pop, popSize));
		}

		info.stack = callInfo.initialStack;
	}
	// Compile call expression cleanup
	else {
		info.stack.Pop(callInfo.argSize);

		if (callInfo.isInit) {
			callInfo.retSize = 0;
		}
		else {
			callInfo.retSize -= callInfo.func->ReturnType(0)->Size();
		}

		callInfo.cn.argument = Argument(MemoryLocation(info.stack.Offset() + callInfo.retSize));

		info.stack.top = callInfo.initialStack.top;
	}
}

CompiledNode CallNode::Compile(CompileInfo& info) { // TODO: more accurate arg error lines
	// Setup call info
	CallInfo callInfo;
	callInfo.func   = GetFunc();
	callInfo.isInit = IsInit();

	callInfo.cn = CompiledNode();
	callInfo.initialStack = info.stack;

	// Setup stack frame
	SetupStackFrame(callInfo, info);

	// Compile arguments
	CompileArguments(callInfo, info);

	// Get size of first return value
	if (!callInfo.func->returnValues.IsEmpty()) {
		callInfo.cn.size = callInfo.func->ReturnType(0)->Size();
	}
	else {
		callInfo.cn.size = info.stack.ptrSize;
	}
	
	// Pop stack back to previous frame size
	info.stack.PopExpr(callInfo.frame, callInfo.cn);

	// Compile call
	CompileCall(callInfo, info);

	return callInfo.cn;
}

void CallNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);

	for (NodePtr arg : args) {
		arg->IncludeScan(info);
	}

	GetFunc();
}

ScanResult CallNode::Scan(ScanInfoStack& info) {
	// Scan called node
	ScanResult result = node->Scan(info);
	result.SelfUseCheck(info, node->file);

	// Scan function
	FunctionSymbol* const func = GetFunc();
	if (func == nullptr) return ScanResult();

	if (info.UseFunction()) {
		if (!info.usedFunctions.Contains(func)) {
			info.usedFunctions.Add(func);
			info.functions.Add(func->node);
		}
	}

	// Scan arguments
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

	// Scan argument assignment
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