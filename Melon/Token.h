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
		Character,
		Boolean,
		Name,
		Nil,
		Type,
		StringLiteral,

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
		Hash,
		Exclamation,
		Question,
		DoubleQuestion,
		Arrow,

		// Keywords
		If,
		Alif,
		Elif,
		While,
		Alwhile,
		Elwhile,
		For,
		Alfor,
		Elfor,
		Do,
		Also,
		Else,
		End,
		Match,
		Is,
		Guard,
		Fun,
		Ret,
		Break,
		Abort,
		Continue,
		Repeat,
		Until,
		Struct,
		Class,
		Enum,
		Const,
		Ref,
		NoRef,
		Copy,
		Init,
		Operator,
		Static,
		Required,
		Override,
		Debug,
		Include,
		Import,
		Global,
		As,
		Discard,
		Throw,
		Any,
		Ext,
		Abst,
		New,

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

		Token() {
			type = TokenType::None;
			value = "";
			line = 0;
		}

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
		bool ignore{};

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
