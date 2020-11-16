#include "AssignmentParser.h"

#include "TypeParser.h"
#include "ExpressionParser.h"
#include "AssignableParser.h"
#include "VariableAttributeParser.h"

#include "Melon/Nodes/AssignNode.h"
#include "Melon/Nodes/NameNode.h"
#include "Melon/Nodes/DiscardNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr AssignmentParser::Parse(ParsingInfo& info, const bool single, const bool newAssign) {
	const UInt startIndex = info.index;
	const UInt startLine = info.Current().line;

	List<ScopeList> types = ParseTypes(info);

	bool singleType = types.Size() == 1;

	if (!types.IsEmpty() && info.Current().type == TokenType::Colon) {
		info.index++;

		bool newVars = false;

		for (const ScopeList& type : types) {
			if (type != ScopeList::Discard) {
				newVars = true;
			}
		}

		if (newAssign && !newVars) {
			info.index = startIndex;
			return nullptr;
		}
	}
	else {
		if (newAssign) {
			info.index = startIndex;
			return nullptr;
		}

		info.index = startIndex;
		types = List<ScopeList>();
		types.Add(ScopeList::Discard);

		singleType = true;
	}

	Pointer<AssignNode> assign = new AssignNode(info.scopes, FileInfo(info.filename, startLine, info.statementNumber, info.currentNamespace, info.includedNamespaces));

	for (UInt i = 0; true; i++) {
		if (i > 0) {
			if (info.Current().type != TokenType::Comma) {
				break;
			}

			info.index++;
		}

		if (i >= types.Size()) {
			if (singleType) {
				ScopeList last = types.Last();
				types.Add(last);
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::FewVariables, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
		}

		if (types[i] != ScopeList::Discard) {
			Set<SymbolAttribute> attributes = VariableAttributeParser::Parse(info);
			Scope name;

			if (info.Current().type == TokenType::Discard) {
				name = ScopeList::Discard.Last();
			}
			else if (info.Current().type == TokenType::Name) {
				name = Scope(info.Current().value);
			}
			else {
				if (!attributes.IsEmpty()) {
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("variable name", "attributes"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				}
				else {
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("variable name", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				}
			}

			info.index++;

			if (name != ScopeList::Discard.Last()) {
				Pointer<NameNode> nn = new NameNode(info.scopes, FileInfo(info.filename, info.Current(-1).line, info.statementNumber, info.currentNamespace, info.includedNamespaces));
				nn->name = name;
				assign->vars.Add(nn);
			}
			else {
				assign->vars.Add(new DiscardNode(info.scopes, FileInfo(info.filename, info.Current(-1).line, info.statementNumber, info.currentNamespace, info.includedNamespaces)));
			}

			if (name == ScopeList::Discard.Last()) continue;

			Symbol v = Symbol(SymbolType::Variable);
			v.symbolFile = info.currentFile;
			v.symbolNamespace = info.currentNamespace;
			v.includedNamespaces = info.includedNamespaces;
			v.statementNumber = info.statementNumber;
			v.varType = types[i];
			v.attributes = attributes;

			Symbol::Add(info.scopes.Add(name), v, FileInfo(info.filename, info.Current().line, info.statementNumber));
		}
		else {
			if (NodePtr node = AssignableParser::Parse(info)) {
				assign->vars.Add(node);
			}
			else {
				break;
			}
		}
	}

	if (single && assign->vars.Size() > 1) {
		info.index = startIndex;
		return nullptr;
	}

	if (assign->vars.IsEmpty()) {
		info.index = startIndex;
		return nullptr;
	}

	assign->types = types;

	if (info.Current().type != TokenType::Assign) {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'='", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		info.index = startIndex;
		return nullptr;
	}

	info.index++;

	bool moreValues = true;

	for (UInt i = 0; i < assign->vars.Size(); i++) {
		if (moreValues) {
			if (NodePtr node = ExpressionParser::Parse(info)) {
				assign->values.Add(node);

				if (i < assign->vars.Size() - 1) {
					if (info.EndOfFile() || info.Current().type != TokenType::Comma) {
						if (node->Types().Size() + i < assign->vars.Size())
							ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("','", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
						moreValues = false;
					}
					else {
						info.index++;
					}
				}
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::FewExpressions, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
		}
	}

	info.statementNumber++;
	return assign;
}

List<ScopeList> AssignmentParser::ParseTypes(ParsingInfo& info) {
	List<ScopeList> types;

	while (true) {
		if (info.Current().type == TokenType::Discard) {
			info.index++;
			types.Add(ScopeList::Discard);
		}
		else if (Optional<ScopeList> type = TypeParser::Parse(info)) {
			types.Add(type.Get());
		}
		else {
			break;
		}

		if (info.Current().type == TokenType::Comma) {
			info.index++;
		}
		else {
			break;
		}
	}

	return types;
}