#include "Parser.h"

#include "Boxx/Regex.h"

#include "StatementParser.h"
#include "IncludeParser.h"

#include "Melon/MelonCompiler.h"
#include "Melon/Lexer.h"
#include "Melon/Errors.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

List<Melon::TokenPattern> Parser::patterns;

Melon::Token ParsingInfo::Next() {
	if (index + 1 >= tokens.Size())
		ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedEOF, FileInfo(filename, tokens.Last().line, 0)));

	return tokens[++index];
}

Melon::Token ParsingInfo::Previous() {
	if (index - 1 >= tokens.Size())
		ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedEOF, FileInfo(filename, tokens.Last().line, 0)));

	return tokens[--index];
}

bool ParsingInfo::EndOfFile() const {
	return index >= tokens.Size();
}

Melon::Token ParsingInfo::Current() {
	if (index >= tokens.Size())
		ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedEOF, FileInfo(filename, tokens.Last().line, 0)));

	return tokens[index];
}

Melon::Token ParsingInfo::Current(const Int offset) {
	if (index + offset >= tokens.Size())
		ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedEOF, FileInfo(filename, tokens.Last().line, 0)));

	return tokens[index + offset];
}

ParsingInfo Parser::Parse(const String& filename, const CompilerOptions& options) {
	SetupTokens();

	ParsingInfo info;
	info.options = options;
	info.filename = filename;
	info.index = 0;
	info.loops = 0;
	info.scopeCount = 0;
	info.statementNumber = 1;

	ParseFile(filename, info);

	return info;
}

NodePtr Parser::UnexpectedToken(ParsingInfo& info) {
	ErrorLog::Error(SyntaxError(SyntaxError::UnexpectedTokenStart + info.Current().value + SyntaxError::UnexpectedTokenEnd, FileInfo(info.filename, info.Current().line, info.statementNumber)));
	return nullptr;
}

void Parser::ParseFile(const String& filename, ParsingInfo& info) {
	try {
		FileReader file = FileReader(filename);
		info.tokens = Lexer::Lex(GetTokenPatterns(), file.ReadAll(), filename);
		file.Close();
	}
	catch (FileError e) {
		ErrorLog::Error(PlainError(e.Message()));
	}

	while (!info.EndOfFile()) {
		if (!IncludeParser::Parse(info)) {
			break;
		}
	}

	while (!info.EndOfFile()) {
		if (!ParseNext(info)) {
			break;
		}
	}
}

NodePtr Parser::ParseNext(ParsingInfo& info) {
	if (NodePtr node = StatementParser::Parse(info)) {
		info.root.nodes.Add(node);
		return node;
	}

	return UnexpectedToken(info);
}

void Parser::SetupTokens() {
	patterns = List<TokenPattern>();

	// Comments
	patterns.Add(TokenPattern(TokenType::CommentBlock, "%-%-#{%/+}~{%0%-}*%0%-%-", true, true));
	patterns.Add(TokenPattern(TokenType::CommentLine, "%-%-~\n*", true, true));

	// Raw
	patterns.Add(TokenPattern(TokenType::Float, "%d*%.%d+"));
	patterns.Add(TokenPattern(TokenType::Integer, "%d+"));
	patterns.Add(TokenPattern(TokenType::Boolean, "true|false"));

	// Bitwise
	patterns.Add(TokenPattern(TokenType::BNand, "%~%&", true));
	patterns.Add(TokenPattern(TokenType::BNor, "%~%|", true));
	patterns.Add(TokenPattern(TokenType::BXnor, "%~%^", true));
	patterns.Add(TokenPattern(TokenType::BAnd, "%&", true));
	patterns.Add(TokenPattern(TokenType::BOr, "%|", true));
	patterns.Add(TokenPattern(TokenType::BXor, "%^", true));
	patterns.Add(TokenPattern(TokenType::BShiftLeft, "%<%<", true));
	patterns.Add(TokenPattern(TokenType::BShiftRight, "%>%>", true));

	// Compare
	patterns.Add(TokenPattern(TokenType::Equal, "%=%=", true));
	patterns.Add(TokenPattern(TokenType::NotEqual, "%~%=", true));
	patterns.Add(TokenPattern(TokenType::LessEq, "%<%=", true));
	patterns.Add(TokenPattern(TokenType::GreaterEq, "%>%=", true));
	patterns.Add(TokenPattern(TokenType::Less, "%<", true));
	patterns.Add(TokenPattern(TokenType::Greater, "%>", true));

	patterns.Add(TokenPattern(TokenType::BNot, "%~", true));

	// Math
	patterns.Add(TokenPattern(TokenType::Plus, "%+", true));
	patterns.Add(TokenPattern(TokenType::Minus, "%-", true));
	patterns.Add(TokenPattern(TokenType::Mul, "%*", true));
	patterns.Add(TokenPattern(TokenType::IDiv, "%/%/", true));
	patterns.Add(TokenPattern(TokenType::Div, "%/", true));
	patterns.Add(TokenPattern(TokenType::Mod, "%%", true));
	//patterns.Add(TokenPattern(TokenType::Pow, "%^", true));

	// Assign
	patterns.Add(TokenPattern(TokenType::Assign, "%=", true));

	// Delimiters
	patterns.Add(TokenPattern(TokenType::ParenOpen, "%(", true));
	patterns.Add(TokenPattern(TokenType::ParenClose, "%)", true));
	patterns.Add(TokenPattern(TokenType::CurlyOpen, "%{", true));
	patterns.Add(TokenPattern(TokenType::CurlyClose, "%}", true));
	patterns.Add(TokenPattern(TokenType::SquareOpen, "%[", true));
	patterns.Add(TokenPattern(TokenType::SquareClose, "%]", true));

	// Operators
	patterns.Add(TokenPattern(TokenType::Comma, "%,", true));
	patterns.Add(TokenPattern(TokenType::Colon, "%:", true));
	patterns.Add(TokenPattern(TokenType::Dot, "%.", true));
	patterns.Add(TokenPattern(TokenType::Len, "%#", true));
	patterns.Add(TokenPattern(TokenType::Exclamation, "%!", true));
	patterns.Add(TokenPattern(TokenType::Question, "%?", true));

	// Keywords
	patterns.Add(TokenPattern(TokenType::Include, "include"));
	patterns.Add(TokenPattern(TokenType::Global, "global"));
	patterns.Add(TokenPattern(TokenType::If, "if"));
	patterns.Add(TokenPattern(TokenType::Then, "then"));
	patterns.Add(TokenPattern(TokenType::AlsoIf, "alsoif"));
	patterns.Add(TokenPattern(TokenType::ElseIf, "elseif"));
	patterns.Add(TokenPattern(TokenType::While, "while"));
	patterns.Add(TokenPattern(TokenType::AlsoWhile, "alsowhile"));
	patterns.Add(TokenPattern(TokenType::ElseWhile, "elsewhile"));
	patterns.Add(TokenPattern(TokenType::For, "for"));
	patterns.Add(TokenPattern(TokenType::AlsoFor, "alsofor"));
	patterns.Add(TokenPattern(TokenType::ElseFor, "elsefor"));
	patterns.Add(TokenPattern(TokenType::Do, "do"));
	patterns.Add(TokenPattern(TokenType::Also, "also"));
	patterns.Add(TokenPattern(TokenType::Else, "else"));
	patterns.Add(TokenPattern(TokenType::End, "end"));
	patterns.Add(TokenPattern(TokenType::Switch, "switch"));
	patterns.Add(TokenPattern(TokenType::Case, "case"));
	patterns.Add(TokenPattern(TokenType::Default, "default"));
	patterns.Add(TokenPattern(TokenType::Guard, "guard"));
	patterns.Add(TokenPattern(TokenType::Function, "function"));
	patterns.Add(TokenPattern(TokenType::Return, "return"));
	patterns.Add(TokenPattern(TokenType::Break, "break"));
	patterns.Add(TokenPattern(TokenType::Abort, "abort"));
	patterns.Add(TokenPattern(TokenType::Continue, "continue"));
	patterns.Add(TokenPattern(TokenType::Repeat, "repeat"));
	patterns.Add(TokenPattern(TokenType::Until, "until"));
	patterns.Add(TokenPattern(TokenType::Struct, "struct"));
	patterns.Add(TokenPattern(TokenType::Enum, "enum"));
	patterns.Add(TokenPattern(TokenType::Init, "init"));

	patterns.Add(TokenPattern(TokenType::Const, "const"));
	patterns.Add(TokenPattern(TokenType::Ref, "ref"));
	patterns.Add(TokenPattern(TokenType::Copy, "copy"));

	patterns.Add(TokenPattern(TokenType::Nil, "nil"));

	patterns.Add(TokenPattern(TokenType::Type, "bool"));
	patterns.Add(TokenPattern(TokenType::Type, "char"));
	patterns.Add(TokenPattern(TokenType::Type, "byte"));
	patterns.Add(TokenPattern(TokenType::Type, "ubyte"));
	patterns.Add(TokenPattern(TokenType::Type, "short"));
	patterns.Add(TokenPattern(TokenType::Type, "ushort"));
	patterns.Add(TokenPattern(TokenType::Type, "int"));
	patterns.Add(TokenPattern(TokenType::Type, "uint"));
	patterns.Add(TokenPattern(TokenType::Type, "long"));
	patterns.Add(TokenPattern(TokenType::Type, "ulong"));
	patterns.Add(TokenPattern(TokenType::Type, "float"));
	patterns.Add(TokenPattern(TokenType::Type, "double"));
	patterns.Add(TokenPattern(TokenType::Type, "string"));

	patterns.Add(TokenPattern(TokenType::Static, "static"));
	patterns.Add(TokenPattern(TokenType::Override, "override"));
	patterns.Add(TokenPattern(TokenType::Operator, "operator"));
	patterns.Add(TokenPattern(TokenType::Required, "required"));
	patterns.Add(TokenPattern(TokenType::Debug, "debug"));

	// Logic
	patterns.Add(TokenPattern(TokenType::Not, "not"));
	patterns.Add(TokenPattern(TokenType::Or, "or"));
	patterns.Add(TokenPattern(TokenType::And, "and"));
	patterns.Add(TokenPattern(TokenType::Xor, "xor"));
	patterns.Add(TokenPattern(TokenType::Nor, "nor"));
	patterns.Add(TokenPattern(TokenType::Nand, "nand"));
	patterns.Add(TokenPattern(TokenType::Xnor, "xnor"));

	// Name
	patterns.Add(TokenPattern(TokenType::Name, "%w+"));
}

List<Melon::TokenPattern> Parser::GetTokenPatterns() {
	return patterns;
}