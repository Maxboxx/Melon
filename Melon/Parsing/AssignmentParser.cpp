#include "AssignmentParser.h"

#include "TypeParser.h"
#include "ExpressionParser.h"
#include "AssignableParser.h"
#include "VariableAttributeParser.h"

#include "Melon/Nodes/AssignNode.h"
#include "Melon/Nodes/NameNode.h"
#include "Melon/Nodes/DiscardNode.h"

#include "Melon/Symbols/VariableSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr AssignmentParser::Parse(ParsingInfo& info, const Flags flags) {
	const UInt startIndex = info.index;
	const UInt startLine = info.Current().line;

	// Parse types
	List<ScopeList> types = ParseTypes(info);

	// Validate types
	if (!ValidateTypes(info, types, flags)) {
		info.index = startIndex;
		return nullptr;
	}

	bool singleType = types.Size() == 1;

	// Create assign node
	Pointer<AssignNode> assign = new AssignNode(info.scope, info.GetFileInfo(startLine));
	List<Tuple<Scope, Symbol*>> symbols;

	// Parse variables
	ParseVariables(info, types, assign, symbols, singleType);

	// Check for single flag
	if ((flags & Flags::Single) != Flags::None && assign->vars.Size() > 1) {
		info.index = startIndex;
		return nullptr;
	}

	// Break if assignment is empty
	if (assign->vars.IsEmpty()) {
		info.index = startIndex;
		return nullptr;
	}

	assign->types = types;

	// Check for assignment token
	if (info.Current().type != TokenType::Assign) {
		if ((flags & Flags::MethodCall) != Flags::None && info.Current().type == TokenType::ParenOpen) {
			info.index = startIndex;
			return nullptr;
		}

		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'='", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		info.index = startIndex;
		return nullptr;
	}

	info.index++;

	// Parse expressions
	ParseExpressions(info, assign);

	// Check if there are too many expressions
	if (assign->values.Size() > assign->vars.Size()) {
		ErrorLog::Error(SyntaxError(SyntaxError::ManyExprAssign, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
	}

	// Add symbols to scope
	for (const Tuple<Scope, Symbol*>& symbol : symbols) {
		info.scope->AddSymbol(symbol.value1, symbol.value2);
	}

	info.statementNumber++;
	return assign;
}

List<ScopeList> AssignmentParser::ParseTypes(ParsingInfo& info) {
	List<ScopeList> types;

	// Parse types
	while (true) {
		// Discard type
		if (info.Current().type == TokenType::Discard) {
			info.index++;
			types.Add(ScopeList::Discard);
		}
		// Parse type
		else if (Optional<ScopeList> type = TypeParser::Parse(info)) {
			types.Add(*type);
		}
		// Invalid type
		else {
			break;
		}

		// Check for comma
		if (info.Current().type == TokenType::Comma) {
			info.index++;
		}
		else {
			break;
		}
	}

	return types;
}

bool AssignmentParser::ValidateTypes(ParsingInfo& info, List<ScopeList>& types, const Flags flags) {
	// Parse colon
	if (!types.IsEmpty() && info.Current().type == TokenType::Colon) {
		info.index++;

		bool newVars = false;

		// Check if new variables will be created
		for (const ScopeList& type : types) {
			if (type != ScopeList::Discard) {
				newVars = true;
			}
		}

		// Break if new assign is required and no vars are created
		if ((flags & Flags::NewAssign) != Flags::None && !newVars) {
			return false;
		}
	}
	// Check if assign is valid
	else {
		if ((flags & Flags::NewAssign) != Flags::None) {
			return false;
		}

		// Set types to discard
		types = List<ScopeList>();
		types.Add(ScopeList::Discard);
	}

	return true;
}

void AssignmentParser::ParseVariables(ParsingInfo& info, List<ScopeList>& types, Pointer<AssignNode>& assign, List<Tuple<Scope, Symbol*>>& symbols, const bool singleType) {
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
			VariableAttributes attributes = VariableAttributeParser::Parse(info);
			Scope name;

			if (info.Current().type == TokenType::Discard) {
				name = ScopeList::Discard.Last();
			}
			else if (info.Current().type == TokenType::Name) {
				name = Scope(info.Current().value);
			}
			else {
				if (attributes != VariableAttributes::None) {
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("variable name", "attributes"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				}
				else {
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("variable name", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				}
			}

			info.index++;

			if (name != ScopeList::Discard.Last()) {
				FileInfo file = info.GetFileInfo(info.Current(-1).line);
				file.statement++;

				Pointer<NameNode> nn = new NameNode(info.scope, file);
				nn->name = name;
				assign->vars.Add(nn);
			}
			else {
				assign->vars.Add(new DiscardNode(info.scope, info.GetFileInfo(info.Current(-1).line)));
			}

			if (name == ScopeList::Discard.Last()) continue;

			VariableSymbol* v = new VariableSymbol(info.GetFileInfo(info.Current(-1).line));
			v->type = types[i];
			v->attributes = attributes;

			symbols.Add(Tuple<Scope, Symbol*>(name, v));
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
}

void AssignmentParser::ParseExpressions(ParsingInfo& info, Pointer<AssignNode>& assign) {
	for (UInt i = 0; true; i++) {
		if (i > 0) {
			if (info.EndOfFile() || info.Current().type != TokenType::Comma) {
				break;
			}

			info.index++;
		}

		if (NodePtr node = ExpressionParser::Parse(info)) {
			assign->values.Add(node);
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("expression", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}
	}
}