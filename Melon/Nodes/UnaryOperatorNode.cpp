#include "UnaryOperatorNode.h"

#include "CallNode.h"
#include "TypeNode.h"
#include "BooleanNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

UnaryOperatorNode::UnaryOperatorNode(Symbols::Symbol* const scope, const Name& op, const FileInfo& file) : ExpressionNode(scope, file) {
	this->op = op;
}

UnaryOperatorNode::~UnaryOperatorNode() {

}

TypeSymbol* UnaryOperatorNode::Type() const {
	List<TypeSymbol*> args;
	args.Add(operand->Type());

	TypeSymbol* const type = operand->Type();

	FunctionSymbol* const s = type->FindUnaryOperator(op, file);

	if (s && !s->returnValues.IsEmpty()) {
		TypeSymbol* const s2 = s->ReturnType(0);

		if (s2 && s2->Is<TemplateSymbol>()) {
			return s2->Type();
		}
		
		return s2;
	}

	return nullptr;
}

Symbol* UnaryOperatorNode::Symbol() const {
	if (op == Name::Unwrap) {
		if (TypeSymbol* const type = operand->Type()) {
			return type->Find<VariableSymbol>(Name::Value, file);
		}
	}

	return nullptr;
}

Name UnaryOperatorNode::GetOperator() const {
	return op;
}

CompiledNode UnaryOperatorNode::Compile(CompileInfo& info) {
	List<Expression> nodes;
	nodes.Add(operand);

	List<Symbols::Symbol*> args;
	args.Add(operand->Type());

	TypeSymbol* const type = operand->Type();

	FunctionSymbol* const func = type->FindUnaryOperator(op, file);

	// Compile symbol node
	if (func->symbolNode) {
		return func->symbolNode->Compile(nodes, info);
	}
	// Compile operator function
	else {
		Pointer<CallNode> cn = new CallNode(scope, file);
		cn->arguments = nodes;

		Pointer<TypeNode> tn = new TypeNode(func->ParentType()->AbsoluteName());
		cn->expression = tn;

		return cn->Compile(info);
	}
}

void UnaryOperatorNode::IncludeScan(ParsingInfo& info) {
	operand->IncludeScan(info);
}

ScanResult UnaryOperatorNode::Scan(ScanInfoStack& info) {
	ScanResult result = operand->Scan(info);
	result.SelfUseCheck(info, operand->File());

	if (op == Name::Unwrap) {
		// TODO: fix
		ErrorLog::Warning(LogMessage::Message("unwrap operator does not work properly for nil values"), file);
	}

	if (TypeSymbol* const type = operand->Type()) {
		type->FindUnaryOperator(op, file);
	}

	return result;
}

NameList UnaryOperatorNode::FindSideEffectScope(const bool assign) {
	// TODO: Check operator function
	return operand->GetSideEffectScope(assign);
}

Expression UnaryOperatorNode::Optimize(OptimizeInfo& info) {
	Node::Optimize(operand, info);

	// TODO: Add more operators
	if (operand->IsImmediate()) {
		if (op == Name::Not) {
			Pointer<BooleanNode> bn = new BooleanNode(operand->File());
			bn->boolean = operand->GetImmediate() == 0;
			info.optimized = true;
			return bn;
		}
	}

	return nullptr;
}

StringBuilder UnaryOperatorNode::ToMelon(const UInt indent) const {
	if (op == Name::Unwrap) {
		StringBuilder sb = operand->ToMelon(indent);
		sb += op.ToString();
		return sb;
	}
	else {
		StringBuilder sb = op.ToString();

		if (op == Name::Not) sb += " ";

		sb += operand->ToMelon(indent);
		return sb;
	}
}