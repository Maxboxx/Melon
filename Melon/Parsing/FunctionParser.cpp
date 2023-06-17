#include "FunctionParser.h"

#include "TypeParser.h"
#include "VariableAttributeParser.h"
#include "ScopeParser.h"
#include "TemplateParser.h"

#include "Melon/Symbols/MapSymbol.h";
#include "Melon/Symbols/TemplateSymbol.h";
#include "Melon/Symbols/StructSymbol.h";
#include "Melon/Symbols/EnumSymbol.h";

#include "Melon/Nodes/FunctionBody.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

Ptr<EmptyStatement> FunctionParser::Parse(ParsingInfo& info, TypeSymbol* const parent) {
	const UInt startIndex = info.index;
	const UInt startLine = info.Current().line;

	// TODO: More accurate lines
	MapSymbol* const temp = info.scope;
	FunctionSymbol* funcSym = new FunctionSymbol(info.GetFileInfo(startLine));
	info.scope = funcSym;

	if (Optional<FunctionHead> fh = ParseFunctionHead(info, parent == nullptr)) {
		const FunctionHead funcHead = *fh;
		Ptr<FunctionBody> func = new FunctionBody(info.scope, info.GetFileInfo(startLine));

		UInt line = 0;

		MapSymbol* const parentSym = parent ? parent->Cast<MapSymbol>() : temp;
		FunctionSymbol* functionParent = nullptr;

		if (Symbol* const func = parentSym->Contains(funcHead.name)) {
			functionParent = func->Cast<FunctionSymbol>();
		}
		else {
			functionParent = parentSym->AddSymbol(funcHead.name, new FunctionSymbol(info.GetFileInfo(startLine)))->Cast<FunctionSymbol>();
		}

		funcSym = functionParent->AddOverload(funcSym);
		funcSym->returnValues = funcHead.returnTypes;
		funcSym->modifiers    = funcHead.modifiers;
		funcSym->modifiers   = funcHead.modifiers;

		for (const NameList& arg : funcHead.templateArgs) {
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
				var->modifiers = VariableModifiers::Ref;
			}
			else if (parent->Is<EnumSymbol>()) {
				var->modifiers = VariableModifiers::Ref;
			}

			funcSym->AddSymbol(Name::Self, var);
			funcSym->arguments.Add(NameList(Name::Self));
		}

		for (const Argument& arg : funcHead.arguments) {
			FileInfo file = info.GetFileInfo(startLine);
			file.statement = 0;
			VariableSymbol* const var = new VariableSymbol(file);
			var->type = arg.type;
			var->modifiers = arg.modifiers;
			funcSym->AddSymbol(arg.name, var);

			funcSym->arguments.Add(NameList(arg.name));
		}

		UInt loops = info.loops;
		UInt scopeCount = info.scopeCount;
		info.loops = 0;
		info.scopeCount = 0;
		
		func->statements = ScopeParser::Parse(info, TokenType::None, ScopeParser::Info("function", startLine));

		func->sym = funcSym;
		funcSym->node = func;

		info.loops = loops;
		info.scopeCount = scopeCount;

		info.scope = temp;
		info.root.funcs.Add(func);
		info.statementNumber++;

		Ptr<EmptyStatement> empty = new EmptyStatement();
		empty->statement = info.root.funcs.Last();
		return empty;
	}

	delete funcSym;
	info.scope = temp;
	info.index = startIndex;
	return nullptr;
}

Optional<Name> FunctionParser::ParseFunctionName(ParsingInfo& info, const bool isPlain) {
	if (info.Current().type == TokenType::Name) {
		const Name s = Name(info.Current().value);
		info.index++;
		return s;
	}
	else if (!isPlain) {
		if (info.Current().type == TokenType::Init) {
			const Name s = Name::Init;
			info.index++;
			return s;
		}
	}

	return nullptr;
}

Optional<Name> FunctionParser::ParseOperatorName(ParsingInfo& info) {
	switch (info.Current().type) {
		// Math
		case TokenType::Plus: {
			info.index++;
			return Name::Add;
		}
		case TokenType::Minus: {
			info.index++;
			return Name::Sub;
		}
		case TokenType::Mul: {
			info.index++;
			return Name::Mul;
		}
		case TokenType::Div: {
			info.index++;
			return Name::Div;
		}
		case TokenType::IDiv: {
			info.index++;
			return Name::IDiv;
		}
		case TokenType::Mod: {
			info.index++;
			return Name::Mod;
		}

		// Bitwise
		case TokenType::BNot: {
			info.index++;
			return Name::BitNot;
		}
		case TokenType::BOr: {
			info.index++;
			return Name::BitOr;
		}
		case TokenType::BAnd: {
			info.index++;
			return Name::BitAnd;
		}
		case TokenType::BXor: {
			info.index++;
			return Name::BitXor;
		}
		case TokenType::BNor: {
			info.index++;
			return Name::BitNor;
		}
		case TokenType::BNand: {
			info.index++;
			return Name::BitNand;
		}
		case TokenType::BXnor: {
			info.index++;
			return Name::BitXnor;
		}
		case TokenType::BShiftLeft: {
			info.index++;
			return Name::ShiftLeft;
		}
		case TokenType::BShiftRight: {
			info.index++;
			return Name::ShiftRight;
		}

		// Compare
		case TokenType::Equal: {
			info.index++;
			return Name::Equal;
		}
		case TokenType::NotEqual: {
			info.index++;
			return Name::NotEqual;
		}
		case TokenType::Less: {
			info.index++;
			return Name::Less;
		}
		case TokenType::LessEq: {
			info.index++;
			return Name::LessEqual;
		}
		case TokenType::Greater: {
			info.index++;
			return Name::Greater;
		}
		case TokenType::GreaterEq: {
			info.index++;
			return Name::GreaterEqual;
		}

		// Misc
		case TokenType::Hash: {
			info.index++;
			return Name::Len;
		}
		case TokenType::SquareOpen: {
			if (info.Prev().type == TokenType::SquareClose) {
				info.index += 2;
				return Name::Index;
			}

			return nullptr;
		}
	}

	return nullptr;
}

Optional<Name> FunctionParser::ParseName(const bool isOperator, ParsingInfo& info, const bool isPlain) {
	static Regex upper = Regex("^%u");
	static Regex underscore = Regex("%a_+%a");

	if (!isOperator) {
		if (Optional<Name> fname = ParseFunctionName(info, isPlain)) {
			const Name name = *fname;

			if (upper.Match(name.name)) {
				ErrorLog::Info(LogMessage("info.name.lower", "function", name.name), info.GetFileInfoPrev());
			}

			if (underscore.Match(name.name)) {
				ErrorLog::Info(LogMessage("info.name.under", "function", name.name), info.GetFileInfoPrev());
			}

			return name;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "function name", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
			return nullptr;
		}
	}
	else {
		if (Optional<Name> name = ParseOperatorName(info)) {
			return *name;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "operator", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
			return nullptr;
		}
	}
}

FunctionModifiers FunctionParser::ParseModifiers(ParsingInfo& info, const bool isPlain) {
	FunctionModifiers modifiers = FunctionModifiers::None;

	bool loop = !isPlain;

	while (loop) {
		// TODO: Error for invalid modifiers
		switch (info.Current().type) {
			case TokenType::Static: {
				if ((modifiers & FunctionModifiers::Static) != FunctionModifiers::None) {
					ErrorLog::Error(LogMessage("error.syntax.attribute.multiple", "static"), info.GetFileInfo());
				}
				
				modifiers |= FunctionModifiers::Static;
				break;
			}
			case TokenType::Override: {
				if ((modifiers & FunctionModifiers::Override) != FunctionModifiers::None) {
					ErrorLog::Error(LogMessage("error.syntax.attribute.multiple", "override"), info.GetFileInfo());
				}

				modifiers |= FunctionModifiers::Override;
				break;
			}
			case TokenType::Required: {
				if ((modifiers & FunctionModifiers::Required) != FunctionModifiers::None) {
					ErrorLog::Error(LogMessage("error.syntax.attribute.multiple", "required"), info.GetFileInfo());
				}

				modifiers |= FunctionModifiers::Required;
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

	return modifiers;
}

FunctionAttributes FunctionParser::ParseAttributes(ParsingInfo& info, const bool isPlain) {
	FunctionAttributes modifiers = FunctionAttributes::None;

	bool loop = true;

	while (loop) {
		// TODO: Error for invalid attributes
		switch (info.Current().type) {
			case TokenType::Throw: {
				if ((modifiers & FunctionAttributes::Throw) != FunctionAttributes::None) {
					ErrorLog::Error(LogMessage("error.syntax.attribute.multiple", "throw"), info.GetFileInfo());
				}

				modifiers |= FunctionAttributes::Throw;
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

	return modifiers;
}

List<NameList> FunctionParser::ParseReturnTypes(ParsingInfo& info) {
	const UInt retIndex = info.index;
	List<NameList> returnTypes;

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
				if (returnTypes.Count() > 1) {
					ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(":"), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
				}

				info.index = retIndex;
				returnTypes = List<NameList>();

				break;
			}
		}

		if (Optional<NameList> type = TypeParser::Parse(info)) {
			returnTypes.Add(*type);
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
		ErrorLog::Error(LogMessage("error.syntax.expected.after", "'('", LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
		return arguments;
	}

	info.index++;

	while (info.Current().type != TokenType::ParenClose) {
		if (!arguments.IsEmpty()) {
			if (info.Current().type != TokenType::Comma) {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(")"), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
			}

			info.index++;
		}

		Optional<NameList> type = TypeParser::Parse(info);

		if (type && info.Current().type == TokenType::Colon) {
			info.index++;

			Argument argument;
			argument.type       = *type;
			argument.modifiers = VariableAttributeParser::Parse(info, true);

			if (info.Current().type != TokenType::Name) {
				ErrorLog::Error(LogMessage("error.syntax.expected.name.argument"), info.GetFileInfoPrev());
			}

			argument.name = Name(info.Current().value);
			arguments.Add(argument);

			info.index++;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote(")"), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
		}
	}

	info.index++;
	return arguments;
}

Optional<FunctionParser::FunctionHead> FunctionParser::ParseFunctionHead(ParsingInfo& info, const bool isPlain) {
	const UInt startIndex = info.index;
	const UInt startLine = info.Current().line;

	FunctionHead funcHead;
	funcHead.modifiers = ParseModifiers(info, isPlain);
	funcHead.isMethod  = !isPlain && (funcHead.modifiers & FunctionModifiers::Static) == FunctionModifiers::None;

	if (info.Current().type != TokenType::Fun) {
		if (funcHead.modifiers != FunctionModifiers::None) {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", LogMessage::Quote("function"), LogMessage::Quote(info.Prev().value)), info.GetFileInfoPrev());
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
	funcHead.attributes  = ParseAttributes(info, isPlain);
	
	if (Optional<Name> name = ParseName(funcHead.isOperator, info, isPlain)) {
		funcHead.name = *name;
	}

	if (Optional<List<NameList>> templateArgs = TemplateParser::ParseDefine(info)) {
		funcHead.templateArgs = *templateArgs;
	}

	funcHead.arguments = ParseArguments(info);
	return funcHead;
}
