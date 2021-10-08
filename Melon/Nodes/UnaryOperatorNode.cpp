#include "UnaryOperatorNode.h"

#include "CallNode.h"
#include "TypeNode.h"
#include "BooleanNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

UnaryOperatorNode::UnaryOperatorNode(Symbol* const scope, const Name& op, const FileInfo& file) : Node(scope, file) {
	this->op = op;
}

UnaryOperatorNode::~UnaryOperatorNode() {

}

TypeSymbol* UnaryOperatorNode::Type() const {
	List<TypeSymbol*> args;
	args.Add(node->Type());

	TypeSymbol* const type = node->Type();

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

Symbol* UnaryOperatorNode::GetSymbol() const {
	if (op == Name::Unwrap) {
		if (TypeSymbol* const type = node->Type()) {
			return type->Find<VariableSymbol>(Name::Value, file);
		}
	}

	return nullptr;
}

Name UnaryOperatorNode::GetOperator() const {
	return op;
}

CompiledNode UnaryOperatorNode::Compile(CompileInfo& info) {
	List<NodePtr> nodes;
	nodes.Add(node);

	List<Symbol*> args;
	args.Add(node->Type());

	TypeSymbol* const type = node->Type();

	FunctionSymbol* const func = type->FindUnaryOperator(op, file);

	// Compile symbol node
	if (func->symbolNode) {
		return func->symbolNode->Compile(nodes, info);
	}
	// Compile operator function
	else {
		Pointer<CallNode> cn = new CallNode(scope, file);
		cn->arguments = nodes;
		cn->isMethod = false;

		Pointer<TypeNode> tn = new TypeNode(func->ParentType()->AbsoluteName());
		cn->expression = tn;
		cn->op = true;

		return cn->Compile(info);
	}
}

void UnaryOperatorNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);
}

ScanResult UnaryOperatorNode::Scan(ScanInfoStack& info) {
	ScanResult result = node->Scan(info);
	result.SelfUseCheck(info, node->file);

	if (op == Name::Unwrap) {
		// TODO: fix
		ErrorLog::Warning(LogMessage::Message("unwrap operator does not work properly for nil values"), file);
	}

	if (TypeSymbol* const type = node->Type()) {
		type->FindUnaryOperator(op, file);
	}

	return result;
}

NameList UnaryOperatorNode::FindSideEffectScope(const bool assign) {
	// TODO: Check operator function
	return node->GetSideEffectScope(assign);
}

NodePtr UnaryOperatorNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	// TODO: Add more operators
	if (node->IsImmediate()) {
		if (op == Name::Not) {
			Pointer<BooleanNode> bn = new BooleanNode(node->file);
			bn->boolean = node->GetImmediate() == 0;
			info.optimized = true;
			return bn;
		}
	}

	return nullptr;
}

StringBuilder UnaryOperatorNode::ToMelon(const UInt indent) const {
	if (op == Name::Unwrap) {
		StringBuilder sb = node->ToMelon(indent);
		sb += op.ToString();
		return sb;
	}
	else {
		StringBuilder sb = op.ToString();

		if (op == Name::Not) sb += " ";

		sb += node->ToMelon(indent);
		return sb;
	}
}