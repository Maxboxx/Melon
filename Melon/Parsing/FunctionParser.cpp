#include "FunctionParser.h"

#include "TypeParser.h"
#include "VariableAttributeParser.h"
#include "StatementParser.h"
#include "TemplateParser.h"

#include "Melon/Nodes/FunctionNode.h"
#include "Melon/Nodes/EmptyNode.h"

#include "Melon/Symbols/MapSymbol.h";
#include "Melon/Symbols/TemplateSymbol.h";
#include "Melon/Symbols/StructSymbol.h";
#include "Melon/Symbols/EnumSymbol.h";

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr FunctionParser::Parse(ParsingInfo& info, TypeSymbol* const parent) {
	const UInt startIndex = info.index;
	const UInt startLine = info.Current().line;

	// TODO: More accurate lines
	MapSymbol* const temp = info.scope;
	FunctionSymbol* funcSym = new FunctionSymbol(info.GetFileInfo(startLine));
	info.scope = funcSym;

	if (Optional<FunctionHead> fh = ParseFunctionHead(info, parent == nullptr)) {
		const FunctionHead funcHead = (FunctionHead)fh;
		Pointer<FunctionNode> func = new FunctionNode(info.scope, info.GetFileInfo(startLine));

		UInt line = 0;

		MapSymbol* const parentSym = parent ? parent->Cast<MapSymbol>() : info.scope;
		FunctionSymbol* functionParent = nullptr;

		if (Symbol* const func = parentSym->Contains(funcHead.name)) {
			functionParent = func->Cast<FunctionSymbol>();
		}
		else {
			functionParent = parentSym->AddSymbol(funcHead.name, new FunctionSymbol(info.GetFileInfo(startLine)))->Cast<FunctionSymbol>();
		}

		funcSym = functionParent->AddOverload(funcSym);
		funcSym->returnValues = funcHead.returnTypes;
		funcSym->attributes   = funcHead.attributes;

		for (const ScopeList& arg : funcHead.templateArgs) {
			if (arg[0].IsEmpty()) {
				funcSym->AddSymbol(arg[1], new TemplateSymbol(info.GetFileInfo(startLine)));
			}

			funcSym->templateArguments.Add(arg);
		}

		if (funcHead.isMethod) {
			FileInfo file = info.GetFileInfo(startLine);
			file.statement = 0;
			VariableSymbol* const var = new VariableSymbol(file);
			var->type = parent->AbsoluteName();

			if (parent->Is<StructSymbol>()) {
				var->attributes = VariableAttributes::Ref;
			}
			else if (parent->Is<EnumSymbol>()) {
				var->attributes = VariableAttributes::Ref;
			}

			funcSym->AddSymbol(Scope::Self, var);
			funcSym->arguments.Add(ScopeList().Add(Scope::Self));
		}

		for (const Argument& arg : funcHead.arguments) {
			VariableSymbol* const var = new VariableSymbol(info.GetFileInfo(startLine));
			var->type = arg.type;
			var->attributes = arg.attributes;
			funcSym->AddSymbol(arg.name, var);

			funcSym->arguments.Add(ScopeList().Add(arg.name));
		}

		UInt loops = info.loops;
		UInt scopeCount = info.scopeCount;
		info.loops = 0;
		info.scopeCount = 0;
		bool single = info.Current().type == TokenType::Arrow;

		if (single) {
			info.index++;

			if (NodePtr statement = StatementParser::Parse(info)) {
				func->node = statement;
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("statement", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}
		}
		else {
			func->node = StatementParser::ParseMultiple(info);
		}

		func->sym = funcSym;
		funcSym->node = func;

		info.loops = loops;
		info.scopeCount = scopeCount;

		if (!single && info.Current().type != TokenType::End) {
			ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("function", line), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}

		if (!single) info.index++;

		info.scope = temp;
		info.root.funcs.Add(func);
		info.statementNumber++;

		Pointer<EmptyNode> en = new EmptyNode();
		en->node = func;
		return en;
	}

	info.scope = temp;
	info.index = startIndex;
	return nullptr;
}

Optional<Scope> FunctionParser::ParseFunctionName(ParsingInfo& info, const bool isPlain) {
	if (isPlain) {
		if (info.Current().type == TokenType::Name) {
			const Scope s = Scope(info.Current().value);
			info.index++;
			return s;
		}
	}
	else {
		if (info.Current().type == TokenType::Name) {
			const Scope s = Scope(info.Current().value);
			info.index++;
			return s;
		}
		else if (info.Current().type == TokenType::Init) {
			const Scope s = Scope::Init;
			info.index++;
			return s;
		}
	}

	return nullptr;
}

Optional<Scope> FunctionParser::ParseOperatorName(ParsingInfo& info) {
	switch (info.Current().type) {
		// Math
		case TokenType::Plus: {
			info.index++;
			return Scope::Add;
		}
		case TokenType::Minus: {
			info.index++;
			return Scope::Sub;
		}
		case TokenType::Mul: {
			info.index++;
			return Scope::Mul;
		}
		case TokenType::Div: {
			info.index++;
			return Scope::Div;
		}
		case TokenType::IDiv: {
			info.index++;
			return Scope::IDiv;
		}
		case TokenType::Mod: {
			info.index++;
			return Scope::Mod;
		}

		// Bitwise
		case TokenType::BNot: {
			info.index++;
			return Scope::BitNot;
		}
		case TokenType::BOr: {
			info.index++;
			return Scope::BitOr;
		}
		case TokenType::BAnd: {
			info.index++;
			return Scope::BitAnd;
		}
		case TokenType::BXor: {
			info.index++;
			return Scope::BitXor;
		}
		case TokenType::BNor: {
			info.index++;
			return Scope::BitNor;
		}
		case TokenType::BNand: {
			info.index++;
			return Scope::BitNand;
		}
		case TokenType::BXnor: {
			info.index++;
			return Scope::BitXnor;
		}
		case TokenType::BShiftLeft: {
			info.index++;
			return Scope::ShiftLeft;
		}
		case TokenType::BShiftRight: {
			info.index++;
			return Scope::ShiftRight;
		}

		// Compare
		case TokenType::Equal: {
			info.index++;
			return Scope::Equal;
		}
		case TokenType::NotEqual: {
			info.index++;
			return Scope::NotEqual;
		}
		case TokenType::Less: {
			info.index++;
			return Scope::Less;
		}
		case TokenType::LessEq: {
			info.index++;
			return Scope::LessEqual;
		}
		case TokenType::Greater: {
			info.index++;
			return Scope::Greater;
		}
		case TokenType::GreaterEq: {
			info.index++;
			return Scope::GreaterEqual;
		}

		// Misc
		case TokenType::ParenOpen: {
			if (info.Current(1).type == TokenType::ParenClose) {
				info.index += 2;
				return Scope::Call;
			}

			return nullptr;
		}
		case TokenType::Len: {
			info.index++;
			return Scope::Len;
		}
		case TokenType::SquareOpen: {
			if (info.Current(1).type == TokenType::SquareClose) {
				info.index += 2;
				return Scope::Index;
			}

			return nullptr;
		}
	}

	return nullptr;
}

Optional<Scope> FunctionParser::ParseName(const bool isOperator, ParsingInfo& info, const bool isPlain) {
	static Regex upper = Regex("^%u");
	static Regex underscore = Regex("%a_+%a");

	if (!isOperator) {
		if (Optional<Scope> fname = ParseFunctionName(info, isPlain)) {
			const Scope name = (Scope)fname;

			if (upper.Match(name.name)) {
				ErrorLog::Info(InfoError(InfoError::LowerName("function", name.name), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}

			if (underscore.Match(name.name)) {
				ErrorLog::Info(InfoError(InfoError::LowerUnderscoreName("function", name.name), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}

			return name;
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("function name", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			return nullptr;
		}
	}
	else {
		if (Optional<Scope> name = ParseOperatorName(info)) {
			return (Scope)name;
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("operator", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			return nullptr;
		}
	}
}

FunctionAttributes FunctionParser::ParseAttributes(ParsingInfo& info, const bool isPlain) {
	FunctionAttributes attributes = FunctionAttributes::None;

	bool loop = !isPlain;

	while (loop) {
		// TODO: Error for invalid attributes
		switch (info.Current().type) {
			case TokenType::Static: {
				if ((attributes & FunctionAttributes::Static) != FunctionAttributes::None) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("static"), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}
				
				attributes |= FunctionAttributes::Static;
				break;
			}
			case TokenType::Override: {
				if ((attributes & FunctionAttributes::Override) != FunctionAttributes::None) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("override"), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}

				attributes |= FunctionAttributes::Override;
				break;
			}
			case TokenType::Required: {
				if ((attributes & FunctionAttributes::Required) != FunctionAttributes::None) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("required"), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}

				attributes |= FunctionAttributes::Required;
				break;
			}
			case TokenType::Debug: {
				if ((attributes & FunctionAttributes::Debug) != FunctionAttributes::None) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("debug"), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}

				attributes |= FunctionAttributes::Debug;
				break;
			}
			default: {
				loop = false;
				info.index--;
				break;
			}
		}

		info.index++;
	}

	return attributes;
}

List<ScopeList> FunctionParser::ParseReturnTypes(ParsingInfo& info) {
	const UInt retIndex = info.index;
	List<ScopeList> returnTypes;

	while (true) {
		if (!returnTypes.IsEmpty()) {
			if (info.Current().type == TokenType::Comma) {
				info.index++;
			}
			else if (info.Current().type == TokenType::Colon) {
				info.index++;
				break;
			}
			else {
				if (returnTypes.Size() > 1) {
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("':'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				}

				info.index = retIndex;
				returnTypes = List<ScopeList>();

				break;
			}
		}

		if (Optional<ScopeList> type = TypeParser::Parse(info)) {
			returnTypes.Add((ScopeList)type);
		}
		else {
			break;
		}
	}

	return returnTypes;
}

List<FunctionParser::Argument> FunctionParser::ParseArguments(ParsingInfo& info) {
	List<Argument> arguments;

	if (info.Current().type != TokenType::ParenOpen) {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'('", "function name"), info.GetFileInfo(info.Current(-1).line)));
		return arguments;
	}

	info.index++;

	while (info.Current().type != TokenType::ParenClose) {
		if (!arguments.IsEmpty()) {
			if (info.Current().type != TokenType::Comma) {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("')'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}

			info.index++;
		}

		Optional<ScopeList> type = TypeParser::Parse(info);

		if (type && info.Current().type == TokenType::Colon) {
			info.index++;

			Argument argument;
			argument.type       = (ScopeList)type;
			argument.attributes = VariableAttributeParser::Parse(info, true);

			if (info.Current().type != TokenType::Name) {
				ErrorLog::Error(SyntaxError(SyntaxError::ArgNameExpected, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
			}

			argument.name = Scope(info.Current().value);
			arguments.Add(argument);

			info.index++;
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("')'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}
	}

	info.index++;
	return arguments;
}

Optional<FunctionParser::FunctionHead> FunctionParser::ParseFunctionHead(ParsingInfo& info, const bool isPlain) {
	const UInt startIndex = info.index;
	const UInt startLine = info.Current().line;

	FunctionHead funcHead;
	funcHead.attributes = ParseAttributes(info, isPlain);
	funcHead.isMethod   = !isPlain && (funcHead.attributes & FunctionAttributes::Static) == FunctionAttributes::None;

	if (info.Current().type != TokenType::Function) {
		if (funcHead.attributes != FunctionAttributes::None) {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'function'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}

		return nullptr;
	}

	if (info.Next().type == TokenType::Operator && !isPlain) {
		funcHead.isOperator = true;
		info.index++;
	}
	else {
		funcHead.isOperator = false;
	}

	funcHead.returnTypes = ParseReturnTypes(info);
	
	if (Optional<Scope> name = ParseName(funcHead.isOperator, info, isPlain)) {
		funcHead.name = (Scope)name;
	}

	if (Optional<List<ScopeList>> templateArgs = TemplateParser::ParseDefine(info)) {
		funcHead.templateArgs = templateArgs.Get();
	}

	funcHead.arguments = ParseArguments(info);
	return funcHead;
}
