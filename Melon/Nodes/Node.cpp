#include "Node.h"

#include "Expression.h"
#include "Statement.h"
#include "Statements.h"
#include "Condition.h"
#include "TypeConversion.h"
#include "EmptyStatement.h"
#include "RootNode.h"
#include "KiwiVariable.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/NamespaceSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

#include "Boxx/Math.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RootNode* Node::_root = nullptr;

Node::Node(Symbol* const scope, const FileInfo& file) : scope(scope) {
	this->file = file;
}

Node::~Node() {

}

void Node::IncludeScan(ParsingInfo& info) {

}

ScanResult Node::Scan(ScanInfoStack& info) {
	return ScanResult();
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

void Node::Optimize(Ptr<Expression>& node, OptimizeInfo& info) {
	if (Ptr<Expression> n = node->Optimize(info)) {
		node = n;
	}
}

void Node::Optimize(Ptr<Condition>& node, OptimizeInfo& info) {
	if (Ptr<Condition> n = node->Optimize(info)) {
		node = n;
	}
}

void Node::Optimize(Ptr<Statement>& node, OptimizeInfo& info) {
	if (Ptr<Statement> n = node->Optimize(info)) {
		node = n;
	}
}

void Node::Optimize(Ptr<Statements>& node, OptimizeInfo& info) {
	if (Ptr<Statements> n = node->Optimize(info)) {
		node = n;
	}
}

void Node::Optimize(Ptr<Node>& node, OptimizeInfo& info) {
	if (Ptr<Expression> n = node.AsPtr<Expression>()) {
		Optimize(n, info);
		node = n;
	}
	else if (Ptr<Condition> n = node.AsPtr<Condition>()) {
		Optimize(n, info);
		node = n;
	}
	else if (Ptr<Statement> n = node.AsPtr<Statement>()) {
		Optimize(n, info);
		node = n;
	}
	else if (Ptr<Statements> n = node.AsPtr<Statements>()) {
		Optimize(n, info);
		node = n;
	}
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

ScanResult Node::ScanAssignment(Weak<Expression> assignable, Weak<Expression> value, ScanInfoStack& info, const FileInfo& file) {
	List<TypeSymbol*> args;
	args.Add(value->Type(info.PeekExpectedType()));

	if (TypeSymbol* const type = assignable->Type()) {
		if (FunctionSymbol* const func = type->Find<FunctionSymbol>(Name::Assign, file)) {
			func->FindOverload(args, file);
		}
	}

	return ScanResult();
}

Ptr<Kiwi::Value> Node::CompileAssignment(Weak<Expression> assignable, Weak<Expression> value, CompileInfo& info, const FileInfo& file, bool includeType) {
	if (FunctionSymbol* assignFunc = assignable->AssignFunc(value->Type(info.PeekExpectedType()))) {
		return assignable->CompileAssignFunc(assignFunc, value, info);
	}
	
	List<TypeSymbol*> args;
	args.Add(value->Type(assignable->Type()));

	FunctionSymbol* assign = nullptr;

	if (TypeSymbol* const type = assignable->Type()) {
		if (FunctionSymbol* const func = type->Find<FunctionSymbol>(Name::Assign, file)) {
			assign = func->FindOverload(args, file);

			if (assign && assign->symbolNode) {
				assign->symbolNode->Compile(assignable, value, info, includeType);
				return nullptr;
			}
		}
	}

	return CompileAssignmentSimple(assignable, value, info, file, includeType);
}

Ptr<Kiwi::Value> Node::CompileAssignmentSimple(Weak<Expression> assignable, Weak<Expression> value, CompileInfo& info, const FileInfo& file, bool includeType) {
	const Kiwi::Type type = assignable->Type()->KiwiType();

	bool assign = info.assign;
	info.assign = true;
	Ptr<Kiwi::Variable> kiwiVar = assignable->Compile(info).AsPtr<Kiwi::Variable>();
	info.assign = assign;

	info.PushExpectedType(assignable->Type());
	Ptr<Kiwi::Value> kiwiValue  = value->Compile(info);
	info.PopExpectedType();

	if (IncludeType(kiwiVar, includeType)) {
		info.currentBlock->AddInstruction(new Kiwi::AssignInstruction(type, kiwiVar, kiwiValue));
	}
	else {
		info.currentBlock->AddInstruction(new Kiwi::AssignInstruction(kiwiVar, kiwiValue));
	}

	return nullptr;
}

bool Node::IncludeType(Weak<Kiwi::Variable> var, bool includeType) {
	return includeType && !(var.Is<Kiwi::SubVariable>() || var.Is<Kiwi::DerefVariable>());
}

NameList Node::FindSideEffectScope(const bool assign) {
	return scope ? scope->AbsoluteName() : NameList();
}

void Node::Include(const Symbols::Name& name, ParsingInfo& info) {
	Include(NameList(name), info);
}

void Node::Include(const Symbols::NameList& name, ParsingInfo& info) {
	while (Symbol* s = SymbolTable::Find(name, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates)) {
		bool done = true;

		for (UInt i = 1; i < name.Size(); i++) {
			if (s->Is<NamespaceSymbol>()) {
				if (Symbol* const sym = s->Contains(name[i])) {
					s = sym;
				}
				else {
					IncludeParser::Include(s->AbsoluteName().Add(name[i]), info, false);
					done = false;
					break;
				}
			}
		}

		if (done) break;
	}
}

RootNode* Node::Root() {
	return _root;
}

bool Node::IsEmpty(Weak<Statement> statement) {
	if (Weak<EmptyStatement> empty = statement.As<EmptyStatement>()) {
		return !empty->statement;
	}
	
	return false;
}

bool Node::IsEmpty(Weak<Statements> statements) {
	for (Weak<Statement> statement : statements->statements) {
		if (!IsEmpty(statement)) return false;
	}

	return true;
}
