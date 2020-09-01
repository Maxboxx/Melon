#pragma once

#include "Types.h"
#include "List.h"
#include "Pair.h"
#include "Map.h"
#include "String.h"
#include "Regex.h"
#include "Token.h"
#include "Lexer.h"
#include "Optional.h"

///N Mango
namespace Boxx {

	///B MangoType
	/// Enum of all types of mango values
	enum class MangoType : UByte {
		///T Values
		///M
		Nil,
		Boolean,
		Number,
		String,
		List,
		Map
		///M
	};

	///B Mango
	/// A mango data structure that can encode and decode mango files
	class Mango {
	public:
		///H Constructors

		///T Nil
		Mango();

		///T MangoType
		/// Creates a mango value of the specified type
		explicit Mango(const MangoType type);

		///T Boolean
		Mango(const bool boolean);

		///T Integer
		///M
		Mango(const Int number);
		Mango(const Long number);
		///M

		///T Floating point numbers
		///M
		Mango(const float number);
		Mango(const double number);
		///M

		///T String
		Mango(const String& string);

		///T Mango list
		Mango(const List<Mango>& list);

		///T Mango map
		Mango(const Map<String, Mango>& map);

		///T Constructors with labels
		///M
		Mango(const String& label, const MangoType type);
		Mango(const String& label, const bool boolean);
		Mango(const String& label, const Int number);
		Mango(const String& label, const Long number);
		Mango(const String& label, const float number);
		Mango(const String& label, const double number);
		Mango(const String& label, const String& string);
		Mango(const String& label, const List<Mango>& list);
		Mango(const String& label, const Map<String, Mango>& map);
		///M

		Mango(const Mango& mango);
		Mango(Mango&& mango) noexcept;
		~Mango();

		///T Type
		/// Get the current type of the mango value
		MangoType Type() const;

		///T Set label
		/// Set the label of a mango value
		/// An empty string will remove the label
		void SetLabel(const String& label);

		///T Get label
		/// Get the label for this mango value
		///R String label: The label. If the mango value has no label an empty string is returned
		String GetLabel() const;

		///T Add to list
		/// Add a mango value to the list
		///E MangoTypeError: Thrown if the current type is not list
		void Add(const Mango& mango);

		///T Add to map
		/// Adds a value to the map with the specified key
		///E MangoTypeError: Thrown if the current type is not map
		void Add(const String& key, const Mango& mango);

		///T Size of list
		/// Returns the size of the list
		///E MangoTypeError: Thrown if the current type is not list
		UInt Size() const;

		///H Operators

		///T Convert
		/// Returns the mango value
		///E MangoTypeError: Thrown if the mango type does not match the conversion type
		///M
		operator bool() const;
		operator Int() const;
		operator Long() const;
		operator float() const;
		operator double() const;
		operator String() const;
		operator List<Mango>() const;
		operator Map<String, Mango>() const;
		///M

		///T List index
		/// Get or set a value in the list
		///E MangoTypeError: Thrown if the current type is not list
		///M
		Mango& operator[](const UInt index);
		const Mango& operator[](const UInt index) const;
		///M

		///T Map index
		/// Get or set a value in the map
		///E MangoTypeError: Thrown if the current type is not map
		///E MangoKeyError: Thrown if the key does not exist
		///M
		Mango& operator[](const String& key);
		const Mango& operator[](const String& key) const;
		///M

		///T Assign value
		/// Set the mango value. Changes the type to the type of the new value
		///M
		void operator=(const bool boolean);
		void operator=(const Int number);
		void operator=(const Long number);
		void operator=(const float number);
		void operator=(const double number);
		void operator=(const String& string);
		void operator=(const List<Mango>& list);
		void operator=(const Map<String, Mango>& map);
		///M

		void operator=(const Mango& mango);
		void operator=(Mango&& mango) noexcept;

		///T Equality
		/// Checks the equality of two mango values
		///M
		bool operator==(const Mango& mango) const;
		bool operator!=(const Mango& mango) const;
		///M

		///H Static functions

		///T Encode
		/// Encodes the mango value to a string
		///A const bool pretty: Whether or not to create a pretty string
		static String Encode(const Mango& mango, const bool pretty = false);

		///T Decode
		/// Decodes a string to a mango value
		///E MangoDecodeError: Thrown if the mango string can not be decoded
		static Mango Decode(const String& mango);

	private:
		static String EncodeNode(const Mango& mango);
		static String EncodeNode(const Mango& mango, const String& tabs);

		MangoType type;
		String label;
		bool boolean = false;
		bool integer = false;
		double number = 0.0;
		String string;
		List<Mango> list;
		Map<String, Mango> map;

		enum class MangoTokenType : UByte {
			None,
			Comment,
			Name,
			Boolean,
			Nil,
			Number,
			String,
			Colon,
			OpenCurl,
			CloseCurl,
			OpenSq,
			CloseSq
		};

		static Mango DecodeTokens(const List<Token<MangoTokenType>>& tokens);
		static Optional<Mango> DecodeValue(const List<Token<MangoTokenType>>& tokens, UInt& index);
		static Optional<String> DecodeLabel(const List<Token<MangoTokenType>>& tokens, UInt& index);
	};

	///B MangoError
	/// Base class for all mango errors
	class MangoError : public Error {
	public:
		MangoError() : Error() {}
		MangoError(const char* const msg) : Error(msg) {}

		virtual String Name() const override {
			return "MangoError";
		}
	};

	///B MangoTypeError
	/// Used for type related mango errors
	class MangoTypeError : public MangoError {
	public:
		MangoTypeError() : MangoError() {}
		MangoTypeError(const char* const msg) : MangoError(msg) {}

		virtual String Name() const override {
			return "MangoTypeError";
		}
	};

	///B MangoKeyError
	/// Used for mango map key related errors
	class MangoKeyError : public MangoError {
	public:
		MangoKeyError() : MangoError() {}
		MangoKeyError(const char* const msg) : MangoError(msg) {}

		virtual String Name() const override {
			return "MangoKeyError";
		}
	};

	///B MangoDecodeError
	/// Used for decoding errors
	class MangoDecodeError : public MangoError {
	public:
		MangoDecodeError() : MangoError() {}
		MangoDecodeError(const char* const msg) : MangoError(msg) {}

		virtual String Name() const override {
			return "MangoDecodeError";
		}
	};

	inline Mango::Mango() {
		type = MangoType::Nil;
	}

	inline Mango::Mango(const MangoType type) {
		this->type = type;
	}

	inline Mango::Mango(const bool boolean) {
		type = MangoType::Boolean;
		this->boolean = boolean;
	}

	inline Mango::Mango(const Int number) {
		type = MangoType::Number;
		integer = true;
		this->number = number;
	}

	inline Mango::Mango(const Long number) {
		type = MangoType::Number;
		integer = true;
		this->number = (double)number;
	}

	inline Mango::Mango(const float number) {
		type = MangoType::Number;
		integer = false;
		this->number = number;
	}

	inline Mango::Mango(const double number) {
		type = MangoType::Number;
		integer = false;
		this->number = number;
	}

	inline Mango::Mango(const String& string) {
		type = MangoType::String;
		this->string = string;
	}

	inline Mango::Mango(const List<Mango>& list) {
		type = MangoType::List;
		this->list = list;
	}

	inline Mango::Mango(const Map<String, Mango>& map) {
		type = MangoType::Map;
		this->map = map;
	}

	inline Mango::Mango(const String& label, const MangoType type) {
		this->type = type;
		this->label = label;
	}

	inline Mango::Mango(const String& label, const bool boolean) {
		type = MangoType::Boolean;
		this->boolean = boolean;
		this->label = label;
	}

	inline Mango::Mango(const String& label, const Int number) {
		type = MangoType::Number;
		integer = true;
		this->number = number;
		this->label = label;
	}

	inline Mango::Mango(const String& label, const Long number) {
		type = MangoType::Number;
		integer = true;
		this->number = (double)number;
		this->label = label;
	}

	inline Mango::Mango(const String& label, const float number) {
		type = MangoType::Number;
		integer = false;
		this->number = number;
		this->label = label;
	}

	inline Mango::Mango(const String& label, const double number) {
		type = MangoType::Number;
		integer = false;
		this->number = number;
		this->label = label;
	}

	inline Mango::Mango(const String& label, const String& string) {
		type = MangoType::String;
		this->string = string;
		this->label = label;
	}

	inline Mango::Mango(const String& label, const List<Mango>& list) {
		type = MangoType::List;
		this->list = list;
		this->label = label;
	}

	inline Mango::Mango(const String& label, const Map<String, Mango>& map) {
		type = MangoType::Map;
		this->map = map;
		this->label = label;
	}

	inline Mango::Mango(const Mango& mango) {
		type = mango.type;
		label = mango.label;

		switch (type) {
			case MangoType::Nil: break;
			case MangoType::Boolean: boolean = mango.boolean; break;
			case MangoType::Number: number = mango.number; integer = mango.integer; break;
			case MangoType::String: string = mango.string; break;
			case MangoType::List: list = mango.list; break;
			case MangoType::Map: map = mango.map; break;
		}
	}

	inline Mango::Mango(Mango&& mango) noexcept {
		type = mango.type;
		label = std::move(mango.label);

		switch (type) {
			case MangoType::Nil: break;
			case MangoType::Boolean: boolean = mango.boolean; break;
			case MangoType::Number: number = mango.number; integer = mango.integer; break;
			case MangoType::String: string = std::move(mango.string); break;
			case MangoType::List: list = std::move(mango.list); break;
			case MangoType::Map: map = std::move(mango.map); break;
		}
	}

	inline Mango::~Mango() {

	}

	inline MangoType Mango::Type() const {
		return type;
	}

	inline void Mango::SetLabel(const String& label) {
		this->label = label;
	}

	inline String Mango::GetLabel() const {
		return label;
	}

	inline void Mango::Add(const Mango& mango) {
		if (type != MangoType::List) throw MangoTypeError("Type is not list");
		list.Add(mango);
	}

	inline void Mango::Add(const String& key, const Mango& mango) {
		if (type != MangoType::Map) throw MangoTypeError("Type is not map");

		try {
			map.Add(key, mango);
		}
		catch (MapKeyError) {
			throw MangoKeyError("Key already exists");
		}
	}

	inline UInt Mango::Size() const {
		if (type != MangoType::List) throw MangoTypeError("Type is not list");
		return list.Size();
	}

	inline Mango::operator bool() const {
		if (type != MangoType::Boolean) throw MangoTypeError("Type is not boolean");
		return boolean;
	}

	inline Mango::operator Int() const {
		if (type != MangoType::Number) throw MangoTypeError("Type is not number");
		return (Int)number;
	}

	inline Mango::operator Long() const {
		if (type != MangoType::Number) throw MangoTypeError("Type is not number");
		return (Long)number;
	}

	inline Mango::operator float() const {
		if (type != MangoType::Number) throw MangoTypeError("Type is not number");
		return (float)number;
	}

	inline Mango::operator double() const {
		if (type != MangoType::Number) throw MangoTypeError("Type is not number");
		return number;
	}

	inline Mango::operator String() const {
		if (type != MangoType::String) throw MangoTypeError("Type is not string");
		return string;
	}

	inline Mango::operator List<Mango>() const {
		if (type != MangoType::List) throw MangoTypeError("Type is not list");
		return list;
	}

	inline Mango::operator Map<String, Mango>() const {
		if (type != MangoType::Map) throw MangoTypeError("Type is not map");
		return map;
	}

	inline Mango& Mango::operator[](const UInt index) {
		if (type != MangoType::List) throw MangoTypeError("Type is not list");
		return list[index];
	}

	inline const Mango& Mango::operator[](const UInt index) const {
		if (type != MangoType::List) throw MangoTypeError("Type is not list");
		return list[index];
	}

	inline Mango& Mango::operator[](const String& key) {
		if (type != MangoType::Map) throw MangoTypeError("Type is not map");

		try {
			return map[key];
		}
		catch (MapKeyError) {
			throw MangoKeyError("Key not found");
		}
	}

	inline const Mango& Mango::operator[](const String& key) const {
		if (type != MangoType::Map) throw MangoTypeError("Type is not map");
		
		try {
			return map[key];
		}
		catch (MapKeyError) {
			throw MangoKeyError("Key not found");
		}
	}

	inline void Mango::operator=(const bool boolean) {
		type = MangoType::Boolean;
		this->boolean = boolean;
	}

	inline void Mango::operator=(const Int number) {
		type = MangoType::Number;
		integer = true;
		this->number = number;
	}

	inline void Mango::operator=(const Long number) {
		type = MangoType::Number;
		integer = true;
		this->number = (double)number;
	}

	inline void Mango::operator=(const float number) {
		type = MangoType::Number;
		integer = false;
		this->number = number;
	}

	inline void Mango::operator=(const double number) {
		type = MangoType::Number;
		integer = false;
		this->number = number;
	}

	inline void Mango::operator=(const String& string) {
		type = MangoType::String;
		this->string = string;
	}

	inline void Mango::operator=(const List<Mango>& list) {
		type = MangoType::List;
		this->list = list;
	}

	inline void Mango::operator=(const Map<String, Mango>& map) {
		type = MangoType::Map;
		this->map = map;
	}

	inline void Mango::operator=(const Mango& mango) {
		type = mango.type;
		label = mango.label;

		switch (type) {
			case MangoType::Nil: break;
			case MangoType::Boolean: boolean = mango.boolean; break;
			case MangoType::Number: number = mango.number; integer = mango.integer; break;
			case MangoType::String: string = mango.string; break;
			case MangoType::List: list = mango.list; break;
			case MangoType::Map: map = mango.map; break;
		}
	}

	inline void Mango::operator=(Mango&& mango) noexcept {
		type = mango.type;
		label = mango.label;

		switch (type) {
			case MangoType::Nil: break;
			case MangoType::Boolean: boolean = mango.boolean; break;
			case MangoType::Number: number = mango.number; integer = mango.integer; break;
			case MangoType::String: string = std::move(mango.string); break;
			case MangoType::List: list = std::move(mango.list); break;
			case MangoType::Map: map = std::move(mango.map); break;
		}
	}
	
	inline bool Mango::operator==(const Mango& mango) const {
		if (type != mango.type) return false;

		switch (type) {
			case MangoType::Nil: return true;
			case MangoType::Boolean: return boolean == mango.boolean;
			case MangoType::Number: return number == mango.number;
			case MangoType::String: return string == mango.string;
			case MangoType::List: return list == mango.list;
			case MangoType::Map: return map == mango.map;
		}
	}

	inline bool Mango::operator!=(const Mango& mango) const {
		return !operator==(mango);
	}

	inline String Mango::Encode(const Mango& mango, const bool pretty) {
		if (pretty)
			return EncodeNode(mango, "");
		else
			return EncodeNode(mango);
	}

	inline String Mango::EncodeNode(const Mango& mango) {
		static Regex labelPattern = Regex("^%w+$");

		String str = "";

		if (mango.label.Size() > 0) {
			if (labelPattern.Match(mango.label))
				str += mango.label + ":";
			else
				str += "\"" + mango.label.Escape() + "\":";
		}

		switch (mango.type) {
			case MangoType::Nil: {
				str += "nil";
				break;
			}
			case MangoType::Boolean: {
				str += mango.boolean ? "true" : "false";
				break;
			}
			case MangoType::Number: {
				if (mango.integer)
					str += String::ToString((Long)mango.number);
				else
					str += String::ToString(mango.number);
				break;
			}
			case MangoType::String: {
				str += "\"" + mango.string.Escape() + "\"";
				break;
			}
			case MangoType::List: {
				str += "[";

				for (const Mango& m : mango.list)
					str += EncodeNode(m) + " ";

				str = str.Sub(0, str.Size() - 2) + "]";
				break;
			}
			case MangoType::Map: {
				str += "{";

				for (const Pair<String, Mango>& m : mango.map) {
					if (labelPattern.Match(m.key))
						str += m.key + ":";
					else
						str += "\"" + m.key.Escape() + "\":";

					str += EncodeNode(m.value) + " ";
				}

				str = str.Sub(0, str.Size() - 2) + "}";
				break;
			}
		}

		return str;
	}

	inline String Mango::EncodeNode(const Mango& mango, const String& tabs) {
		static Regex labelPattern = Regex("^%w+$");

		const String nextTabs = tabs + "\t";
		String str = "";

		if (mango.label.Size() > 0) {
			if (labelPattern.Match(mango.label))
				str += mango.label + ": ";
			else
				str += "\"" + mango.label.Escape() + "\": ";
		}

		switch (mango.type) {
			case MangoType::Nil: {
				str += "nil";
				break;
			}
			case MangoType::Boolean: {
				str += mango.boolean ? "true" : "false";
				break;
			}
			case MangoType::Number: {
				if (mango.integer)
					str += String::ToString((Long)mango.number);
				else
					str += String::ToString(mango.number);
				break;
			}
			case MangoType::String: {
				str += "\"" + mango.string.Escape() + "\"";
				break;
			}
			case MangoType::List: {
				str += "[\n";

				for (const Mango& m : mango.list)
					str += nextTabs + EncodeNode(m, nextTabs);

				str += tabs + "]";
				break;
			}
			case MangoType::Map: {
				str += "{\n";

				for (const Pair<String, Mango>& m : mango.map) {
					if (labelPattern.Match(m.key))
						str += nextTabs + m.key + ": ";
					else
						str += nextTabs + "\"" + m.key.Escape() + "\": ";

					str += EncodeNode(m.value, nextTabs);
				}

				str += tabs + "}";
				break;
			}
		}

		return str + "\n";
	}

	inline Mango Mango::Decode(const String& mango) {
		List<TokenPattern<MangoTokenType>> patterns;
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::Comment, "%-%-#{%/+}~{%0%-}*%0%-%-", true, true));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::Comment, "%-%-~\n*", true, true));

		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::Boolean, "true|false"));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::Nil, "nil"));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::Number, "%d*%.?%d+"));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::Name, "%w+"));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::String, "\"()\"", true));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::String, "\"(./~{{\\\\}*\\})\"", true));

		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::Colon, "%:", true));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::OpenCurl, "%{", true));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::CloseCurl, "%}", true));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::OpenSq, "%[", true));
		patterns.Add(TokenPattern<MangoTokenType>(MangoTokenType::CloseSq, "%]", true));

		try {
			return DecodeTokens(Lexer::Lex(patterns, mango));
		}
		catch (MangoDecodeError e) {
			throw e;
		}
		catch (LexerError e) {
			throw MangoDecodeError(e.Message());
		}
	}

	inline Mango Mango::DecodeTokens(const List<Token<MangoTokenType>>& tokens) {
		UInt index = 0;
		Optional<Mango> mango = DecodeValue(tokens, index);

		if (index < tokens.Size()) throw MangoDecodeError("Unexpected token: '" + tokens[index].value + "'");

		if (mango) {
			return (Mango)mango;
		}
		else {
			throw MangoDecodeError("Invalid value");
		}
	}

	inline Optional<Mango> Mango::DecodeValue(const List<Token<MangoTokenType>>& tokens, UInt& index) {
		String label = "";

		if (Optional<String> s = DecodeLabel(tokens, index)) {
			label = s.Get();
		}

		if (index >= tokens.Size()) throw MangoDecodeError("Unexpected end of string");

		switch (tokens[index].type) {
			case MangoTokenType::Nil: {
				index++;
				return Mango(label, MangoType::Nil);
			}
			
			case MangoTokenType::Boolean: {
				index++;
				return Mango(label, tokens[index - 1].value == "true");
			}
			
			case MangoTokenType::Number: {
				index++;
				return Mango(label, tokens[index - 1].value.ToDouble());
			}
			
			case MangoTokenType::String: {
				index++;
				return Mango(label, tokens[index - 1].value);
			}
			
			case MangoTokenType::OpenSq: {
				index++;
				Mango list = Mango(label, MangoType::List);

				while (Optional<Mango> value = DecodeValue(tokens, index)) {
					list.Add((Mango)value);
				}

				if (index >= tokens.Size()) throw MangoDecodeError("Unexpected end of string");

				if (tokens[index].type != MangoTokenType::CloseSq) {
					throw MangoDecodeError("']' expected to close list");
				}

				index++;

				return list;
			}

			case MangoTokenType::OpenCurl: {
				index++;
				Mango map = Mango(label, MangoType::Map);

				while (Optional<String> key = DecodeLabel(tokens, index)) {
					if (Optional<Mango> m = DecodeValue(tokens, index)) {
						map.Add((String)key, (Mango)m);
					}
					else {
						throw MangoDecodeError("Value expected after map key");
					}
				}

				if (index >= tokens.Size()) throw MangoDecodeError("Unexpected end of string");

				if (tokens[index].type != MangoTokenType::CloseCurl) {
					throw MangoDecodeError("'}' expected to close map");
				}

				index++;

				return map;
			}
		}
	}

	inline Optional<String> Mango::DecodeLabel(const List<Token<MangoTokenType>>& tokens, UInt& index) {
		if (index >= tokens.Size()) return nullptr;

		if (tokens[index].type == MangoTokenType::Name) {
			if (index + 1 >= tokens.Size()) throw MangoDecodeError("Unexpected end of string");

			if (tokens[index + 1].type == MangoTokenType::Colon) {
				index += 2;
				return tokens[index - 2].value;
			}
		}

		return nullptr;
	}
}
