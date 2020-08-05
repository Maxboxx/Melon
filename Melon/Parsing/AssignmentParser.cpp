#include "AssignmentParser.h"

#include "NewVariableParser.h"
#include "ExpressionParser.h"
#include "AssignableParser.h"

#include "Melon/Nodes/AssignNode.h"
#include "Melon/Nodes/NameNode.h"
#include "Melon/Nodes/NewVariableNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr AssignmentParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;
	const UInt startLine = info.Current().line;

	if (NodePtr node = NewVariableParser::Parse(info)) {
		Pointer<NewVariableNode> nn = node.Cast<NewVariableNode>();

		for (UInt i = 0; i < nn->names.Size(); i++) {
			Symbol v = Symbol(SymbolType::Variable);
			v.symbolFile = info.currentFile;
			v.symbolNamespace = info.currentNamespace;
			v.includedNamespaces = info.includedNamespaces;
			v.statementNumber = info.statementNumber;

			if (nn->types.Size() == 1)
				v.varType = nn->types[0];
			else
				v.varType = nn->types[i];

			v.attributes = nn->attributes[i];

			Symbol::Add(info.scopes.Add(nn->names[i]), v, FileInfo(info.filename, info.Current().line, info.statementNumber));
		}

		if (info.Current().type == TokenType::Assign) {
			info.index++;

			bool moreValues = true;

			Pointer<AssignNode> assign = new AssignNode(info.scopes, FileInfo(info.filename, startLine, info.statementNumber, info.currentNamespace, info.includedNamespaces));
			Pointer<NewVariableNode> vars = node.Cast<NewVariableNode>();
			assign->newVars = vars;

			for (UInt i = 0; i < vars->names.Size(); i++) {
				Pointer<NameNode> name = new NameNode(info.scopes, FileInfo(info.filename, startLine, info.statementNumber + 1, info.currentNamespace, info.includedNamespaces)); // TODO: more accurate line
				name->name = vars->names[i];
				assign->vars.Add(name);

				if (moreValues) {
					if (NodePtr node2 = ExpressionParser::Parse(info)) {
						assign->values.Add(node2);

						if (i < vars->names.Size() - 1) {
							if (info.EndOfFile() || info.Current().type != TokenType::Comma) {
								if (node2->Types().Size() + i < vars->names.Size())
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

		info.statementNumber++;
		return node;
	}
	else {
		List<NodePtr> nodes;

		if (NodePtr node = AssignableParser::Parse(info))
			nodes.Add(node);
		else {
			return nullptr;
		}

		while (info.Current().type == TokenType::Comma) {
			info.index++;

			if (NodePtr node = AssignableParser::Parse(info))
				nodes.Add(node);
			else {
				return Parser::UnexpectedToken(info);
			}
		}

		if (info.Current().type == TokenType::Assign) {
			info.index++;

			bool moreValues = true;

			Pointer<AssignNode> assign = new AssignNode(info.scopes, FileInfo(info.filename, startLine, info.statementNumber));

			for (UInt i = 0; i < nodes.Size(); i++) {
				assign->vars.Add(nodes[i]);

				if (moreValues) {
					if (NodePtr node2 = ExpressionParser::Parse(info)) {
						assign->values.Add(node2);

						if (i < nodes.Size() - 1) {
							if (info.EndOfFile() || info.Current().type != TokenType::Comma) {
								if (node2->Types().Size() + i < nodes.Size())
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
	}

	info.index = startIndex;
	return nullptr;
}