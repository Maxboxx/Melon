#pragma once

#include "Types.h"
#include "List.h"
#include "Pair.h"
#include "Map.h"
#include "Set.h"
#include "String.h"
#include "Regex.h"
#include "Token.h"
#include "Lexer.h"
#include "Optional.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	class Mango;

	///[Heading] Mango

	///[Title] MangoType
	/// Enum of all types of mango values.
	///[Block] MangoType
	enum class MangoType : UByte {
		/// Nil type.
		Nil,

		/// Boolean type.
		Boolean,

		/// Number type.
		Number,

		/// String type.
		String,

		/// List type.
		List,

		/// Map type.
		Map
	};

	///[Title] MangoList
	/// Typedef for Mango list.
	typedef List<Mango> MangoList;

	///[Title] MangoMap
	/// Typedef for Mango map.
	typedef Map<String, Mango> MangoMap;

	///[Title] Mango
	/// A mango data structure used for mango data.
	///[Block] Mango
	class Mango {
	public:
		///[Heading] Constructors

		/// Nil constructor.
		Mango();

		/// Creates a mango value of the specified type.
		explicit Mango(const MangoType type);

		/// Creates a boolean mango value.
		Mango(const bool boolean);

		/// Creates a mango number from int.
		Mango(const Int number);

		/// Creates a mango number from long.
		Mango(const Long number);

		/// Creates a mango number from float.
		Mango(const float number);

		/// Creates a mango number from double.
		Mango(const double number);

		/// Creates a mango string value.
		Mango(const String& string);

		/// Creates a mango list value.
		Mango(const MangoList& list);

		/// Creates a mango map value.
		Mango(const MangoMap& map);

		/// Creates a labeled mango value of the specified type.
		Mango(const String& label, const MangoType type);

		/// Creates a labeled boolean mango value.
		Mango(const String& label, const bool boolean);

		/// Creates a labeled mango number.
		Mango(const String& label, const Int number);

		/// Creates a labeled mango number.
		Mango(const String& label, const Long number);

		/// Creates a labeled mango number.
		Mango(const String& label, const float number);

		/// Creates a labeled mango number.
		Mango(const String& label, const double number);

		/// Creates a labeled mango string.
		Mango(const String& label, const String& string);

		/// Creates a labeled mango list.
		Mango(const String& label, const MangoList& list);

		/// Creates a labeled mango map.
		Mango(const String& label, const MangoMap& map);

		Mango(const Mango& mango);
		Mango(Mango&& mango) noexcept;
		~Mango();

		///[Heading] Methods

		/// Get the type of the mango value.
		MangoType Type() const;

		/// Setd the label of the mango value.
		/// An empty string will remove the label.
		void SetLabel(const String& label);

		/// Gets the label for this mango value.
		/// If the mango value has no label an empty string is returned.
		String GetLabel() const;

		/// Adds a mango value to the mango list.
		///[Error] MangoTypeError: Thrown if the mango type is not list.
		void Add(const Mango& mango);

		/// Adds a new key to the mango map.
		///[Error] MangoTypeError: Thrown if the mango type is not map.
		void Add(const String& key, const Mango& mango);

		/// Returns the size of the mango list.
		///[Error] MangoTypeError: Thrown if the mango type is not list.
		UInt Size() const;

		/// Copies the mango value.
		Mango Copy() const;

		///[Heading] Operators

		/// Returns the boolean mango value.
		///[Error] MangoTypeError: Thrown if the mango type is not boolean.
		operator bool() const;

		/// Returns the mango number.
		///[Error] MangoTypeError: Thrown if the mango type is not number.
		operator Int() const;

		/// Returns the mango number.
		///[Error] MangoTypeError: Thrown if the mango type is not number.
		operator Long() const;

		/// Returns the mango number.
		///[Error] MangoTypeError: Thrown if the mango type is not number.
		operator float() const;

		/// Returns the mango number.
		///[Error] MangoTypeError: Thrown if the mango type is not number.
		operator double() const;

		/// Returns the mango string.
		///[Error] MangoTypeError: Thrown if the mango type is not string.
		operator String() const;

		/// Returns the mango list.
		///[Error] MangoTypeError: Thrown if the mango type is not list.
		operator MangoList() const;

		/// Returns the mango map.
		///[Error] MangoTypeError: Thrown if the mango type is not map.
		operator MangoMap() const;

		/// Gets or sets a value in the mango list.
		///[Error] MangoTypeError: Thrown if the mango type is not list.
		///M
		Mango& operator[](const UInt index);
		const Mango& operator[](const UInt index) const;
		///M

		/// Gets or sets a value in the mango map.
		///[Error] MangoTypeError: Thrown if the mango type is not map.
		///[Error] MangoKeyError: Thrown if the key does not exist.
		///M
		Mango& operator[](const String& key);
		const Mango& operator[](const String& key) const;
		Mango& operator[](const char* const key);
		const Mango& operator[](const char* const key) const;
		///M

		/// Assigns a boolean value to the mango value.
		/// The mango type is changed to boolean.
		void operator=(const bool boolean);

		/// Assigns a number to the mango value.
		/// The mango type is changed to number.
		void operator=(const Int number);

		/// Assigns a number to the mango value.
		/// The mango type is changed to number.
		void operator=(const Long number);

		/// Assigns a number to the mango value.
		/// The mango type is changed to number.
		void operator=(const float number);

		/// Assigns a number to the mango value.
		/// The mango type is changed to number.
		void operator=(const double number);

		/// Assigns a string to the mango value.
		/// The mango type is changed to string.
		void operator=(const String& string);

		/// Assigns a list to the mango value.
		/// The mango type is changed to list.
		void operator=(const MangoList& list);

		/// Assigns a map to the mango value.
		/// The mango type is changed to map.
		void operator=(const MangoMap& map);

		void operator=(const Mango& mango);
		void operator=(Mango&& mango) noexcept;

		/// Checks the equality of two mango values.
		///M
		bool operator==(const Mango& mango) const;
		bool operator!=(const Mango& mango) const;
		///M

		///[Heading] Static functions

		/// Encodes the mango value to a string.
		///[Arg] pretty: Creates a pretty string if {true}.
		static String Encode(const Mango& mango, const bool pretty = false);

		/// Decodes a string to a mango value.
		///[Arg] version: The version of the mango string to use.
		///[Error] MangoDecodeError: Thrown if the mango string can not be decoded.
		static Mango Decode(const String& mango, const String& version = "");

	private:
		static String EncodeNode(const Mango& mango);
		static String EncodeNode(const Mango& mango, const String& tabs);

		MangoType type;
		String label;
		bool boolean = false;
		bool integer = false;
		double number = 0.0;
		String string;
		MangoList list;
		MangoMap map;

		enum class TokenType : UByte {
			None,
			Comment,
			Name,
			VarName,
			VersionName,
			VersionDefault,
			VersionEnd,
			VersionList,
			VersionContent,
			Boolean,
			Nil,
			Number,
			String,
			Colon,
			Assign,
			OpenCurl,
			CloseCurl,
			OpenSq,
			CloseSq
		};

		struct ParsingInfo {
			String version;
			MangoMap variables;

			ParsingInfo Copy() const {
				ParsingInfo info;
				info.version = version;

				for (const Pair<String, Mango>& pair : variables) {
					info.variables.Add(pair.key, pair.value.Copy());
				}

				return info;
			}
		};

		static Mango Parse(TokenList<TokenType>& tokens, const String& version);
		static String ParseVersion(TokenList<TokenType>& tokens, const String& version);

		static Optional<Mango> ParseNil(TokenList<TokenType>& tokens, ParsingInfo& info);
		static Optional<Mango> ParseBoolean(TokenList<TokenType>& tokens, ParsingInfo& info);
		static Optional<Mango> ParseNumber(TokenList<TokenType>& tokens, ParsingInfo& info);
		static Optional<Mango> ParseString(TokenList<TokenType>& tokens, ParsingInfo& info);
		static Optional<Mango> ParseVariable(TokenList<TokenType>& tokens, ParsingInfo& info);

		static MangoList ParseListItems(TokenList<TokenType>& tokens, ParsingInfo& info, const bool includeVariables = true);
		static Optional<Mango> ParseList(TokenList<TokenType>& tokens, ParsingInfo& info);

		static Optional<Pair<String, Mango>> ParseMapItem(TokenList<TokenType>& tokens, ParsingInfo& info);
		static MangoMap ParseMapItems(TokenList<TokenType>& tokens, ParsingInfo& info, const bool includeVariables = true);
		static Optional<Mango> ParseMap(TokenList<TokenType>& tokens, ParsingInfo& info);

		static Optional<String> ParseRawString(TokenList<TokenType>& tokens, ParsingInfo& info);
		static Optional<String> ParseName(TokenList<TokenType>& tokens, ParsingInfo& info);
		static Optional<String> ParseLabel(TokenList<TokenType>& tokens, ParsingInfo& info);
		static Optional<Mango> ParseValue(TokenList<TokenType>& tokens, ParsingInfo& info, const bool allowInnerLabels = false);
		static Optional<Mango> ParseLabeledValue(TokenList<TokenType>& tokens, ParsingInfo& info, const bool forceLabel = false);

		static bool ParseVariableAssignment(TokenList<TokenType>& tokens, ParsingInfo& info, const bool isList, const bool forceSimple = false);

		static void ValidateVersions(Set<String>& versions, const Set<String>& newVersions, bool& hasDefault);
		static bool VersionMatch(const Set<String>& newVersions, bool& perfectMatch, const String& version);

		static Optional<Set<String>> ParseVersions(TokenList<TokenType>& tokens, ParsingInfo& info);
		static Optional<Mango> ParseVersionValue(TokenList<TokenType>& tokens, ParsingInfo& info, const bool hasLabel);

		static bool ParseVersionVariables(TokenList<TokenType>& tokens, ParsingInfo& info, const bool isList);
		static bool ParseVersionListItems(TokenList<TokenType>& tokens, ParsingInfo& info, MangoList& list);
		static bool ParseVersionMapItems(TokenList<TokenType>& tokens, ParsingInfo& info, MangoMap& map);
	};

	///[Title] MangoError
	/// Base class for all mango errors.
	///[Block] MangoError: Error
	class MangoError : public Error {
	public:
		MangoError() : Error() {}
		MangoError(const char* const msg) : Error(msg) {}

		virtual String Name() const override {
			return "MangoError";
		}
	};

	///[Title] MangoTypeError
	/// Used for type related mango errors.
	///[Block] MangoTypeError: MangoError
	class MangoTypeError : public MangoError {
	public:
		MangoTypeError() : MangoError() {}
		MangoTypeError(const char* const msg) : MangoError(msg) {}

		virtual String Name() const override {
			return "MangoTypeError";
		}
	};

	///[Title] MangoKeyError
	/// Used for map key related errors.
	///[Block] MangoKeyError: MangoError
	class MangoKeyError : public MangoError {
	public:
		MangoKeyError() : MangoError() {}
		MangoKeyError(const char* const msg) : MangoError(msg) {}

		virtual String Name() const override {
			return "MangoKeyError";
		}
	};

	///[Title] MangoDecodeError
	/// Used for decoding errors.
	///[Block] MangoDecodeError: MangoError
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

	inline Mango::Mango(const MangoList& list) {
		type = MangoType::List;
		this->list = list;
	}

	inline Mango::Mango(const MangoMap& map) {
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

	inline Mango::Mango(const String& label, const MangoList& list) {
		type = MangoType::List;
		this->list = list;
		this->label = label;
	}

	inline Mango::Mango(const String& label, const MangoMap& map) {
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

	inline Mango Mango::Copy() const {
		switch (type) {
			case MangoType::Nil: {
				return Mango(label, MangoType::Nil);
			}

			case MangoType::Boolean: {
				return Mango(label, boolean);
			}

			case MangoType::Number: {
				return Mango(label, number);
			}

			case MangoType::String: {
				return Mango(label, string);
			}

			case MangoType::List: {
				MangoList list;

				for (const Mango& mango : this->list) {
					list.Add(mango.Copy());
				}

				return Mango(label, list);
			}

			case MangoType::Map: {
				MangoMap map;

				for (const Pair<String, Mango>& pair : this->map) {
					map.Add(pair.key, pair.value.Copy());
				}

				return Mango(label, map);
			}
		}

		return Mango(MangoType::Nil);
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

	inline Mango::operator MangoList() const {
		if (type != MangoType::List) throw MangoTypeError("Type is not list");
		return list;
	}

	inline Mango::operator MangoMap() const {
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
			throw MangoKeyError("Key '" + key + "' not found");
		}
	}

	inline const Mango& Mango::operator[](const String& key) const {
		if (type != MangoType::Map) throw MangoTypeError("Type is not map");
		
		try {
			return map[key];
		}
		catch (MapKeyError) {
			throw MangoKeyError("Key '" + key + "' not found");
		}
	}

	inline Mango& Mango::operator[](const char* const key) {
		if (type != MangoType::Map) throw MangoTypeError("Type is not map");

		try {
			return map[key];
		}
		catch (MapKeyError) {
			throw MangoKeyError("Key '" + String(key) + "' not found");
		}
	}

	inline const Mango& Mango::operator[](const char* const key) const {
		if (type != MangoType::Map) throw MangoTypeError("Type is not map");

		try {
			return map[key];
		}
		catch (MapKeyError) {
			throw MangoKeyError("Key '" + String(key) + "' not found");
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

	inline void Mango::operator=(const MangoList& list) {
		type = MangoType::List;
		this->list = list;
	}

	inline void Mango::operator=(const MangoMap& map) {
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
						str += nextTabs + m.key;
					else
						str += nextTabs + "\"" + m.key.Escape() + "\"";

					if (m.value.label.Size() == 0) {
						str += ": ";
					}
					else {
						str += " ";
					}

					str += EncodeNode(m.value, nextTabs);
				}

				str += tabs + "}";
				break;
			}
		}

		return str + "\n";
	}

	inline Mango Mango::Decode(const String& mango, const String& version) {
		List<TokenPattern<TokenType>> patterns;
		patterns.Add(TokenPattern<TokenType>(TokenType::Comment, "%-%-#{%/+}~{%0%-}*%0%-%-", true, true));
		patterns.Add(TokenPattern<TokenType>(TokenType::Comment, "%-%-~\n*", true, true));

		patterns.Add(TokenPattern<TokenType>(TokenType::Boolean, "true|false"));
		patterns.Add(TokenPattern<TokenType>(TokenType::Nil, "nil"));
		patterns.Add(TokenPattern<TokenType>(TokenType::Number, "%d*%.?%d+"));
		patterns.Add(TokenPattern<TokenType>(TokenType::Name, "%w+"));
		patterns.Add(TokenPattern<TokenType>(TokenType::String, "\"()\"", true));
		patterns.Add(TokenPattern<TokenType>(TokenType::String, "\"(\\\\)\"", true));
		patterns.Add(TokenPattern<TokenType>(TokenType::String, "\"(./~{{\\\\}*\\})\"", true));

		patterns.Add(TokenPattern<TokenType>(TokenType::VersionEnd, "%#%#", true));
		patterns.Add(TokenPattern<TokenType>(TokenType::VersionList, "%#%[", true));
		patterns.Add(TokenPattern<TokenType>(TokenType::VersionContent, "%#%{", true));

		patterns.Add(TokenPattern<TokenType>(TokenType::VersionName, "%#(%w+)"));
		patterns.Add(TokenPattern<TokenType>(TokenType::VarName, "%$(%w+)"));
		patterns.Add(TokenPattern<TokenType>(TokenType::VersionDefault, "%#()", true));

		patterns.Add(TokenPattern<TokenType>(TokenType::Assign, "%=", true));
		patterns.Add(TokenPattern<TokenType>(TokenType::Colon, "%:", true));
		patterns.Add(TokenPattern<TokenType>(TokenType::OpenCurl, "%{", true));
		patterns.Add(TokenPattern<TokenType>(TokenType::CloseCurl, "%}", true));
		patterns.Add(TokenPattern<TokenType>(TokenType::OpenSq, "%[", true));
		patterns.Add(TokenPattern<TokenType>(TokenType::CloseSq, "%]", true));

		try {
			TokenList<TokenType> tokens = Lexer::Lex(patterns, mango);
			return Parse(tokens, version);
		}
		catch (MangoDecodeError& e) {
			throw e;
		}
		catch (TokenListError& e) {
			throw MangoDecodeError("Unexpected end of string");
		}
		catch (LexerError& e) {
			throw MangoDecodeError(e.Message());
		}
	}

	inline Mango Mango::Parse(TokenList<TokenType>& tokens, const String& version) {
		UInt index = 0;

		ParsingInfo info;
		info.version = ParseVersion(tokens, version);

		Optional<Mango> mango = ParseLabeledValue(tokens, info);

		if (!tokens.AtEnd()) throw MangoDecodeError("Unexpected token: '" + tokens.Current().rawValue + "'");

		if (mango) {
			return *mango;
		}
		else {
			throw MangoDecodeError("Invalid mango value");
		}
	}

	inline String Mango::ParseVersion(TokenList<TokenType>& tokens, const String& version) {
		Optional<String> ver = nullptr;
		Optional<String> verMatch = nullptr;
		bool hasVersions = false;

		for (UInt i = 0; i < 2; i++) {
			if (!ver && tokens.Current().type == TokenType::VersionName) {
				ver = tokens.Current().value;
				tokens.Advance();
			}
			else if (!hasVersions && tokens.Current().type == TokenType::VersionList) {
				Set<String> versions;

				while (tokens.Next().type != TokenType::CloseSq) {
					if (tokens.Current().type != TokenType::Name) {
						throw MangoDecodeError("Unexpected token: '" + tokens.Current().rawValue + "'");
					}

					if (versions.Contains(tokens.Current().value)) {
						throw MangoDecodeError("Duplicate versions in version list");
					}

					versions.Add(tokens.Current().value);

					if (version == tokens.Current().value) verMatch = version;
				}

				tokens.Advance();
				hasVersions = true;
			}
		}

		if (hasVersions) {
			if (verMatch) {
				return *verMatch;
			}
			else if (hasVersions && ver) {
				return *ver;
			}
		}
		else {
			if (version.Size() == 0 && ver) {
				return *ver;
			}
			else {
				return version;
			}
		}

		throw MangoDecodeError("Invalid version");
	}

	inline Optional<Mango> Mango::ParseNil(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (tokens.Current().type == TokenType::Nil) {
			tokens.Advance();
			return Mango(MangoType::Nil);
		}

		return nullptr;
	}

	inline Optional<Mango> Mango::ParseBoolean(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (tokens.Current().type == TokenType::Boolean) {
			tokens.Advance();
			return Mango(tokens.PeekPrevious().value == "true");
		}

		return nullptr;
	}

	inline Optional<Mango> Mango::ParseNumber(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (tokens.Current().type == TokenType::Number) {
			tokens.Advance();
			return Mango(tokens.PeekPrevious().value.ToDouble());
		}

		return nullptr;
	}

	inline Optional<Mango> Mango::ParseString(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (Optional<String> str = ParseRawString(tokens, info)) {
			return Mango(*str);
		}

		return nullptr;
	}

	inline Optional<Mango> Mango::ParseVariable(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (tokens.Current().type == TokenType::VarName) {
			tokens.Advance();
			Mango m;

			if (info.variables.Contains(tokens.PeekPrevious().value, m)) {
				return m.Copy();
			}
			else {
				return Mango(MangoType::Nil);
			}
		}

		return nullptr;
	}

	inline MangoList Mango::ParseListItems(TokenList<TokenType>& tokens, ParsingInfo& info, const bool includeVariables) {
		ParsingInfo parseInfo = info.Copy();
		MangoList list;

		if (includeVariables) {
			while (ParseVariableAssignment(tokens, parseInfo, true));
		}

		while (ParseVersionListItems(tokens, parseInfo, list));

		while (Optional<Mango> item = ParseLabeledValue(tokens, parseInfo)) {
			list.Add(*item);
		}

		return list;
	}

	inline Optional<Mango> Mango::ParseList(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (tokens.Current().type != TokenType::OpenSq) return nullptr;
		tokens.Advance();

		MangoList list = ParseListItems(tokens, info);

		if (tokens.Current().type != TokenType::CloseSq) {
			throw MangoDecodeError("']' expected");	
		}

		tokens.Advance();
		return Mango(list);
	}

	inline Optional<Pair<String, Mango>> Mango::ParseMapItem(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (Optional<String> key = ParseName(tokens, info)) {
			Optional<Mango> value = nullptr;

			if (tokens.Current().type == TokenType::Colon) {
				tokens.Advance();
				value = ParseValue(tokens, info, true);
			}
			else {
				value = ParseLabeledValue(tokens, info, true);
			}

			if (!value) {
				throw MangoDecodeError("Value expected after map key");
			}

			return Pair<String, Mango>(*key, *value);
		}

		return nullptr;
	}

	inline MangoMap Mango::ParseMapItems(TokenList<TokenType>& tokens, ParsingInfo& info, const bool includeVariables) {
		ParsingInfo parseInfo = info.Copy();
		MangoMap map;

		if (includeVariables) {
			while (ParseVariableAssignment(tokens, parseInfo, false));
		}

		while (ParseVersionMapItems(tokens, parseInfo, map));

		while (Optional<Pair<String, Mango>> item = ParseMapItem(tokens, parseInfo)) {
			if (map.Contains(item->key)) {
				throw MangoDecodeError("Duplicate map key");
			}

			map.Add(*item);
		}

		return map;
	}

	inline Optional<Mango> Mango::ParseMap(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (tokens.Current().type != TokenType::OpenCurl) return nullptr;
		tokens.Advance();

		MangoMap map = ParseMapItems(tokens, info);

		if (tokens.Current().type != TokenType::CloseCurl) {
			throw MangoDecodeError("'}' expected");	
		}

		tokens.Advance();
		return Mango(map);
	}

	inline Optional<String> Mango::ParseRawString(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (tokens.Current().type == TokenType::String) {
			tokens.Advance();
			return tokens.PeekPrevious().value;
		}

		return nullptr;
	}

	inline Optional<String> Mango::ParseName(TokenList<TokenType>& tokens, ParsingInfo& info) {
		if (tokens.Current().type == TokenType::Name) {
			tokens.Advance();
			return tokens.PeekPrevious().value;
		}
		else if (Optional<String> str = ParseRawString(tokens, info)) {
			return *str;
		}

		return nullptr;
	}

	inline Optional<String> Mango::ParseLabel(TokenList<TokenType>& tokens, ParsingInfo& info) {
		const UInt pos = tokens.GetPos();

		if (Optional<String> lbl = ParseName(tokens, info)) {
			if (tokens.Current().type == TokenType::Colon) {
				tokens.Advance();
				return lbl;
			}
		}

		tokens.SetPos(pos);
		return nullptr;
	}

	inline Optional<Mango> Mango::ParseValue(TokenList<TokenType>& tokens, ParsingInfo& info, const bool allowInnerLabels) {
		switch (tokens.Current().type) {
			case TokenType::Nil:      return ParseNil(tokens, info);
			case TokenType::Boolean:  return ParseBoolean(tokens, info);
			case TokenType::Number:   return ParseNumber(tokens, info);
			case TokenType::String:   return ParseString(tokens, info);
			case TokenType::OpenSq:   return ParseList(tokens, info);
			case TokenType::OpenCurl: return ParseMap(tokens, info);
			case TokenType::VarName:  return ParseVariable(tokens, info);

			case TokenType::VersionContent: return ParseVersionValue(tokens, info, !allowInnerLabels);
			default: return nullptr;
		}
	}

	inline Optional<Mango> Mango::ParseLabeledValue(TokenList<TokenType>& tokens, ParsingInfo& info, const bool forceLabel) {
		const UInt pos = tokens.GetPos();
		Optional<String> label = ParseLabel(tokens, info);

		if (forceLabel && !label) {
			tokens.SetPos(pos);
			return nullptr;
		}

		if (Optional<Mango> mango = ParseVersionValue(tokens, info, label.HasValue())) {
			if (label) mango->SetLabel(*label);
			return *mango;
		}
		else if (Optional<Mango> mango = ParseValue(tokens, info)) {
			if (label) mango->SetLabel(*label);
			return *mango;
		}

		tokens.SetPos(pos);
		return nullptr;
	}

	inline bool Mango::ParseVariableAssignment(TokenList<TokenType>& tokens, ParsingInfo& info, const bool isList, const bool forceSimple) {
		if (!forceSimple) {
			if (ParseVersionVariables(tokens, info, isList)) {
				return true;
			}
		}

		if (tokens.Current().type != TokenType::VarName) return false;
		const UInt pos = tokens.GetPos();
		const String var = tokens.Current().value;
		tokens.Advance();

		if (tokens.Current().type == TokenType::Assign) {
			tokens.Advance();

			if (Optional<Mango> mango = ParseLabeledValue(tokens, info)) {
				if (info.variables.Contains(var)) {
					info.variables[var] = *mango;
				}
				else {
					info.variables.Add(var, *mango);
				}

				return true;
			}
			else {
				throw MangoDecodeError("Invalid variable assignment");
			}
 		}

		tokens.SetPos(pos);
		return false;
	}

	inline void Mango::ValidateVersions(Set<String>& versions, const Set<String>& newVersions, bool& hasDefault) {
		for (const String& version : newVersions) {
			if (versions.Contains(version)) {
				throw MangoDecodeError("Duplicate version names");
			}

			versions.Add(version);
		}

		if (newVersions.IsEmpty()) {
			if (hasDefault) {
				throw MangoDecodeError("Multiple default versions");
			}
			
			hasDefault = true;
		}
	}

	inline bool Mango::VersionMatch(const Set<String>& newVersions, bool& perfectMatch, const String& version) {
		if (newVersions.Contains(version)) {
			perfectMatch = true;
			return true;
		}
		else if (!perfectMatch && newVersions.IsEmpty()) {
			return true;
		}

		return false;
	}

	inline Optional<Set<String>> Mango::ParseVersions(TokenList<TokenType>& tokens, ParsingInfo& info) {
		const TokenType type = tokens.Current().type;
		
		if (type != TokenType::VersionName && type != TokenType::VersionList && type != TokenType::VersionDefault) {
			return nullptr;
		}

		Set<String> versions;

		switch (tokens.Current().type) {
			case TokenType::VersionName: {
				versions.Add(tokens.Current().value);
				tokens.Advance();
				break;
			}

			case TokenType::VersionList: {
				while (tokens.Next().type == TokenType::Name) {
					if (versions.Contains(tokens.Current().value)) {
						throw MangoDecodeError("Duplicate version names in version list");
					}

					versions.Add(tokens.Current().value);
				}

				if (tokens.Current().type != TokenType::CloseSq) {
					throw MangoDecodeError("']' expected");
				}

				tokens.Advance();
				break;
			}

			case TokenType::VersionDefault: {
				tokens.Advance();
				break;
			}
		}

		return versions;
	}

	inline Optional<Mango> Mango::ParseVersionValue(TokenList<TokenType>& tokens, ParsingInfo& info, const bool hasLabel) {
		if (tokens.Current().type != TokenType::VersionContent) return nullptr;
		tokens.Advance();

		Set<String> versionSet;
		bool hasDefault = false;

		Optional<Mango> value = nullptr;
		bool perfectMatch = false;

		while (Optional<Set<String>> versions = ParseVersions(tokens, info)) {
			ValidateVersions(versionSet, *versions, hasDefault);
			const bool match = VersionMatch(*versions, perfectMatch, info.version);

			Mango mango;

			if (hasLabel || tokens.Current().type == TokenType::Colon) {
				if (tokens.Current().type != TokenType::Colon) {
					throw MangoDecodeError("':' expected after version");
				}

				tokens.Advance();

				if (Optional<Mango> m = ParseValue(tokens, info)) {
					mango = *m;
				}
				else {
					throw MangoDecodeError("Value expected after ':'");
				}
			}
			else {
				if (Optional<Mango> m = ParseLabeledValue(tokens, info, true)) {
					mango = *m;
				}
				else {
					throw MangoDecodeError("Value expected after ':'");
				}
			}

			if (match) value = mango;
		}

		if (tokens.Current().type != TokenType::CloseCurl) {
			throw MangoDecodeError("'}' expected");
		}

		tokens.Advance();

		if (value) {
			return *value;
		}
		else {
			return Mango(MangoType::Nil);
		}
	}

	inline bool Mango::ParseVersionVariables(TokenList<TokenType>& tokens, ParsingInfo& info, const bool isList) {
		const UInt pos = tokens.GetPos();

		Set<String> versionSet;
		bool hasDefault = false;

		Optional<ParsingInfo> parsingInfo = nullptr;
		bool perfectMatch = false;
		bool found = false;

		while (Optional<Set<String>> versions = ParseVersions(tokens, info)) {
			ValidateVersions(versionSet, *versions, hasDefault);
			const bool match = VersionMatch(*versions, perfectMatch, info.version);

			ParsingInfo pi = info.Copy();

			if (tokens.Current().type == TokenType::Colon) {
				tokens.Advance();

				if (isList && tokens.Current().type == TokenType::OpenSq) {
					tokens.Advance();
				}
				else if (!isList && tokens.Current().type == TokenType::OpenCurl) {
					tokens.Advance();
				}
				else {
					if (found) {
						throw MangoDecodeError("'" + String(isList ? '[' : '{') + "' expected");
					}
					else {
						tokens.SetPos(pos);
						return false;
					}
				}

				while (ParseVariableAssignment(tokens, pi, isList));

				if (isList && tokens.Current().type == TokenType::CloseSq) {
					tokens.Advance();
				}
				else if (!isList && tokens.Current().type == TokenType::CloseCurl) {
					tokens.Advance();
				}
				else {
					if (found) {
						throw MangoDecodeError("'" + String(isList ? ']' : '}') + "' expected");
					}
					else {
						tokens.SetPos(pos);
						return false;
					}
				}
			}
			else if (!ParseVariableAssignment(tokens, pi, isList, true)) {
				if (found) {
					throw MangoDecodeError("Variable assignment expected after version name");
				}
				else {
					tokens.SetPos(pos);
					return false;
				}
			}

			if (match) parsingInfo = pi;
			found = true;
		}

		if (!found) {
			tokens.SetPos(pos);
			return false;
		}

		if (tokens.Current().type == TokenType::VersionEnd) {
			tokens.Advance();
		}

		if (parsingInfo) info = *parsingInfo;
		return true;
	}

	inline bool Mango::ParseVersionListItems(TokenList<TokenType>& tokens, ParsingInfo& info, MangoList& list) {
		Set<String> versionSet;
		bool hasDefault = false;

		Optional<MangoList> mangoList = nullptr;
		bool perfectMatch = false;
		bool found = false;

		while (Optional<Set<String>> versions = ParseVersions(tokens, info)) {
			ValidateVersions(versionSet, *versions, hasDefault);
			const bool match = VersionMatch(*versions, perfectMatch, info.version);

			MangoList mlist;

			if (tokens.Current().type == TokenType::Colon) {
				tokens.Advance();

				if (tokens.Current().type == TokenType::OpenSq) {
					tokens.Advance();

					mlist = ParseListItems(tokens, info, false);

					if (tokens.Current().type == TokenType::CloseSq) {
						tokens.Advance();
					}
					else {
						throw MangoDecodeError("']' expected");
					}
				}
				else {
					throw MangoDecodeError("'[' expected");
				}
			}
			else {
				throw MangoDecodeError("':' expected after version name");
			}

			if (match) mangoList = mlist;
			found = true;
		}

		if (!found) return false;

		if (tokens.Current().type == TokenType::VersionEnd) {
			tokens.Advance();
		}

		if (mangoList) {
			for (const Mango& m : *mangoList) {
				list.Add(m);
			}
		}

		return true;
	}

	inline bool Mango::ParseVersionMapItems(TokenList<TokenType>& tokens, ParsingInfo& info, MangoMap& map) {
		Set<String> versionSet;
		bool hasDefault = false;

		Optional<MangoMap> mangoMap = nullptr;
		bool perfectMatch = false;
		bool found = false;

		while (Optional<Set<String>> versions = ParseVersions(tokens, info)) {
			ValidateVersions(versionSet, *versions, hasDefault);
			const bool match = VersionMatch(*versions, perfectMatch, info.version);

			MangoMap mmap;

			if (tokens.Current().type == TokenType::Colon) {
				tokens.Advance();

				if (tokens.Current().type == TokenType::OpenCurl) {
					tokens.Advance();

					mmap = ParseMapItems(tokens, info, false);

					if (tokens.Current().type == TokenType::CloseCurl) {
						tokens.Advance();
					}
					else {
						throw MangoDecodeError("'}' expected");
					}
				}
				else {
					throw MangoDecodeError("'{' expected");
				}
			}
			else if (Optional<Pair<String, Mango>> pair = ParseMapItem(tokens, info)) {
				mmap.Add(*pair);
			}
			else {
				throw MangoDecodeError("':' expected after version name");
			}

			if (match) mangoMap = mmap;
			found = true;
		}

		if (!found) return false;

		if (tokens.Current().type == TokenType::VersionEnd) {
			tokens.Advance();
		}

		if (mangoMap) {
			for (const Pair<String, Mango>& m : *mangoMap) {
				if (map.Contains(m.key)) {
					throw MangoDecodeError("Duplicate map keys");
				}

				map.Add(m);
			}
		}

		return true;
	}
}

