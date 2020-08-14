#pragma once

#include "Error.h"
#include "Types.h"
#include "String.h"
#include "Array.h"
#include "List.h"
#include "Set.h"
#include "Math.h"
#include "Pointer.h"
#include "Stack.h"

///N Regex
namespace Boxx {
	///B Regex
	/// Class for parsing strings using a regular expression pattern
	class Regex {
	public:
		///H Constructors

		///T Empty
		/// Creates an empty regex pattern
		Regex();

		///T Pattern
		/// Creates a regex with a pattern using the following format:
		///C Groups
		///V (...): Matches the expression between <code>(</code> and <code>)</code>
		/// The matched expression will be added to the returned list from the <code>Match()</code> method.
		/// If no groups are present in the regex the entire matched string will be returned from the <code>Match()</code> method.
		///V {...}: Matches the expression between <code>{</code> and <code>}</code>
		/// The matched expression will not be returned from the <code>Match()</code> method.
		///C Quantifiers
		///V +: Matches the previous element one or more times as many times as possible
		///V *: Matches the previous element zero or more times as many times as possible
		///V -: Matches the previous element one or more times as few times as possible
		///V /: Matches the previous element zero or more times as few times as possible
		///V ?: Matches the previous element zero or one time
		///V <>: User defined quantifier that can be written in the following ways:
		/// <code>&lt;n&gt;</code> Matches the previous element <code>n</code> times
		/// <code>&lt;n:m&gt;</code> Matches the previous element a minimum of <code>n</code> times and a maximum of <code>m</code> times
		/// <code>&lt;n:&gt;</code> Matches the previous element <code>n</code> or more times
		/// <code>&lt;:n&gt;</code> Matches the previous element a minimum of zero times and a maximum of <code>n</code> times
		/// <br>
		/// A <code>+</code> can be added right before <code>&gt;</code> to match as many times as possible
		/// and a <code>-</code> can be added to match as few times as possible.
		/// It defaults to <code>+</code> if omitted
		///C Character sets
		///V .: Matches any character
		///V %d: Matches a didgit
		///V %l: Matches a lower case letter
		///V %u: Matches an upper case letter
		///V %a: Matches a letter
		///V %v: Matches an alphanumeric character
		///V %w: Matches an alphanumeric character or an underscore
		///V %x: Matches a hexadecimal didgit
		///V %p: Matches a punctuation character
		///V %s: Matches a space or a tab
		///V %n: Macthes a whitespace character
		///V %b: Matches a word boundary
		///V a::b: Matches any character in the range from <code>a</code> to <code>b</code>
		///V [...]: Custom character set. Matches any character present between <code>[</code> and <code>]</code>.
		/// Predefined character sets and ranges works inside the custom character set
		///C Special characters
		///V ~: Inverse match. Matches anything except the next element
		///V #: Element match. The result of an element prefixed by <code>#</code> can be reffered to by using <code>%</code> followed by the number of that element starting at 1.
		///V ^: Matches the start of the search.
		///V $: Matches the end of the string.
		///V |: Matches either the expression before or after the <code>|</code> character
		///V !: Empty match. Matches an empty string. It is always successful.
		///V %: Escape character. Treats the next character in the pattern as a literal charater unless it is a predefined character set
		///E RegexPatternError: Thrown if there are syntax errors in the pattern
		explicit Regex(const String& pattern);

		Regex(const Regex& regex);
		Regex(Regex&& regex) noexcept;
		~Regex();

		///H Methods

		///T Match
		/// Matches a string using the regex pattern
		///A UInt pos: The position in the string to start at
		///R Array<String> matches: A list of all matches found in the string
		/// Returns an empty array if no matches where found
		Array<String> Match(const String& string, UInt pos = 0) const;

		///H Static functions

		///T Escape string
		/// Escapes some special characters in a string to make the regex engine interpret the string literally
		static String Escape(const String& str);

		void operator=(const Regex& regex);
		void operator=(Regex&& regex) noexcept;

	private:
		enum class Chars : char {
			// Misc
			None = '\0',
			Escape = '%',
			Start = '^',
			End = '$',
			Choice = '|',
			Range = ':',
			Any = '.',
			Skip = '!',

			// Delimiters
			GroupOpen = '(',
			GroupClose = ')',
			HiddenGroupOpen = '{',
			HiddenGroupClose = '}',
			SetOpen = '[',
			SetClose = ']',
			QuantifierOpen = '<',
			QuantifierClose = '>',

			// Prefix
			ElementMatch = '#',
			Inverse = '~',

			// Suffix
			Many = '+',
			ManyOptional = '*',
			Few = '-',
			FewOptional = '/',
			Optional = '?',

			// Char sets
			Didgit = 'd',
			Lower = 'l',
			Upper = 'u',
			Alpha = 'a',
			AlphaNumeric = 'v',
			Word = 'w',
			Hex = 'x',
			Punctuation = 'p',
			Space = 's',
			WhiteSpace = 'n',
			Boundary = 'b'
		};

		enum class CharSets : char {
			NumStart = '0',
			NumEnd = '9',
			LowerStart = 'a',
			LowerEnd = 'z',
			UpperStart = 'A',
			UpperEnd = 'Z',
			WordExtra = '_'
		};

		static Set<char> SetupPunctuation();

		static bool Punctuation(const char c);

		static bool Word(const char c);
		static bool AlphaNumeric(const char c);
		static bool Alpha(const char c);
		static bool Lower(const char c);
		static bool Upper(const char c);
		static bool Numeric(const char c);

		static bool Space(const char c);
		static bool WhiteSpace(const char c);

		static bool IsPrefix(const char c);
		static bool IsSuffix(const char c);
		static bool IsSpecial(const char c);

		struct GroupValues;
		struct QuantifierValues;
		struct Pattern;

		struct PatternInfo {
			const char* first{};
			const char* last{};
			List<String> groups;
			Stack<GroupValues> groupStack;
			Stack<Pointer<QuantifierValues>> quantifierStack;
			Stack<Pointer<Pattern>> choiceStack;

			~PatternInfo() {}
		};

		struct Pattern {
			Pointer<Pattern> next = nullptr;
			bool pastEnd = false;

			~Pattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) {
				const char* c = Next(str, matches, info);
				if (c != nullptr && matches.IsEmpty()) 
					matches.Add(String(str, (UInt)(c - str)));
				return c;
			}

			const char* Next(const char* str, List<String>& matches, PatternInfo& info) {
				if (next != nullptr) {
					if (str < info.last || next->pastEnd)
						return next->Match(str, matches, info);
					return nullptr;
				}

				if (str - 1 >= info.last)
					return nullptr;
				return str;
			}

			static bool CanContinue(const Pointer<Pattern>& next, const char* str, const PatternInfo& info) {
				return next != nullptr && (str < info.last || next->pastEnd);
			}
		};

		Pointer<Pattern> patterns;
		bool start;
		bool end;

		struct GroupValues {
			const char* str{};
			bool hidden{};
			bool elementMatch{};
			Pointer<Pattern> next;
			UInt match{};
			UInt group{};

			~GroupValues() {}
		};

		struct GroupPattern : public Pattern {
			GroupPattern() {pastEnd = true;}
			~GroupPattern() {}

			bool hidden = false;
			bool elementMatch = false;
			Pointer<Pattern> content = nullptr;

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				GroupValues values;
				values.hidden = hidden;
				values.elementMatch = elementMatch;
				values.next = next;
				values.str = str;

				if (!hidden) {
					values.match = matches.Size();
					matches.Add("");
				}

				if (elementMatch) {
					values.group = info.groups.Size();
					info.groups.Add("");
				}

				info.groupStack.Push(values);
				const char* c = content->Match(str, matches, info);
				info.groupStack.Pop();

				if (!hidden && c == nullptr) {
					matches.RemoveAt(values.match);
				}

				if (elementMatch && c == nullptr) {
					info.groups.RemoveAt(values.group);
				}

				return c;
			}
		};

		struct GroupEndPattern : public Pattern {
			GroupEndPattern() {pastEnd = true;}
			~GroupEndPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				GroupValues values = info.groupStack.Peek();

				if (!values.hidden)
					matches[values.match] = String(values.str, (UInt)(str - values.str));
				if (values.elementMatch)
					info.groups[values.group] = String(values.str, (UInt)(str - values.str));

				const char* c = str;

				if (values.next != nullptr) {
					if (Pattern::CanContinue(values.next, str, info)) {
						info.groupStack.Pop();
						c = values.next->Match(str, matches, info);
						info.groupStack.Push(values);
					}
					else {
						c = nullptr;
					}

					if (c == nullptr) {
						if (!values.hidden) matches[values.match] = "";
						if (values.elementMatch) info.groups[values.group] = "";
					}
				}

				return c;
			}
		};

		struct ElementMatchPattern : public Pattern {
			ElementMatchPattern() {pastEnd = true;}
			~ElementMatchPattern() {}

			UInt num = 0;

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (num >= info.groups.Size()) return nullptr;

				const String match = info.groups[num];
				UInt len = match.Size();

				if (info.last - str < len) return nullptr;

				UInt i = 0;

				while (i < len && *(str + i) == match[i]) {
					i++;
				}

				if (i == len) return Next(str + len, matches, info);
				return nullptr;
			}
		};

		struct AnyPattern : public Pattern {
			~AnyPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				return Next(str + 1, matches, info);
			}
		};

		struct SkipPattern : public Pattern {
			~SkipPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				return Next(str, matches, info);
			}
		};

		struct StartPattern : public Pattern {
			~StartPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (str == info.first)
					return Next(str, matches, info);
				return nullptr;
			}
		};

		struct EndPattern : public Pattern {
			EndPattern() {pastEnd = true;}
			~EndPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (str == info.last)
					return Next(str, matches, info);
				return nullptr;
			}
		};

		struct InversePattern : public Pattern {
			~InversePattern() {}

			Pointer<Pattern> content;

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (content->Match(str, matches, info) == nullptr)
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct CharPattern : public Pattern {
			~CharPattern() {}

			char c{};

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (*str == c)
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct PunctuationPattern : public Pattern {
			~PunctuationPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (Punctuation(*str))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct WordPattern : public Pattern {
			~WordPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (Word(*str))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct AlphaNumericPattern : public Pattern {
			~AlphaNumericPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (AlphaNumeric(*str))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct AlphaPattern : public Pattern {
			~AlphaPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (Alpha(*str))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct LowerPattern : public Pattern {
			~LowerPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (Lower(*str))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct UpperPattern : public Pattern {
			~UpperPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (Upper(*str))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct NumericPattern : public Pattern {
			~NumericPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (Numeric(*str))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct HexPattern : public Pattern {
			~HexPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (Numeric(*str) || (*str >= 'a' && *str <= 'f') || (*str >= 'A' && *str <= 'F'))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct SpacePattern : public Pattern {
			~SpacePattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (Space(*str))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct WhiteSpacePattern : public Pattern {
			~WhiteSpacePattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (WhiteSpace(*str))
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct BoundaryPattern : public Pattern {
			BoundaryPattern() {pastEnd = true;}
			~BoundaryPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (str >= info.last || str - 1 < info.first) return Next(str, matches, info);
				if (Word(*str) && !Word(*(str - 1))) return Next(str, matches, info);
				if (!Word(*str) && Word(*(str - 1))) return Next(str, matches, info);
				return nullptr;
			}
		};

		struct RangePattern : public Pattern {
			~RangePattern() {}

			char c1{}, c2{};

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				if (*str >= c1 && *str <= c2)
					return Next(str + 1, matches, info);
				return nullptr;
			}
		};

		struct CharSetPattern : public Pattern {
			~CharSetPattern() {}

			Array<Pointer<Pattern>> patterns;

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				const char* c;

				for (Pointer<Pattern>& pattern : patterns) {
					c = pattern->Match(str, matches, info);

					if (c != nullptr) {
						return Next(str + 1, matches, info);
					}
				}

				return nullptr;
			}
		};

		struct ChoicePattern : public Pattern {
			ChoicePattern() {pastEnd = true;}
			~ChoicePattern() {}

			Array<Pointer<Pattern>> patterns;

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				const char* c = nullptr;

				info.choiceStack.Push(next);

				for (Pointer<Pattern>& pattern : patterns) {
					if (Pattern::CanContinue(pattern, str, info)) {
						c = pattern->Match(str, matches, info);
					}

					if (c != nullptr) {
						info.choiceStack.Pop();
						return c;
					}
				}

				info.choiceStack.Pop();
				return nullptr;
			}
		};

		struct ChoiceEndPattern : public Pattern {
			ChoiceEndPattern() {pastEnd = true;}
			~ChoiceEndPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				Pointer<Pattern> next = info.choiceStack.Pop();
				const char* c = nullptr;

				if (Pattern::CanContinue(next, str, info))
					c = next->Match(str, matches, info);

				info.choiceStack.Push(next);
				return c;
			}
		};

		struct QuantifierValues {
			UInt min{};
			UInt max{};
			UInt current{};
			bool many{};
			Pointer<Pattern> next;
			Pointer<Pattern> content;

			~QuantifierValues() {}
		};

		struct QuantifierPattern : public Pattern {
			QuantifierPattern() {pastEnd = true;}
			~QuantifierPattern() {}

			UInt min = Math::UIntMin();
			UInt max = Math::UIntMax();
			Pointer<Pattern> content;
			bool many{};

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				Pointer<QuantifierValues> values = new QuantifierValues();
				values->min = min;
				values->max = max;
				values->current = 1;
				values->many = many;
				values->next = next;
				values->content = content;

				if (!many) {
					const char* c;

					if (min == 0) {
						c = Next(str, matches, info);
						if (c != nullptr) return c;
					}

					info.quantifierStack.Push(values);
					c = content->Match(str, matches, info);
					info.quantifierStack.Pop();
					return c;
				}
				else {
					const char* c;

					info.quantifierStack.Push(values);
					c = content->Match(str, matches, info);
					info.quantifierStack.Pop();
					
					if (c != nullptr) return c;

					if (min == 0) {
						c = Next(str, matches, info);
						if (c != nullptr) return c;
					}

					return nullptr;
				}
			}
		};

		struct QuantifierEndPattern : public Pattern {
			QuantifierEndPattern() {pastEnd = true;}
			~QuantifierEndPattern() {}

			virtual const char* Match(const char* str, List<String>& matches, PatternInfo& info) override {
				Pointer<QuantifierValues> values = info.quantifierStack.Peek();

				if (!values->many) {
					if (values->current >= values->min) {
						const char* c = str;

						if (values->next != nullptr) {
							if (Pattern::CanContinue(values->next, str, info)) {
								info.quantifierStack.Pop();
								c = values->next->Match(str, matches, info);
								info.quantifierStack.Push(values);
							}
							else {
								c = nullptr;
							}
						}

						if (c != nullptr) return c;
					}

					if (++values->current > values->max) return nullptr;

					if (Pattern::CanContinue(values->content, str, info))
						return values->content->Match(str, matches, info);
					else
						return nullptr;
				}
				else {
					if (values->current < values->max && Pattern::CanContinue(values->content, str, info)) {
						values->current++;
						const char* c = values->content->Match(str, matches, info);
						if (c != nullptr) return c;
						values->current--;
					}
					
					if (values->current < values->min) return nullptr;
					
					const char* c = str;

					if (values->next != nullptr) {
						if (Pattern::CanContinue(values->next, str, info)) {
							info.quantifierStack.Pop();
							c = values->next->Match(str, matches, info);
							info.quantifierStack.Push(values);
						}
						else {
							c = nullptr;
						}
					}

					if (c != nullptr) return c;

					return nullptr;
				}
			}
		};

		enum class Type : UByte {
			None,
			Group,
			HiddenGroup,
			Set
		};

		Pointer<Pattern> ParseString(const String& pattern, UInt& pos, const Type type = Type::None);
		Pointer<Pattern> CreatePattern(const List<Pointer<Pattern>>& patterns, const Type type = Type::None);
	};

	///B Regex error
	/// The base class for all regex errors
	class RegexError : public Error {
	public:
		RegexError():Error(){}
		RegexError(const char* const msg):Error(msg){}
	};

	///B Regex pattern error
	/// Thrown if there are syntax errors in the regex pattern
	class RegexPatternError : public RegexError {
	public:
		RegexPatternError():RegexError(){}
		RegexPatternError(const char* const msg):RegexError(msg){}
	};

	inline Set<char> Regex::SetupPunctuation() {
		const String p = "[!\"#$%&'()*+,./:;<=>?@\\^`{|}~-]";
		Set<char> set = Set<char>(p.Size());

		for (const char c : p)
			set.Add(c);

		return set;
	}
	
	inline Regex::Regex() {
		start = false;
		end = false;
	}

	inline Regex::Regex(const String& pattern) {
		UInt pos = 0;
		patterns = ParseString(pattern, pos);
	}

	inline Regex::Regex(const Regex& regex) {
		patterns = regex.patterns;
	}

	inline Regex::Regex(Regex&& regex) noexcept {
		patterns = regex.patterns;
	}

	inline Regex::~Regex() {

	}

	inline void Regex::operator=(const Regex& regex) {
		patterns = regex.patterns;
	}

	inline void Regex::operator=(Regex&& regex) noexcept {
		patterns = regex.patterns;
	}

	inline Array<String> Regex::Match(const String& string, UInt pos) const {
		if (patterns == nullptr) return Array<String>();
		const char* str = string;

		PatternInfo info;
		info.first = str + pos;
		info.last = str + string.Size();

		List<String> matches;
		patterns->Match(info.first, matches, info);

		if (!matches.IsEmpty())
			return matches.ToArray();
		else if (patterns.Cast<StartPattern>() != nullptr)
			return Array<String>();

		for (UInt i = pos + 1; i < string.Size(); i++) {
			patterns->Match(str + i, matches, info);

			if (!matches.IsEmpty())
				return matches.ToArray();
		}

		return Array<String>();
	}

	inline String Regex::Escape(const String& str) {
		List<char> chars{str.Size()};

		for (const char c : str) {
			if (Word(c)) {
				chars.Add(c);
			}
			else {
				chars.Add((char)Chars::Escape);
				chars.Add(c);
			}
		}

		return String(chars);
	}

	inline bool Regex::Punctuation(const char c) {
		static const Set<char> punctuation = SetupPunctuation();
		return punctuation.Contains(c);
	}

	inline bool Regex::Word(const char c) {
		return AlphaNumeric(c) || c == (char)CharSets::WordExtra;
	}

	inline bool Regex::AlphaNumeric(const char c) {
		return Alpha(c) || Numeric(c);
	}

	inline bool Regex::Alpha(const char c) {
		return Lower(c) || Upper(c);
	}

	inline bool Regex::Lower(const char c) {
		return c >= (char)CharSets::LowerStart && c <= (char)CharSets::LowerEnd;
	}

	inline bool Regex::Upper(const char c) {
		return c >= (char)CharSets::UpperStart && c <= (char)CharSets::UpperEnd;
	}

	inline bool Regex::Numeric(const char c) {
		return c >= (char)CharSets::NumStart && c <= (char)CharSets::NumEnd;
	}

	inline bool Regex::Space(const char c) {
		return c == ' ' || c == '\t';
	}

	inline bool Regex::WhiteSpace(const char c) {
		return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\v';
	}

	inline bool Regex::IsPrefix(const char c) {
		return c == (char)Chars::Inverse;
	}

	inline bool Regex::IsSuffix(const char c) {
		return c == (char)Chars::Many || c == (char)Chars::ManyOptional || c == (char)Chars::Few || c == (char)Chars::FewOptional || c == (char)Chars::Optional;
	}

	inline bool Regex::IsSpecial(const char c) {
		return c == (char)Chars::Any || c == (char)Chars::Start || c == (char)Chars::End;
	}

	inline Pointer<Regex::Pattern> Regex::CreatePattern(const List<Pointer<Regex::Pattern>>& patterns, const Regex::Type type) {
		Pointer<Pattern> base;
		
		if (type == Type::None)
			base = new Pattern();
		else if (type == Type::HiddenGroup) {
			GroupPattern* s = new GroupPattern();
			s->hidden = true;
			base = s;
		}
		else if (type == Type::Group)
			base = new GroupPattern();
		else if (type == Type::Set) {
			CharSetPattern* s = new CharSetPattern();
			s->patterns = patterns.ToArray();
			base = s;
		}

		if (type != Type::Set) {
			if (!patterns.IsEmpty()) {
				if (type == Type::None)
					base->next = patterns[0];
				else
					base.Cast<GroupPattern>()->content = patterns[0];

				for (UInt i = 1; i < patterns.Size(); i++)
					patterns[i - 1]->next = patterns[i];

				if (type != Type::None) {
					patterns.Last()->next = new GroupEndPattern();
				}
			}
			else {
				if (type != Type::None)
					base.Cast<GroupPattern>()->content = new GroupEndPattern();
			}
		}

		return base;
	}

	inline Pointer<Regex::Pattern> Regex::ParseString(const String& pattern, UInt& pos, const Regex::Type type) {
		List<Pointer<Pattern>> patterns;
		List<Pointer<Pattern>> choicePatterns;
		bool escape = false;
		bool range = false;
		bool choice = false;
		UByte inverse = 0;
		UByte elementMatch = 0;

		for (; pos < pattern.Size(); pos++) {
			const char c = pattern[pos];

			if (type != Type::None && !escape) {
				if (
					(type == Type::Group && c == (char)Chars::GroupClose) ||
					(type == Type::HiddenGroup && c == (char)Chars::HiddenGroupClose) ||
					(type == Type::Set && c == (char)Chars::SetClose)
				) {
					if (range) throw RegexPatternError("Invalid range end");

					if (choice) {
						if (patterns.Size() == 1)
							choicePatterns.Add(patterns[0]);
						else
							choicePatterns.Add(CreatePattern(patterns, Type::HiddenGroup));

						for (Pointer<Pattern>& pattern : choicePatterns)
							pattern->next = new ChoiceEndPattern();

						ChoicePattern* const s = new ChoicePattern();
						s->patterns = choicePatterns.ToArray();

						patterns = List<Pointer<Pattern>>();
						patterns.Add(s);
					}
					
					return CreatePattern(patterns, type);
				}
			}

			if (escape) {
				if (!range) {
					if (c == (char)Chars::Didgit)
						patterns.Add(new NumericPattern());
					else if (c == (char)Chars::Lower)
						patterns.Add(new LowerPattern());
					else if (c == (char)Chars::Upper)
						patterns.Add(new UpperPattern());
					else if (c == (char)Chars::Alpha)
						patterns.Add(new AlphaPattern());
					else if (c == (char)Chars::AlphaNumeric)
						patterns.Add(new AlphaNumericPattern());
					else if (c == (char)Chars::Word)
						patterns.Add(new WordPattern());
					else if (c == (char)Chars::Hex)
						patterns.Add(new HexPattern());
					else if (c == (char)Chars::Punctuation)
						patterns.Add(new PunctuationPattern());
					else if (c == (char)Chars::Space)
						patterns.Add(new SpacePattern());
					else if (c == (char)Chars::WhiteSpace)
						patterns.Add(new WhiteSpacePattern());
					else if (c == (char)Chars::Boundary)
						patterns.Add(new BoundaryPattern());
					else if (type != Type::Set && Numeric(c)) {
						UInt start = pos;

						while (pos < pattern.Size() - 1 && Numeric(pattern[pos + 1])) {
							pos++;
						}

						UInt num = pattern.Sub(start, pos).ToInt();

						if (num > 0) {
							ElementMatchPattern* const g = new ElementMatchPattern();
							g->num = num - 1;
							patterns.Add(g);
						}
						else throw RegexPatternError("Element match number is zero. Element matches start at one");
					}
					else {
						CharPattern* const s = new CharPattern();
						s->c = c;
						patterns.Add(s);
					}
				}
				else {
					bool err = false;

					if (c == (char)Chars::Didgit)
						err = true;
					else if (c == (char)Chars::Lower)
						err = true;
					else if (c == (char)Chars::Upper)
						err = true;
					else if (c == (char)Chars::Alpha)
						err = true;
					else if (c == (char)Chars::AlphaNumeric)
						err = true;
					else if (c == (char)Chars::Word)
						err = true;
					else if (c == (char)Chars::Hex)
						err = true;
					else if (c == (char)Chars::Punctuation)
						err = true;
					else if (c == (char)Chars::Space)
						err = true;
					else if (c == (char)Chars::WhiteSpace)
						err = true;
					else if (c == (char)Chars::Boundary)
						err = true;

					if (err) {
						throw RegexPatternError("Invalid range end");
					}

					if (patterns.Last().Cast<InversePattern>() != nullptr) throw RegexPatternError("Expected pattern after '" + String((char)Chars::Inverse) + "'");
					if (patterns.Last().Cast<InversePattern>() != nullptr) throw RegexPatternError("Expected pattern after '" + String((char)Chars::ElementMatch) +  "'");

					patterns.Last().Cast<RangePattern>()->c2 = c;
					range = false;
				}

				escape = false;
			}
			else if (type != Type::Set && (c == (char)Chars::GroupOpen || c == (char)Chars::HiddenGroupOpen || c == (char)Chars::SetOpen)) {
				if (range) throw RegexPatternError("Invalid range end");

				if (c == (char)Chars::GroupOpen) {
					patterns.Add(ParseString(pattern, ++pos, Type::Group));
				}
				else if (c == (char)Chars::HiddenGroupOpen) {
					patterns.Add(ParseString(pattern, ++pos, Type::HiddenGroup));
				}
				else if (c == (char)Chars::SetOpen) {
					patterns.Add(ParseString(pattern, ++pos, Type::Set));
				}
			}
			else if (type != Type::Set && IsSuffix(c)) {
				if (range) throw RegexPatternError("Invalid range end");
				
				Pointer<Pattern> prev = patterns.Last();
				QuantifierPattern* const q = new QuantifierPattern();
				q->content = prev;
				prev->next = new QuantifierEndPattern();

				if (c == (char)Chars::Many) {
					q->many = true;
					q->min = 1;
				}
				else if (c == (char)Chars::ManyOptional) {
					q->many = true;
				}
				else if (c == (char)Chars::Few) {
					q->many = false;
					q->min = 1;
				}
				else if (c == (char)Chars::FewOptional) {
					q->many = false;
				}
				else {
					q->many = true;
					q->min = 0;
					q->max = 1;
				}

				patterns[patterns.Size() - 1] = q;
			}
			else if (type != Type::Set && c == (char)Chars::Choice) {
				if (!choice) choice = true;

				if (patterns.Size() == 1)
					choicePatterns.Add(patterns[0]);
				else
					choicePatterns.Add(CreatePattern(patterns, Type::HiddenGroup));

				patterns = List<Pointer<Pattern>>();
			}
			else if (type != Type::Set && c == (char)Chars::QuantifierOpen) {
				UInt len = pattern.Size() - 1;
				UInt s = pos + 1;

				if (s > len) throw RegexPatternError("Unfinished quantifier");

				while (pattern[++pos] == (char)Chars::Range || Numeric(pattern[pos])) {
					if (pos + 1 > len) throw RegexPatternError("Unfinished quantifier");
				}

				if (pos > len) throw RegexPatternError("Unfinished quantifier");
				Array<String> nums = pattern.Sub(s, pos - 1).Split((char)Chars::Range);

				if (nums.IsEmpty()) throw RegexPatternError("Expected number in quantifier");
				if (nums.Size() > 2) throw RegexPatternError("Unexpexted ':' in quantifier");

				UInt min = Math::UIntMin();
				UInt max = Math::UIntMax();

				if (nums.Size() == 1) {
					if (nums[0].Size() > 0) {
						min = nums[0].ToInt();
						max = min;
					}
					else {
						throw RegexPatternError("Expected number in quantifier");
					}
				}
				else if (nums.Size() == 2) {
					bool hasMin = false;
					bool hasMax = false;

					if (nums[0].Size() > 0) {
						hasMin = true;
						min = nums[0].ToInt();
					}
					if (nums[1].Size() > 0) {
						hasMax = true;
						max = nums[1].ToInt();
					}

					if (hasMin || hasMax) {
						
					}
					else {
						throw RegexPatternError("Expected number in quantifier");
					}
				}

				if (max == 0) throw RegexPatternError("Quantifier range end is zero");
				if (max < min) throw RegexPatternError("Quantifier range is out of order");

				bool many = true;

				if (pattern[pos] == (char)Chars::Many) {
					pos++;
				}
				else if (pattern[pos] == (char)Chars::Few) {
					pos++;
					many = false;
				}

				if (pos > len) throw RegexPatternError("Unfinished quantifier");
				if (pattern[pos] != (char)Chars::QuantifierClose) throw RegexPatternError("Unexpected '" + String(pattern[pos]) + "' in quantifier");

				Pointer<Pattern> prev = patterns.Last();
				prev->next = new QuantifierEndPattern();
				QuantifierPattern* const q = new QuantifierPattern();
				q->content = prev;
				q->many = many;
				q->min = min;
				q->max = max;
				patterns[patterns.Size() - 1] = q;
			}
			else if (c == (char)Chars::Range) {
				if (range) throw RegexPatternError("Invalid range end");

				Pointer<CharPattern> prev = patterns.Last().Cast<CharPattern>();

				if (prev != nullptr) {
					RangePattern* const r = new RangePattern();
					r->c1 = prev->c;
					patterns[patterns.Size() - 1] = r;
					range = true;
				}
				else {
					throw RegexPatternError("Invalid range start");
				}
			}
			else if (type != Type::Set && c == (char)Chars::Any) {
				patterns.Add(new AnyPattern());
			}
			else if (type != Type::Set && c == (char)Chars::Start) {
				patterns.Add(new StartPattern());
			}
			else if (type != Type::Set && c == (char)Chars::End) {
				patterns.Add(new EndPattern());
			}
			else if (c == (char)Chars::Escape) {
				escape = true;

				if (inverse > 0)
					inverse++;
				if (elementMatch > 0)
					elementMatch++;
			}
			else if (type != Type::Set && c == (char)Chars::Inverse) {
				patterns.Add(new InversePattern());
				inverse = 2;
			}
			else if (type != Type::Set && c == (char)Chars::ElementMatch) {
				patterns.Add(new GroupPattern());
				elementMatch = 2;
			}
			else if (type != Type::Set && c == (char)Chars::Skip) {
				patterns.Add(new SkipPattern());
			}
			else {
				if (type != Type::Set && (c == (char)Chars::GroupClose || c == (char)Chars::HiddenGroupClose || c == (char)Chars::SetClose)) {
					throw RegexPatternError("Unexpected '" + String(c) + "'");
				}

				if (!range) {
					CharPattern* s = new CharPattern();
					s->c = c;
					patterns.Add(s);
				}
				else {
					patterns.Last().Cast<RangePattern>()->c2 = c;
					range = false;
				}
			}

			if (inverse > 0) {
				if (--inverse == 0) {
					Pointer<InversePattern> inv = patterns.Last(1).Cast<InversePattern>();
					if (inv == nullptr) throw RegexPatternError("Unexpected '" + String(c) + "' after inverse");
					inv->content = patterns.Last();
					patterns.RemoveAt(patterns.Size() - 1);
				}
			}

			if (elementMatch > 0) {
				if (--elementMatch == 0) {
					Pointer<GroupPattern> group = patterns.Last(1).Cast<GroupPattern>();
					if (group == nullptr) throw RegexPatternError("Unexpected '" + String(c) + "' after element match");
					Pointer<GroupPattern> g = patterns.Last().Cast<GroupPattern>();

					if (g != nullptr) {
						g->elementMatch = true;
						patterns.RemoveAt(patterns.Size() - 2);
					}
					else {
						group->hidden = true;
						group->elementMatch = true;
						group->content = patterns.Last();
						group->content->next = new GroupEndPattern();
						patterns.RemoveAt(patterns.Size() - 1);
					}
				}
			}
		}

		if (range) throw RegexPatternError("Invalid range end");
		if (type != Type::None) {
			if (type == Type::Group)
				throw RegexPatternError(String("Expected '") + (char)Chars::GroupClose + "' to close group");
			else if (type == Type::HiddenGroup)
				throw RegexPatternError(String("Expected '") + (char)Chars::HiddenGroupClose + "' to close hidden group");
			else if (type == Type::Set)
				throw RegexPatternError(String("Expected '") + (char)Chars::SetClose + "' to close character set");
		}

		if (choice) {
			if (patterns.Size() == 1)
				choicePatterns.Add(patterns[0]);
			else
				choicePatterns.Add(CreatePattern(patterns, Type::HiddenGroup));

			for (Pointer<Pattern>& pattern : choicePatterns)
				pattern->next = new ChoiceEndPattern();

			ChoicePattern* const s = new ChoicePattern();
			s->patterns = choicePatterns.ToArray();

			if (type == Type::None) {
				Pattern* const p = new Pattern();
				p->next = s;
				return p;
			}

			return s;
		}

		if (patterns.Last().Cast<InversePattern>() != nullptr) throw RegexPatternError("Expected pattern after '" + String((char)Chars::Inverse) + "'");
		if (patterns.Last().Cast<ElementMatchPattern>() != nullptr) throw RegexPatternError("Expected pattern after '" + String((char)Chars::ElementMatch) +  "'");
		return CreatePattern(patterns, type);
	}
}
