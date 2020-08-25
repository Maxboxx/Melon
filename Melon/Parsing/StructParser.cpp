#include "StructParser.h"

#include "NewVariableParser.h"
#include "FunctionParser.h"

#include "Melon/Symbols/Nodes/StructAssignNode.h"

#include "Melon/Nodes/StructNode.h"
#include "Melon/Nodes/NewVariableNode.h"
#include "Melon/Nodes/EmptyNode.h"
#include "Melon/Nodes/FunctionNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Parsing;

NodePtr StructParser::Parse(ParsingInfo& info) {
	static Regex lower = Regex("^%l");
	static Regex underscore = Regex("%a_+%a");

	if (info.Current().type != TokenType::Struct) return nullptr;

	const UInt structLine = info.Current().line;
	info.index++;

	if (info.Current().type != TokenType::Name)
		ErrorLog::Error(SyntaxError(SyntaxError::StructName, FileInfo(info.filename, structLine, info.statementNumber)));

	const Scope structName = Scope(info.Current().value);

	if (!lower.Match(info.Current().value).IsEmpty()) {
		ErrorLog::Info(InfoError(InfoError::UpperName("struct", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber)));
	}

	if (!underscore.Match(info.Current().value).IsEmpty()) {
		ErrorLog::Info(InfoError(InfoError::UpperUnderscoreName("struct", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber)));
	}

	info.index++;
	Symbol structSymbol = Symbol(SymbolType::Struct);
	structSymbol.symbolFile = info.currentFile;
	structSymbol.symbolNamespace = info.currentNamespace;
	structSymbol.includedNamespaces = info.includedNamespaces;
	structSymbol.scope = info.scopes.Add(structName);
	Pointer<StructNode> sn = new StructNode(info.scopes, FileInfo(info.filename, structLine, info.statementNumber));

	sn->name = structName;

	info.scopes = info.scopes.Add(structName);
	Symbol::Add(info.scopes, structSymbol, FileInfo(info.filename, info.Current().line, info.statementNumber));

	while (true) {
		bool found = false;

		if (NodePtr node = ParseVariable(info)) {
			Pointer<NewVariableNode> nn = node.Cast<NewVariableNode>();

			for (UInt i = 0; i < nn->names.Size(); i++) {
				Symbol v = Symbol(SymbolType::Variable);
				v.symbolFile = info.currentFile;
				v.symbolNamespace = info.currentNamespace;
				v.includedNamespaces = info.includedNamespaces;

				if (nn->types.Size() == 1)
					v.varType = nn->types[0];
				else
					v.varType = nn->types[i];

				v.attributes = nn->attributes[i];

				structSymbol.Add(nn->names[i], v, FileInfo(info.filename, info.Current().line, info.statementNumber));
				sn->vars.Add(nn->names[i]);
				structSymbol.args.Add(ScopeList().Add(nn->names[i]));
			}

			found = true;
		}
		else if (NodePtr node = ParseFunction(info)) {
			found = true;
		}

		if (!found) break;
	}

	if (info.Current().type != TokenType::End)
		ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("struct", structLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

	Symbol assign = Symbol(SymbolType::Function);
	assign.symbolFile = info.currentFile;
	assign.symbolNamespace = info.currentNamespace;
	assign.includedNamespaces = info.includedNamespaces;
	assign.args.Add(info.scopes);
	assign.node = new StructAssignNode();
	structSymbol.Add(Scope::Assign, assign, FileInfo(info.filename, info.Current().line, info.statementNumber));

	Symbol::Add(info.scopes, structSymbol, FileInfo(info.filename, structLine, info.statementNumber), true);

	info.index++;

	info.scopes = info.scopes.Pop();
	return sn;
}

NodePtr StructParser::ParseFunction(ParsingInfo& info) {
	if (NodePtr node = FunctionParser::Parse(info, false)) {
		if (Pointer<EmptyNode> en = node.Cast<EmptyNode>()) {
			if (Pointer<FunctionNode> fn = en->node.Cast<FunctionNode>()) {
				if (!fn->s.attributes.Contains(SymbolAttribute::Static)) {
					Symbol a = Symbol(SymbolType::Variable);
					a.symbolNamespace = info.currentNamespace;
					a.includedNamespaces = info.includedNamespaces;
					a.varType = info.scopes;
					a.attributes.Add(SymbolAttribute::Ref);
					//a.attributes.Add(SymbolAttribute::Const);	Add to class

					fn->s.args.Insert(0, info.scopes);
					fn->s.names.Insert(0, Scope::Self);
					fn->argNames.Insert(0, Scope::Self);

					Symbol::Add(fn->func, fn->s, FileInfo(info.filename, info.Current().line, info.statementNumber), true);
					fn->s = Symbol::Find(fn->func, FileInfo(info.filename, info.Current().line, info.statementNumber));
					fn->s.Add(Scope::Self, a, FileInfo(info.filename, info.Current().line, info.statementNumber));
				}

				return en;
			}
		}
	}

	return nullptr;
}

NodePtr StructParser::ParseVariable(ParsingInfo& info) {
	if (NodePtr node = NewVariableParser::Parse(info)) {
		return node;
	}

	return nullptr;
}