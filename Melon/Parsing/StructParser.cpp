#include "StructParser.h"

#include "NewVariableParser.h"
#include "FunctionParser.h"
#include "TemplateParser.h"

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
	if (info.Current().type != TokenType::Struct) return nullptr;

	const UInt structLine = info.Current().line;
	info.index++;

	Pointer<StructNode> sn = ParseName(info, structLine);

	info.scopes = info.scopes.Add(sn->name);
	Symbols::Add(info.scopes, sn->symbol, FileInfo(info.filename, info.Current().line, info.statementNumber));

	Symbols s = Symbols::Find(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));

	if (!s.templateVariants.IsEmpty()) {
		sn->name.variant = s.templateVariants.Size() - 1;
	}

	info.scopes = info.scopes.Pop().Add(sn->name);
	sn->symbol.scope = info.scopes;
	sn->symbol.varType = sn->symbol.scope;

	for (ScopeList& arg : sn->symbol.templateArgs) {
		arg = info.scopes.Add(arg);
	}

	while (true) {
		bool found = false;

		if (NodePtr node = ParseVariable(info)) {
			Pointer<NewVariableNode> nn = node.Cast<NewVariableNode>();

			for (UInt i = 0; i < nn->names.Size(); i++) {
				Symbols v = Symbols(SymbolType::Variable);
				v.symbolFile = info.currentFile;
				v.symbolNamespace = info.currentNamespace;
				v.includedNamespaces = info.includedNamespaces;

				if (nn->types.Size() == 1)
					v.varType = nn->types[0];
				else
					v.varType = nn->types[i];

				v.attributes = nn->attributes[i];

				sn->symbol.Add(nn->names[i], v, FileInfo(info.filename, info.Current().line, info.statementNumber));
				sn->vars.Add(nn->names[i]);
				sn->symbol.names.Add(nn->names[i]);
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

	Symbols assign = Symbols(SymbolType::Function);
	assign.symbolFile = info.currentFile;
	assign.symbolNamespace = info.currentNamespace;
	assign.includedNamespaces = info.includedNamespaces;
	assign.arguments.Add(info.scopes);
	assign.symbolNode = new StructAssignNode();
	sn->symbol.Add(Scope::Assign, assign, FileInfo(info.filename, info.Current().line, info.statementNumber));

	Symbols::Add(info.scopes, sn->symbol, FileInfo(info.filename, structLine, info.statementNumber), true);

	for (UInt i = 0; i < sn->symbol.templateArgs.Size(); i++) {
		Symbols t = Symbols(SymbolType::Template);
		t.templateIndex = i;
		t.scope = sn->symbol.scope.Add(sn->symbol.templateArgs[i].Last());
		t.varType = t.scope;
		sn->symbol.Add(sn->symbol.templateArgs[i].Last(), t, FileInfo(info.filename, structLine, info.statementNumber));
	}

	info.index++;

	info.scopes = info.scopes.Pop();
	return sn;
}

Pointer<StructNode> StructParser::ParseName(ParsingInfo& info, const UInt structLine) {
	static Regex lower = Regex("^%l");
	static Regex underscore = Regex("%a_+%a");

	if (info.Current().type != TokenType::Name)
		ErrorLog::Error(SyntaxError(SyntaxError::StructName, FileInfo(info.filename, structLine, info.statementNumber)));

	Scope structName = Scope(info.Current().value);

	if (lower.Match(info.Current().value)) {
		ErrorLog::Info(InfoError(InfoError::UpperName("struct", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber)));
	}

	if (underscore.Match(info.Current().value)) {
		ErrorLog::Info(InfoError(InfoError::UpperUnderscoreName("struct", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber)));
	}

	info.index++;
	Symbols structSymbol = Symbols(SymbolType::Struct);
	structSymbol.symbolFile = info.currentFile;
	structSymbol.symbolNamespace = info.currentNamespace;
	structSymbol.includedNamespaces = info.includedNamespaces;
	structSymbol.scope = info.scopes.Add(structName);
	
	if (Optional<List<Scope>> templateList = TemplateParser::ParseDefine(info)) {
		for (const Scope& arg : templateList.Get()) {
			structSymbol.templateArgs.Add(ScopeList().Add(arg));
		}

		structName.types = List<ScopeList>();
		info.statementNumber++;
	}

	Pointer<StructNode> sn = new StructNode(info.scopes, FileInfo(info.filename, structLine, info.statementNumber, info.currentNamespace, info.includedNamespaces));
	sn->name = structName;
	structSymbol.node = sn;
	sn->symbol = structSymbol;

	return sn;
}

NodePtr StructParser::ParseFunction(ParsingInfo& info) {
	if (NodePtr node = FunctionParser::Parse(info, false)) {
		if (Pointer<EmptyNode> en = node.Cast<EmptyNode>()) {
			if (Pointer<FunctionNode> fn = en->node.Cast<FunctionNode>()) {
				if (!fn->s.attributes.Contains(SymbolAttribute::Static)) {
					Symbols a = Symbols(SymbolType::Variable);
					a.symbolNamespace = info.currentNamespace;
					a.includedNamespaces = info.includedNamespaces;
					a.varType = info.scopes;
					a.attributes.Add(SymbolAttribute::Ref);
					//a.attributes.Add(SymbolAttribute::Const);	Add to class

					fn->s.arguments.Insert(0, info.scopes);
					fn->s.names.Insert(0, Scope::Self);
					fn->argNames.Insert(0, Scope::Self);

					Symbols::Add(fn->func, fn->s, FileInfo(info.filename, info.Current().line, info.statementNumber), true);
					fn->s = Symbols::Find(fn->func, FileInfo(info.filename, info.Current().line, info.statementNumber));
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