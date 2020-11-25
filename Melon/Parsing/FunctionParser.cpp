#include "FunctionParser.h"

#include "TypeParser.h"
#include "VariableAttributeParser.h"
#include "StatementParser.h"
#include "TemplateParser.h"

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
		const FunctionHead funcHead = (FunctionHead)fh;
		const Optional<List<Scope>> templateArgs = TemplateParser::ParseDefine(info);

		Symbols s = Symbols(funcHead.isMethod && !funcHead.isOperator ? SymbolType::Method : SymbolType::Function);
		s.symbolFile = info.currentFile;
		s.symbolNamespace = info.currentNamespace;
		s.includedNamespaces = info.includedNamespaces;
		if (isPlain) s.statementNumber = info.statementNumber;
		s.returnValues = funcHead.retrunTypes;
		s.size = info.root.funcId++;
		s.varType = ScopeList().Add(funcHead.name);
		s.attributes = funcHead.attributes;

		UInt line = 0;

		Pointer<FunctionNode> func = new FunctionNode(info.scopes, FileInfo(info.filename, startLine, info.statementNumber, info.currentNamespace, info.includedNamespaces));

		if (info.Current().type == TokenType::ParenOpen) {
			info.index++;

			Symbols currentScope = Symbols::Find(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));

			info.scopes = info.scopes.Add(funcHead.name);

			if (!currentScope.Contains(funcHead.name)) {
				Symbols scope = Symbols(SymbolType::Scope);
				scope.symbolNamespace = info.currentNamespace;
				scope.includedNamespaces = info.includedNamespaces;
				Symbols::Add(info.scopes, scope, FileInfo(info.filename, info.Current().line, info.statementNumber));
			}

			Symbols fs = Symbols::Find(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));

			if (templateArgs) {
				Scope name = funcHead.name;
				name.types = List<ScopeList>();
				name.variant = fs.templateVariants.Size();
				info.scopes = info.scopes.Pop().Add(name);

				Symbols scope = Symbols(SymbolType::Scope);
				scope.symbolNamespace = info.currentNamespace;
				scope.includedNamespaces = info.includedNamespaces;
				scope.scope = info.scopes;
				scope.varType = info.scopes;

				for (const Scope& arg : templateArgs.Get()) {
					scope.templateArgs.Add(scope.scope.Add(arg));
				}

				Symbols::Add(info.scopes, scope, FileInfo(info.filename, info.Current().line, info.statementNumber));

				fs = scope;
			}

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

			List<Symbols> args;
			List<String> argNames;

			s.scope = info.scopes;

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
					Set<SymbolAttribute> attributes = VariableAttributeParser::Parse(info, true);

					if (info.Current().type != TokenType::Name) {
						ErrorLog::Error(SyntaxError(SyntaxError::ArgNameExpected, FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
					}

					s.arguments.Add((ScopeList)type);
					s.names.Add(Scope(info.Current().value));
					argNames.Add(info.Current().value);
					func->argNames.Add(Scope(info.Current().value));

					Symbols a = Symbols(SymbolType::Variable);
					a.symbolFile = info.currentFile;
					a.symbolNamespace = info.currentNamespace;
					a.includedNamespaces = info.includedNamespaces;
					a.statementNumber = info.statementNumber;
					a.varType = (ScopeList)type;
					a.attributes = attributes;
					a.scope = info.scopes.Add(Scope(info.Current().value));
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
		s.node = func;

		if (!funcHead.isOperator) {
			info.scopes = info.scopes.Pop();

			Symbols fs = Symbols::Find(info.scopes, FileInfo(info.filename, info.Current().line, info.statementNumber));
			fs.symbolNamespace = info.currentNamespace;
			fs.includedNamespaces = info.includedNamespaces;
			s.scope = info.scopes.Add(Scope::Call);
			s.varType = s.scope;
			Symbols::Add(info.scopes.Add(Scope::Call), s, FileInfo(info.filename, info.Current().line, info.statementNumber));

			Scope scope = Scope::Call;
			scope.variant = fs.Get(Scope::Call, FileInfo(info.filename, info.Current().line, info.statementNumber)).variants.Size() - 1;
			info.scopes = info.scopes.Add(scope);
		}
		else {
			s.scope = info.scopes;
			s.varType = s.scope;
			Symbols::Add(info.scopes, s, FileInfo(info.filename, info.Current().line, info.statementNumber));
		}

		if (templateArgs) {
			Symbols fs = Symbols::Find(info.scopes.Pop(), FileInfo(info.filename, startLine, info.statementNumber));

			for (UInt i = 0; i < templateArgs.Get().Size(); i++) {
				Symbols t = Symbols(SymbolType::Template);
				t.templateIndex = i;
				t.scope = fs.scope.Add(templateArgs.Get()[i]);
				t.varType = t.scope;
				fs.Add(templateArgs.Get()[i], t, FileInfo(info.filename, startLine, info.statementNumber));
			}
		}

		func->s = s;
		func->s.scope = info.scopes;
		func->func = info.scopes;

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

		info.loops = loops;
		info.scopeCount = scopeCount;

		if (!single && info.Current().type != TokenType::End) {
			ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("function", line), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}

		if (!single) info.index++;

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
					if (funcHead.retrunTypes.Size() > 1) {
						ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("':'", "'" + info.Current(-1).value + "'"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
					}

					info.index = retIndex;
					funcHead.retrunTypes = List<ScopeList>();

					break;
				}
			}

			if (Optional<ScopeList> type = TypeParser::Parse(info)) {
				funcHead.retrunTypes.Add((ScopeList)type);
			}
			else {
				break;
			}
		}

		if (!funcHead.isOperator) {
			if (Optional<Scope> fname = ParseFunctionName(info, isPlain)) {
				const Scope name = (Scope)fname;

				if (upper.Match(name.name)) {
					ErrorLog::Info(InfoError(InfoError::LowerName("function", name.name), FileInfo(info.filename, startLine, info.statementNumber)));
				}

				if (underscore.Match(name.name)) {
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
				funcHead.name = (Scope)name;
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