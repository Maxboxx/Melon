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
	List<NameList> types = ParseTypes(info);

	// Validate types
	if (!ValidateTypes(info, types, flags)) {
		info.index = startIndex;
		return nullptr;
	}

	bool singleType = types.Size() == 1;

	// Create assign node
	Pointer<Assignment> assign = new Assignment(info.scope, info.GetFileInfo(startLine));
	List<Tuple<Name, Symbol*>> symbols;

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

		ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("="), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
		info.index = startIndex;
		return nullptr;
	}

	info.index++;

	// Parse expressions
	ParseExpressions(info, assign);

	// Check if there are too many expressions
	if (assign->values.Size() > assign->vars.Size()) {
		ErrorLog::Error(LogMessage("error.syntax.assign.expr.many"), info.GetFileInfoPrev());
	}

	// Add symbols to scope
	for (const Tuple<Name, Symbol*>& symbol : symbols) {
		info.scope->AddSymbol(symbol.value1, symbol.value2);
	}

	info.statementNumber++;
	return assign;
}

List<NameList> AssignmentParser::ParseTypes(ParsingInfo& info) {
	const UInt startIndex = info.index;
	List<NameList> types;

	// Parse types
	while (true) {
		// Discard type
		if (info.Current().type == TokenType::Discard) {
			info.index++;
			types.Add(NameList::Discard);
		}
		// Parse type
		else if (Optional<NameList> type = TypeParser::Parse(info)) {
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

	if (info.Current().type == TokenType::Colon) {
		info.index++;
		return types;
	}

	info.index = startIndex;
	return List<NameList>();
}

bool AssignmentParser::ValidateTypes(ParsingInfo& info, List<NameList>& types, const Flags flags) {
	// Parse colon
	if (!types.IsEmpty()) {
		bool newVars = false;

		// Check if new variables will be created
		for (const NameList& type : types) {
			if (type != NameList::Discard) {
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
		types = List<NameList>();
		types.Add(NameList::Discard);
	}

	return true;
}

void AssignmentParser::ParseVariables(ParsingInfo& info, List<NameList>& types, Pointer<Assignment>& assign, List<Tuple<Name, Symbol*>>& symbols, const bool singleType) {
	for (UInt i = 0; true; i++) {
		if (i > 0) {
			if (info.Current().type != TokenType::Comma) {
				break;
			}

			info.index++;
		}

		if (i >= types.Size()) {
			if (singleType) {
				NameList last = types.Last();
				types.Add(last);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.assign.var.few"), info.GetFileInfoPrev());
			}
		}

		if (types[i] != NameList::Discard) {
			VariableAttributes attributes = VariableAttributeParser::Parse(info);
			Name name;

			if (info.Current().type == TokenType::Discard) {
				name = NameList::Discard.Last();
			}
			else if (info.Current().type == TokenType::Name) {
				name = Name(info.Current().value);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.name.var", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
			}

			info.index++;

			if (name != NameList::Discard.Last()) {
				FileInfo file = info.GetFileInfoPrev();
				file.statement++;

				Pointer<NameExpression> nn = new NameExpression(info.scope, file);
				nn->name = name;
				assign->vars.Add(nn);
			}
			else {
				assign->vars.Add(new DiscardExpression(info.scope, info.GetFileInfoPrev()));
			}

			if (name == NameList::Discard.Last()) continue;

			VariableSymbol* v = new VariableSymbol(info.GetFileInfoPrev());
			v->type = types[i];
			v->attributes = attributes;

			symbols.Add(Tuple<Name, Symbol*>(name, v));
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

void AssignmentParser::ParseExpressions(ParsingInfo& info, Pointer<Assignment>& assign) {
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
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "expression", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
		}
	}
}
