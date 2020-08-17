#include "FunctionParser.h"

#include "TypeParser.h"
#include "VariableAttributeParser.h"
#include "StatementParser.h"

#include "Melon/Nodes/FunctionNode.h"
#include "Melon/Nodes/EmptyNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr FunctionParser::Parse(ParsingInfo& info, const bool isPlain) {
	const UInt startIndex = info.index;
	const UInt startLine = info.Current().line;

	if (Optional<FunctionHead> fh = ParseFunctionHead(info, isPlain)) {
		const FunctionHead funcHead = fh;
		Symbol s = Symbol(funcHead.isMethod && !funcHead.isOperator ? SymbolType::Method : SymbolType::Function);
		s.symbolFile = info.currentFile;
		s.symbolNamespace = info.currentNamespace;
		s.includedNamespaces = info.includedNamespaces;
		if (isPlain) s.statementNumber = info.statementNumber;
		s.ret = funcHead.retrunTypes;
		s.size = info.root.funcId++;
		s.varType = ScopeList().Add(funcHead.name);
		s.attributes = funcHead.attributes;

		UInt line = 0;

		Pointer<FunctionNode> func = new FunctionNode(info.scopes, FileInfo(info.filename, startLine, info.statementNumber));

		if (info.Current().type == TokenType::ParenOpen) {
			info.index++;

			Symbol currentScope = Symbol::Find(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));
			info.scopes = info.scopes.Add(funcHead.name);

			if (!currentScope.Contains(funcHead.name)) {
				Symbol::Add(info.scopes, Symbol(SymbolType::Scope), FileInfo(info.filename, info.Current().line, info.statementNumber));
			}

			Symbol fs = Symbol::Find(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));

			if (!funcHead.isOperator) {
				if (!fs.Contains(Scope::Call)) {
					Scope scope = Scope::Call;
					scope.variant = (UInt)0;
					info.scopes = info.scopes.Add(scope);
				}
				else {
					Scope scope = Scope::Call;
					scope.variant = fs.Get(Scope::Call, FileInfo(info.filename, info.Current().line, info.statementNumber)).variants.Size();
					info.scopes = info.scopes.Add(scope);
				}
			}
			else {
				Scope scope = info.scopes.Last();
				scope.variant = fs.variants.Size();
				info.scopes = info.scopes.Pop().Add(scope);
			}

			List<Symbol> args;
			List<String> argNames;

			while (info.Current().type != TokenType::ParenClose) {
				if (!args.IsEmpty()) {
					if (info.Current().type != TokenType::Comma) {
						ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("')'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
					}

					info.index++;
				}

				Optional<ScopeList> type = TypeParser::Parse(info);

				if (type && info.Current().type == TokenType::Colon) {
					info.index++;
					Set<SymbolAttribute> attributes = VariableAttributeParser::Parse(info);

					if (info.Current().type != TokenType::Name) {
						ErrorLog::Error(SyntaxError(SyntaxError::ArgNameExpected, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
					}

					s.args.Add(type);
					s.names.Add(Scope(info.Current().value));
					argNames.Add(info.Current().value);
					func->argNames.Add(Scope(info.Current().value));

					Symbol a = Symbol(SymbolType::Variable);
					a.symbolFile = info.currentFile;
					a.symbolNamespace = info.currentNamespace;
					a.includedNamespaces = info.includedNamespaces;
					a.statementNumber = info.statementNumber;
					a.varType = type;
					a.attributes = attributes;
					args.Add(a);

					info.index++;
				}
				else {
					ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("')'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				}
			}

			line = info.Current().line;
			info.index++;

			for (UInt i = 0; i < args.Size(); i++) {
				s.Add(func->argNames[i], args[i], FileInfo(info.filename, info.Current().line, info.statementNumber));
			}
		}

		info.statementNumber++;

		if (!funcHead.isOperator) {
			info.scopes = info.scopes.Pop();

			Symbol fs = Symbol::Find(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));
			s.scope = info.scopes.Add(Scope::Call);
			Symbol::Add(info.scopes.Add(Scope::Call), s, FileInfo(info.filename, info.Current().line, info.statementNumber));

			Scope scope = Scope::Call;
			scope.variant = fs.Get(Scope::Call, FileInfo(info.filename, info.Current().line, info.statementNumber)).variants.Size() - 1;
			info.scopes = info.scopes.Add(scope);
		}
		else {
			s.scope = info.scopes;
			Symbol::Add(info.scopes, s, FileInfo(info.filename, info.Current().line, info.statementNumber));
		}

		func->s = s;
		func->func = info.scopes;

		UInt loops = info.loops;
		UInt scopeCount = info.scopeCount;
		info.loops = 0;
		info.scopeCount = 0;
		func->node = StatementParser::ParseMultiple(info);
		info.loops = loops;
		info.scopeCount = scopeCount;

		if (info.Current().type != TokenType::End) {
			ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("function", line), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}

		info.index++;

		if (!funcHead.isOperator) {
			info.scopes = info.scopes.Pop().Pop();
		}
		else {
			info.scopes = info.scopes.Pop();
		}

		info.root.funcs.Add(func);

		info.statementNumber++;

		Pointer<EmptyNode> en = new EmptyNode();
		en->node = func;
		return en;
	}

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

Optional<FunctionParser::FunctionHead> FunctionParser::ParseFunctionHead(ParsingInfo& info, const bool isPlain) {
	static Regex upper = Regex("^%u");
	static Regex underscore = Regex("%a_+%a");

	const UInt startIndex = info.index;
	const UInt startLine = info.Current().line;

	FunctionHead funcHead;
	funcHead.isMethod = !isPlain;

	bool loop = !isPlain;

	while (loop) {
		switch (info.Current().type) {
			case TokenType::Static: {
				if (funcHead.attributes.Contains(SymbolAttribute::Static)) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("static"), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}

				funcHead.isMethod = false;
				funcHead.attributes.Add(SymbolAttribute::Static);
				break;
			}
			case TokenType::Override: {
				if (funcHead.attributes.Contains(SymbolAttribute::Override)) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("override"), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}

				funcHead.attributes.Add(SymbolAttribute::Override);
				break;
			}
			case TokenType::Required: {
				if (funcHead.attributes.Contains(SymbolAttribute::Required)) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("required"), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}

				funcHead.attributes.Add(SymbolAttribute::Required);
				break;
			}
			case TokenType::Debug: {
				if (funcHead.attributes.Contains(SymbolAttribute::Debug)) {
					ErrorLog::Error(SyntaxError(SyntaxError::MultipleAttribute("debug"), FileInfo(info.filename, info.Current().line, info.statementNumber)));
				}

				funcHead.attributes.Add(SymbolAttribute::Debug);
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

	if (info.Current().type == TokenType::Function) {
		if (info.Next().type == TokenType::Operator && !isPlain) {
			funcHead.isOperator = true;
			info.index++;
		}
		else {
			funcHead.isOperator = false;
		}

		const UInt retIndex = info.index; 

		while (true) {
			if (!funcHead.retrunTypes.IsEmpty()) {
				if (info.Current().type == TokenType::Comma) {
					info.index++;
				}
				else if (info.Current().type == TokenType::Colon) {
					info.index++;
					break;
				}
				else {
					info.index = retIndex;
					break;
				}
			}

			if (Optional<ScopeList> type = TypeParser::Parse(info)) {
				funcHead.retrunTypes.Add(type);
			}
			else {
				break;
			}
		}

		if (!funcHead.isOperator) {
			if (Optional<Scope> fname = ParseFunctionName(info, isPlain)) {
				const Scope name = fname;

				if (!upper.Match(name.name).IsEmpty()) {
					ErrorLog::Info(InfoError(InfoError::LowerName("function", name.name), FileInfo(info.filename, startLine, info.statementNumber)));
				}

				if (!underscore.Match(name.name).IsEmpty()) {
					ErrorLog::Info(InfoError(InfoError::LowerUnderscoreName("function", name.name), FileInfo(info.filename, startLine, info.statementNumber)));
				}

				funcHead.name = name;
				return funcHead;
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("function name", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				return nullptr;
			}
		}
		else {
			if (Optional<Scope> name = ParseOperatorName(info)) {
				funcHead.name = name;
				return funcHead;
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("operator", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				return nullptr;
			}
		}
	}
	else if (!funcHead.attributes.IsEmpty()) {
		ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("'function'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		return nullptr;
	}

	info.index = startIndex;
	return nullptr;
}