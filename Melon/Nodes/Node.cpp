#include "Node.h"

#include "ExpressionNode.h"
#include "ConvertNode.h"
#include "EmptyNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

#include "Boxx/Math.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

Node::Node(Symbol* const scope, const FileInfo& file) {
	this->scope = scope;
	this->file = file;
}

Node::~Node() {

}

void Node::IncludeScan(ParsingInfo& info) {

}

ScanResult Node::Scan(ScanInfoStack& info) {
	return ScanResult();
}

Node* Node::Optimize(OptimizeInfo& info) {
	return nullptr;
}

UInt Node::GetSize() const {
	return 0;
}

bool Node::HasSideEffects() {
	return HasSideEffects(scope->AbsoluteName());
}

bool Node::HasSideEffects(const NameList& scope) {
	NameList list = GetSideEffectScope(false);

	if (list.Size() == 1 && list[0] == Name::Global) return true;

	UInt len = Math::Min(list.Size(), scope.Size());

	for (UInt i = 0; i < len; i++) {
		if (list[i] != scope[i]) {
			return true;
		}
	}

	return list.Size() < scope.Size();
}

NameList Node::GetSideEffectScope(const bool assign) {
	if (!sideEffectScope) {
		sideEffectScope = FindSideEffectScope(assign);
	}

	return *sideEffectScope;
}

FileInfo Node::File() const {
	return file;
}

NameList Node::CombineSideEffects(const NameList& scope1, const NameList& scope2) {
	if (scope1.Size() == 1 && scope1[0] == Name::Global) return scope1;
	if (scope2.Size() == 1 && scope2[0] == Name::Global) return scope2;

	UInt len = Math::Min(scope1.Size(), scope2.Size());

	for (UInt i = 0; i < len; i++) {
		if (scope1[i] != scope2[i]) {
			return NameList(true);
		}
	}

	if (scope1.Size() < scope2.Size()) {
		return scope1;
	}
	else {
		return scope2;
	}
}

ScanResult Node::ScanAssignment(ExpressionNode* const assignable, ExpressionNode* const value, ScanInfoStack& info, const FileInfo& file) {
	List<TypeSymbol*> args;
	args.Add(value->Type());

	if (TypeSymbol* const type = assignable->Type()) {
		if (FunctionSymbol* const func = type->Find<FunctionSymbol>(Name::Assign, file)) {
			func->FindOverload(args, file);
		}
	}

	return ScanResult();
}

CompiledNode Node::CompileAssignment(ExpressionNode* const assignable, ExpressionNode* const value, CompileInfo& info, const FileInfo& file) {
	List<TypeSymbol*> args;
	args.Add(value->Type());

	FunctionSymbol* assign = nullptr;

	if (TypeSymbol* const type = assignable->Type()) {
		if (FunctionSymbol* const func = type->Find<FunctionSymbol>(Name::Assign, file)) {
			assign = func->FindOverload(args, file);
		}
	}

	if (assign) {
		List<ExpressionNode*> nodes;
		nodes.Add(assignable);

		ConvertNode* cn = new ConvertNode(value->scope, value->file);
		cn->isExplicit = false;
		cn->node = value;
		cn->type = assign->ArgumentType(0)->AbsoluteName();
		nodes.Add(cn);
		delete cn;

		return assign->symbolNode->Compile(nodes, info);
	}

	return CompiledNode();
}

bool Node::IsEmpty(Node* const node) {
	if (EmptyNode* const empty = node->Cast<EmptyNode>()) {
		return !empty->node;
	}

	return false;
}

NameList Node::FindSideEffectScope(const bool assign) {
	return scope->AbsoluteName();
}

