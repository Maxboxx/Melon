#pragma once

#include "Boxx/String.h"
#include "Boxx/Regex.h"

namespace Melon {
	///B TokenType
	/// Enum containing all token types
	enum class TokenType : Boxx::UByte {
		None,

		// Raw
		Integer,
		Float,
		Boolean,
		Name,
		Nil,
		Type,

		// Math
		Plus,
		Minus,
		Mul,
		Div,
		IDiv,
		Mod,
		Pow,
		Concat,

		// Bitwise
		BNot,
		BAnd,
		BOr,
		BXor,
		BNand,
		BNor,
		BXnor,
		BShiftLeft,
		BShiftRight,

		// Compare
		Equal,
		NotEqual,
		LessEq,
		GreaterEq,
		Less,
		Greater,

		// Assign
		Assign,

		// Delimiters
		ParenOpen,
		ParenClose,
		CurlyOpen,
		CurlyClose,
		SquareOpen,
		SquareClose,

		// Operators
		Comma,
		Colon,
		Dot,
		Len,
		Exclamation,
		Question,
		DoubleQuestion,

		// Keywords
		If,
		Then,
		AlsoIf,
		ElseIf,
		While,
		AlsoWhile,
		ElseWhile,
		For,
		AlsoFor,
		ElseFor,
		Do,
		Also,
		Else,
		End,
		Switch,
		Case,
		Default,
		Guard,
		Function,
		Return,
		Break,
		Abort,
		Continue,
		Repeat,
		Until,
		Struct,
		Enum,
		Const,
		Ref,
		Copy,
		Init,
		Operator,
		Static,
		Required,
		Override,
		Debug,
		Include,
		Global,

		// Logic
		Not,
		Or,
		And,
		Xor,
		Nor,
		Nand,
		Xnor,

		// Comments
		CommentLine,
		CommentBlock
	};

	///B Token
	/// Contains info about a token in the source code
	struct Token {
		///T Type
		TokenType type = TokenType::None;

		///T Value
		/// Contains the matched string for the token
		Boxx::String value;

		///T Line
		/// The line number for the token
		Boxx::UInt line;

		Token() {}

		///T Constructor
		Token(const TokenType type, const Boxx::String& value, const Boxx::UInt line) {
			this->type = type;
			this->value = value;
			this->line = line;
		}
	};

	///B TokenPattern
	/// Contains the pattern for a <type>TokenType</type>
	struct TokenPattern {
		TokenType type = TokenType::None;
		Boxx::Regex pattern;
		bool ignore;

		TokenPattern() {}

		///T Constructor
		///A type: The token type for the pattern
		///A pattern: The regex pattern for the token
		///A ignoreBoundary: Ignores word boundary at the end of the patterns. Used for operators
		///A ignore: Deletes token after lexing it
		TokenPattern(const TokenType type, const Boxx::String& pattern, const bool ignoreBoundary = false, const bool ignore = false) {
			this->type = type;
			this->ignore = ignore;

			if (ignoreBoundary) {
				this->pattern = Boxx::Regex("^{" + pattern + "}");
			}
			else {
				this->pattern = Boxx::Regex("^{" + pattern + "}%b");
			}
		}
	};
}
