#pragma once

#include "Error.h"
#include "Types.h"
#include "String.h"
#include "List.h"
#include "Set.h"
#include "Math.h"
#include "Pointer.h"
#include "Stack.h"
#include "Tuple.h"

///N Regex
namespace Boxx {
	struct Match;

	///B Regex
	/// Class for parsing strings using a regular expression pattern
	/// Regex patterns have the following syntax:

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
	///V #: Element match. The result of an element prefixed by <code>#</code> can be reffered to by using <code>%</code> followed by the number of that element starting at 0.
	///V ^: Matches the start of the search.
	///V $: Matches the end of the string.
	///V |: Matches either the expression before or after the <code>|</code> character
	///V %: Escape character. Treats the next character in the pattern as a literal charater unless it is a predefined character set
	class Regex {
	public:
		///H Constructors

		///T Empty
		Regex();

		///T From Pattern
		/// Creates a regex from a regex pattern
		///E RegexPatternError: Thrown if the pattern is invalid
		explicit Regex(const String& pattern);

		Regex(const Regex& regex);
		Regex(Regex&& regex) noexcept;
		~Regex();

		///H Methods

		///T Match
		/// Find matches in a string
		///A const String& str: The string to find matches in
		///A const UInt pos: The position in the string to start at
		///R Optional<Match>: Contains a value if a match was found
		Optional<Boxx::Match> Match(const String& str, const UInt pos = 0) const;

		///T Match Global
		/// Find all matches in a string
		///A const String& str: The string to find matches in
		///A const UInt pos: The position in the string to start at
		///R List<Match>: Contains all matches found in the string
		List<Boxx::Match> GlobalMatch(const String& str, UInt pos = 0) const;

		///H Static functions

		///T Match
		/// Find matches in a string
		///A const String& pattern: The regex pattern to use
		///A const String& str: The string to find matches in
		///A const UInt pos: The position in the string to start at
		///R Optional<Match>: Contains a value if a match was found
		static Optional<Boxx::Match> Match(const String& pattern, const String& str, const UInt pos = 0);

		///T Match Global
		/// Find all matches in a string
		///A const String& pattern: The regex pattern to use
		///A const String& str: The string to find matches in
		///A const UInt pos: The position in the string to start at
		///R Optional<Match>: Contains a value if a match was found
		static List<Boxx::Match> GlobalMatch(const String& pattern, const String& str, UInt pos = 0);

		///T Escape
		/// Escapes meta characters in a string to make the regex engine interpret the string literally
		static String Escape(const String& str);

		void operator=(const Regex& regex); 
		void operator=(Regex&& regex) noexcept;

	private:

		struct Pattern {
			String pattern;

			char operator[](const UInt i) const;
		};

		struct RegexNode;
		struct MatchInfo;

		typedef Pointer<RegexNode> Node;
		typedef Tuple<Node, Node> NodeLeaf;

		static Node ParsePattern(const Pattern& pattern, UInt& index);
		static NodeLeaf ParseExpression(const Pattern& pattern, UInt& index);
		static NodeLeaf ParseRawExpression(const Pattern& pattern, UInt& index);
		static NodeLeaf ParseElement(const Pattern& pattern, UInt& index);
		static NodeLeaf ParseElementMatch(const Pattern& pattern, UInt& index);
		static NodeLeaf ParseRawElement(const Pattern& pattern, UInt& index);
		static Optional<Tuple<UInt, UInt, bool>> ParseQuantifier(const Pattern& pattern, UInt& index);
		static Node ParseString(const Pattern& pattern, UInt& index);
		static Node ParseRange(const Pattern& pattern, UInt& index);
		static Node ParseEscape(const Pattern& pattern, UInt& index);
		static Node ParseSet(const Pattern& pattern, UInt& index);
		static Node ParseSetRange(const Pattern& pattern, UInt& index);
		static Node ParseSetEscape(const Pattern& pattern, UInt& index);
		static NodeLeaf ParseGroup(const Pattern& pattern, UInt& index);

		static Optional<char> ParseChar(const Pattern& pattern, UInt& index, const bool skipPost = true);
		static Optional<char> ParseSetChar(const Pattern& pattern, UInt& index);
		static Optional<UInt> ParseInt(const Pattern& pattern, UInt& index);

		struct MatchState {
			Node node, next;
			const char* start = nullptr;
			const char* end   = nullptr;
			UInt current = 0;
			UInt groupSize = 0;
			bool backtrack = false;
		};

		struct RegexNode {
			Node next;

			virtual const char* Match(const char* str, MatchInfo& info) = 0;

			virtual bool IsPlain() {
				return next == nullptr || next->IsPlain();
			}
		};

		struct RootNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct LeafNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;

			virtual bool IsPlain() override {
				return true;
			}
		};

		struct StringNode : public RegexNode {
			String string;

			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct CharNode : public RegexNode {
			char c;

			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct RangeNode : public RegexNode {
			char start{}, end{};

			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct HexNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct AlphaNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct AlphaNumNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct WordNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct PunctNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct SpaceNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct WhiteSpaceNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct BoundaryNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct AnyNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct StartNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct EndNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct InverseNode : public RegexNode {
			Node content;

			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct SetNode : public RegexNode {
			Set<char> chars;
			List<Node> nodes;

			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct EmptyNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct GroupNode : public RegexNode {
			bool isHidden = true;

			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct GroupEndNode : public RegexNode {
			bool isHidden = true;

			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct SelectNode : public RegexNode {
			List<Node> nodes;

			virtual const char* Match(const char* str, MatchInfo& info) override;

			virtual bool IsPlain() override {
				return false;
			}
		};

		struct QuantifierNode : public RegexNode {
			UInt min = 0;
			UInt max = 0;
			bool many = true;

			Node content;

			virtual const char* Match(const char* str, MatchInfo& info) override;

			virtual bool IsPlain() override {
				return false;
			}
		};

		struct QuantifierEndNode : public RegexNode {
			UInt min = 0;
			UInt max = 0;
			bool many = true;

			Node content;

			virtual const char* Match(const char* str, MatchInfo& info) override;

			virtual bool IsPlain() override {
				return false;
			}
		};

		struct PlainQuantifierNode : public RegexNode {
			UInt min = 0;
			UInt max = 0;
			bool many = true;

			Node content;

			virtual const char* Match(const char* str, MatchInfo& info) override;

			virtual bool IsPlain() override {
				return false;
			}
		};

		struct AnyQuantifierNode : public RegexNode {
			UInt min = 0;
			UInt max = 0;
			bool many = true;

			virtual const char* Match(const char* str, MatchInfo& info) override;

			virtual bool IsPlain() override {
				return false;
			}
		};

		struct ElementNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct ElementEndNode : public RegexNode {
			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct ElementMatchNode : public RegexNode {
			UInt index = 0;

			virtual const char* Match(const char* str, MatchInfo& info) override;
		};

		struct MatchInfo {
			const char* str = nullptr;
			const char* start = nullptr;
			const char* end = nullptr;

			UInt matchStart{}, matchEnd{};

			List<String> groups;
			Stack<const char*> groupStack;

			Stack<UInt> quantNums;
			Stack<bool> quantBacktrack;

			List<String> elements;
			Stack<const char*> elementStack;
		};

		Node root;

		// Meta Characters
		struct MetaChar {
			static const char range   = ':';
			static const char escape  = '%';
			static const char any     = '.';
			static const char start   = '^';
			static const char end     = '$';
			static const char inverse = '~';
			static const char select  = '|';
			static const char element = '#';

			static const char manyOpt    = '*';
			static const char many       = '+';
			static const char fewOpt     = '/';
			static const char few        = '-';
			static const char optional   = '?';
			static const char quantOpen  = '<';
			static const char quantClose = '>';
			static const char quantRange = ':';

			static const char setOpen     = '[';
			static const char setClose    = ']';
			static const char groupOpen   = '(';
			static const char groupClose  = ')';
			static const char hiddenOpen  = '{';
			static const char hiddenClose = '}';

			static const char lower    = 'l';
			static const char upper    = 'u';
			static const char digit    = 'd';
			static const char hex      = 'x';
			static const char alpha    = 'a';
			static const char alphanum = 'v';
			static const char word     = 'w';
			static const char punct    = 'p';
			static const char space    = 's';
			static const char white    = 'n';
			static const char bound    = 'b';

			static bool IsMetaChar(const char c) {
				static const Set<char> metaChars = GetMetaChars();
				return metaChars.Contains(c);
			}

			static bool IsSetMetaChar(const char c) {
				static const Set<char> metaChars = GetSetMetaChars();
				return metaChars.Contains(c);
			}

			static bool IsPostChar(const char c) {
				static const Set<char> chars = GetPostChars();
				return chars.Contains(c);
			}

			static bool IsReservedEscape(const char c) {
				static const Set<char> reservedChars = GetReservedEscapeChars();
				return reservedChars.Contains(c);
			}

			static bool IsReservedSetEscape(const char c) {
				static const Set<char> reservedChars = GetReservedSetEscapeChars();
				return reservedChars.Contains(c);
			}

			static bool IsHex(const char c) {
				return 
					'0' <= c && c <= '9' ||
					'a' <= c && c <= 'f' ||
					'A' <= c && c <= 'F';
			}

			static bool IsAlpha(const char c) {
				return 
					'a' <= c && c <= 'z' ||
					'A' <= c && c <= 'Z';
			}

			static bool IsAlphaNum(const char c) {
				return 
					'0' <= c && c <= '9' ||
					'a' <= c && c <= 'z' ||
					'A' <= c && c <= 'Z';
			}

			static bool IsWord(const char c) {
				return 
					'0' <= c && c <= '9' ||
					'a' <= c && c <= 'z' ||
					'A' <= c && c <= 'Z' ||
					c == '_';
			}

			static bool IsSpace(const char c) {
				static const Set<char> space = GetSpace();
				return space.Contains(c);
			}

			static bool IsWhiteSpace(const char c) {
				static const Set<char> whiteSpace = GetWhiteSpace();
				return whiteSpace.Contains(c);
			}

			static bool IsPunct(const char c) {
				static const Set<char> punct = GetPunct();
				return punct.Contains(c);
			}

		private:
			static Set<char> GetMetaChars() {
				Set<char> chars;
				chars.Add(range);
				chars.Add(escape);
				chars.Add(any);
				chars.Add(start);
				chars.Add(end);
				chars.Add(inverse);
				chars.Add(select);
				chars.Add(element);

				chars.Add(manyOpt);
				chars.Add(many);
				chars.Add(fewOpt);
				chars.Add(few);
				chars.Add(optional);
				chars.Add(quantOpen);
				chars.Add(quantClose);
				chars.Add(quantRange);

				chars.Add(setOpen);
				chars.Add(setClose);
				chars.Add(groupOpen);
				chars.Add(groupClose);
				chars.Add(hiddenOpen);
				chars.Add(hiddenClose);
				return chars;
			}

			static Set<char> GetSetMetaChars() {
				Set<char> chars;
				chars.Add(range);
				chars.Add(escape);

				chars.Add(setClose);
				return chars;
			}

			static Set<char> GetPostChars() {
				Set<char> chars;
				chars.Add(range);
				chars.Add(quantOpen);
				chars.Add(many);
				chars.Add(manyOpt);
				chars.Add(few);
				chars.Add(fewOpt);
				chars.Add(optional);
				return chars;
			}

			static Set<char> GetReservedEscapeChars() {
				Set<char> chars;
				chars.Add(lower);
				chars.Add(upper);
				chars.Add(digit);
				chars.Add(hex);
				chars.Add(alpha);
				chars.Add(alphanum);
				chars.Add(word);
				chars.Add(punct);
				chars.Add(space);
				chars.Add(white);
				chars.Add(bound);

				chars.Add('0');
				chars.Add('1');
				chars.Add('2');
				chars.Add('3');
				chars.Add('4');
				chars.Add('5');
				chars.Add('6');
				chars.Add('7');
				chars.Add('8');
				chars.Add('9');
				return chars;
			}

			static Set<char> GetReservedSetEscapeChars() {
				Set<char> chars;
				chars.Add(lower);
				chars.Add(upper);
				chars.Add(digit);
				chars.Add(hex);
				chars.Add(alpha);
				chars.Add(alphanum);
				chars.Add(word);
				chars.Add(punct);
				chars.Add(space);
				chars.Add(white);
				return chars;
			}

			static Set<char> GetSpace() {
				Set<char> chars;
				chars.Add(' ');
				chars.Add('\t');
				return chars;
			}

			static Set<char> GetWhiteSpace() {
				Set<char> chars;
				chars.Add(' ');
				chars.Add('\t');
				chars.Add('\r');
				chars.Add('\n');
				chars.Add('\v');
				return chars;
			}

			static Set<char> GetPunct() {
				const String p = "[!\"#$%&'()*+,./:;<=>?@\\^`{|}~-]";
				Set<char> set = Set<char>(p.Size());

				for (const char c : p)
					set.Add(c);

				return set;
			}
		};
	};

	///B Match
	/// Contains information about a regex match
	struct Match {
		///T Index
		/// The index of the string where the pattern was found
		UInt index = 0;

		///T Length
		/// The length of the matched substring
		UInt length = 0;

		///T Match
		/// Contains the entire matched string
		String match;

		///T Groups
		/// A list of all matched groups
		List<String> groups;
	};

	///B RegexPatternError
	/// Thrown if a regex pattern is invalid
	class RegexPatternError : public Error {
	public:
		RegexPatternError() : Error() {}
		RegexPatternError(const char* const msg) : Error(msg) {}

		virtual String Name() const override {
			return "RegexPatternError";
		}
	};

	inline char Regex::Pattern::operator[](const UInt i) const {
		if (i >= pattern.Size()) throw RegexPatternError("Unexpected end of pattern");
		return pattern[i];
	}
	
	inline Regex::Regex() {
	
	}

	inline Regex::Regex(const String& pattern) {
		try {
			UInt i = 0;
			Pattern p;
			p.pattern = pattern;
			root = ParsePattern(p, i);
		}
		catch (RegexPatternError e) {
			throw e;
		}
	}

	inline Regex::Regex(const Regex& regex) {
		root = regex.root;
	}

	inline Regex::Regex(Regex&& regex) noexcept {
		root = std::move(regex.root);
	}

	inline Regex::~Regex() {

	}

	inline Optional<Match> Regex::Match(const String& str, const UInt pos) const {
		MatchInfo info;
		info.str   = (const char*)str;
		info.start = (const char*)str + pos;
		info.end   = (const char*)str + str.Size();
		info.matchStart = pos;

		if (root->Match(info.start, info)) {
			Boxx::Match match;
			match.index   = info.matchStart;
			match.length  = info.matchEnd - info.matchStart;
			match.groups  = info.groups;

			if (match.length > 0) {
				match.match = str.Sub(info.matchStart, info.matchEnd - 1);
			}
			else {
				match.match = "";
			}

			return match;
		}
		else {
			return nullptr;
		}
	}

	inline List<Match> Regex::GlobalMatch(const String& str, UInt pos) const {
		List<Boxx::Match> matches;

		while (Optional<Boxx::Match> match = Match(str, pos)) {
			matches.Add(*match);
			pos = match->index + match->length;
		}

		return matches;
	}

	inline Optional<Match> Regex::Match(const String& pattern, const String& str, const UInt pos) {
		try {
			return Regex(pattern).Match(str, pos);
		}
		catch (RegexPatternError e) {
			throw e;
		}
	}

	inline List<Match> Regex::GlobalMatch(const String& pattern, const String& str, const UInt pos) {
		try {
			return Regex(pattern).GlobalMatch(str, pos);
		}
		catch (RegexPatternError e) {
			throw e;
		}
	}

	inline String Regex::Escape(const String& str) {
		List<char> chars{str.Size()};

		for (const char c : str) {
			if (MetaChar::IsMetaChar(c)) {
				chars.Add(MetaChar::escape);
				chars.Add(c);
			}
			else {
				chars.Add(c);
			}
		}

		return String(chars);
	}

	inline void Regex::operator=(const Regex& regex) {
		root = regex.root;
	}

	inline void Regex::operator=(Regex&& regex) noexcept {
		root = std::move(regex.root);
	}

	inline Regex::Node Regex::ParsePattern(const Pattern& pattern, UInt& index) {
		Pointer<RootNode> root = new RootNode();
		root->next = ParseExpression(pattern, index).value1;

		if (index < pattern.pattern.Size()) throw RegexPatternError("Unexpected character '" + String(pattern[index]) + "'"); 

		if (!root->next) root->next = new LeafNode();

		return root;
	}

	inline Regex::NodeLeaf Regex::ParseExpression(const Pattern& pattern, UInt& index) {
		List<NodeLeaf> nodes;

		NodeLeaf node = ParseRawExpression(pattern, index);

		while (node.value1) {
			nodes.Add(node);

			if (index >= pattern.pattern.Size() || pattern[index] != MetaChar::select) break;

			index++;
			node = ParseRawExpression(pattern, index);
		}

		if (nodes.Size() == 1) return nodes[0];

		Pointer<EmptyNode> empty = new EmptyNode();
		empty->next = new LeafNode();
		Pointer<SelectNode> select = new SelectNode();

		for (NodeLeaf& exp : nodes) {
			exp.value2->next = empty;
			select->nodes.Add(exp.value1);
		}

		return NodeLeaf(select, empty);
	}

	inline Regex::NodeLeaf Regex::ParseRawExpression(const Pattern& pattern, UInt& index) {
		Node root = nullptr;
		Node element = nullptr;

		NodeLeaf node = ParseElement(pattern, index);

		while (node.value1) {
			if (element) element->next = node.value1;
			element = node.value2;

			if (!root) root = node.value1;

			node = ParseElement(pattern, index);
		}

		if (element) {
			element->next = new LeafNode();
			return NodeLeaf(root, element);
		}
		else {
			Pointer<EmptyNode> empty = new EmptyNode();
			return NodeLeaf(empty, empty);
		}
	}

	inline Regex::NodeLeaf Regex::ParseElement(const Pattern& pattern, UInt& index) {
		NodeLeaf element = ParseRawElement(pattern, index);

		if (Optional<Tuple<UInt, UInt, bool>> quant = ParseQuantifier(pattern, index)) {
			if (!element.value1->IsPlain()) {
				Pointer<QuantifierNode> quantifier = new QuantifierNode();
				quantifier->min = quant->value1;
				quantifier->max = quant->value2;
				quantifier->many = quant->value3;

				Pointer<QuantifierEndNode> qEnd = new QuantifierEndNode();
				qEnd->min  = quantifier->min;
				qEnd->max  = quantifier->max;
				qEnd->many = quantifier->many;

				Pointer<EmptyNode> empty = new EmptyNode();

				quantifier->next    = empty;
				quantifier->content = element.value1;

				qEnd->next    = empty;
				qEnd->content = element.value1;

				element.value2->next = qEnd;
				element.value2 = empty;
				element.value1 = quantifier;
			}
			else if (element.value1.Is<AnyNode>()) {
				Pointer<AnyQuantifierNode> quantifier = new AnyQuantifierNode();
				quantifier->min = quant->value1;
				quantifier->max = quant->value2;
				quantifier->many = quant->value3;

				element.value1 = quantifier;
				element.value2 = quantifier;
			}
			else {
				Pointer<PlainQuantifierNode> quantifier = new PlainQuantifierNode();
				quantifier->min = quant->value1;
				quantifier->max = quant->value2;
				quantifier->many = quant->value3;

				quantifier->content = element.value1;
				element.value2->next = new LeafNode();

				element.value1 = quantifier;
				element.value2 = quantifier;
			}
		}

		return element;
	}

	inline Regex::NodeLeaf Regex::ParseElementMatch(const Pattern& pattern, UInt& index) {
		if (index >= pattern.pattern.Size() || pattern[index] != MetaChar::element) {
			return NodeLeaf(nullptr, nullptr);
		}

		index++;

		NodeLeaf node = ParseRawElement(pattern, index);

		if (node.value1 == nullptr) {
			throw RegexPatternError("Element expected after '" + String(MetaChar::element) + "'");
		}

		Pointer<ElementNode> element = new ElementNode();
		element->next = node.value1;

		Pointer<ElementEndNode> elementEnd = new ElementEndNode();
		node.value2->next = elementEnd;

		return NodeLeaf(element, elementEnd);
	}

	inline Regex::NodeLeaf Regex::ParseRawElement(const Pattern& pattern, UInt& index) {
		if (Node range = ParseRange(pattern, index)) {
			return NodeLeaf(range, range);
		}
		else if (Node str = ParseString(pattern, index)) {
			return NodeLeaf(str, str);
		}
		else if (Optional<char> c = ParseChar(pattern, index)) {
			Pointer<StringNode> node = new StringNode();
			node->string = String(*c);
			return NodeLeaf(node, node);
		}

		if (index >= pattern.pattern.Size()) {
			return NodeLeaf(nullptr, nullptr);
		}

		switch (pattern[index]) {
			case MetaChar::escape: {
				Node esc = ParseEscape(pattern, index);
				return NodeLeaf(esc, esc);
			}

			case MetaChar::any: {
				index++;
				Node node = new AnyNode();
				return NodeLeaf(node, node);
			}

			case MetaChar::start: {
				index++;
				Node node = new StartNode();
				return NodeLeaf(node, node);
			}

			case MetaChar::end: {
				index++;
				Node node = new EndNode();
				return NodeLeaf(node, node);
			}

			case MetaChar::inverse: {
				index++;

				NodeLeaf node = ParseRawElement(pattern, index);

				if (node.value1) {
					Pointer<InverseNode> inv = new InverseNode();
					node.value2->next = new LeafNode();
					inv->content = node.value1;
					return NodeLeaf(inv, inv);
				}
				else {
					throw RegexPatternError("Element expected after '" + String(MetaChar::inverse) + "'");
				}
			}

			case MetaChar::setOpen: {
				Node set = ParseSet(pattern, index);
				return NodeLeaf(set, set);
			}

			case MetaChar::groupOpen: 
			case MetaChar::hiddenOpen: {
				return ParseGroup(pattern, index);
			}

			case MetaChar::element: {
				return ParseElementMatch(pattern, index);
			}
		}

		return NodeLeaf(nullptr, nullptr);
	}

	inline Optional<Tuple<UInt, UInt, bool>> Regex::ParseQuantifier(const Pattern& pattern, UInt& index) {
		if (index >= pattern.pattern.Size()) return nullptr;

		UInt min = 0, max = 0;
		bool many = true;
		
		if (pattern[index] == MetaChar::manyOpt) {
			min = 0;
			max = Math::UIntMax();
			many = true;
			index++;
		}
		else if (pattern[index] == MetaChar::many) {
			min = 1;
			max = Math::UIntMax();
			many = true;
			index++;
		}
		else if (pattern[index] == MetaChar::fewOpt) {
			min = 0;
			max = Math::UIntMax();
			many = false;
			index++;
		}
		else if (pattern[index] == MetaChar::few) {
			min = 1;
			max = Math::UIntMax();
			many = false;
			index++;
		}
		else if (pattern[index] == MetaChar::optional) {
			min = 0;
			max = 1;
			many = true;
			index++;
		}
		else if (pattern[index] == MetaChar::quantOpen) {
			index++;
			min = 0;
			max = Math::UIntMax();

			if (index >= pattern.pattern.Size()) {
				throw RegexPatternError("Quantifier range expected after '" + String(MetaChar::quantOpen) + "'");
			}

			if (pattern[index] == MetaChar::quantRange) {
				index++;

				if (Optional<UInt> num = ParseInt(pattern, index)) {
					max = *num;
				}
				else {
					throw RegexPatternError("Integer expected after '" + String(MetaChar::quantRange) + "' in quantifier range");
				}
			}
			else if (Optional<UInt> num = ParseInt(pattern, index)) {
				min = *num;

				if (index < pattern.pattern.Size() && pattern[index] == MetaChar::quantRange) {
					index++;

					if (Optional<UInt> n = ParseInt(pattern, index)) {
						max = *n;
					}
					else {
						max = Math::UIntMax();
					}
				}
				else {
					max = min;
				}
			}

			if (min > max) {
				throw RegexPatternError("Quantifier range is out of order");
			}

			if (index >= pattern.pattern.Size()) {
				throw RegexPatternError("'" + String(MetaChar::quantClose) + "' expected to close quantifier");
			}

			if (pattern[index] == MetaChar::few || pattern[index] == MetaChar::many) {
				many = pattern[index] == MetaChar::many;
				index++;
			}

			if (index >= pattern.pattern.Size() || pattern[index] != MetaChar::quantClose) {
				throw RegexPatternError("'" + String(MetaChar::quantClose) + "' expected to close quantifier");
			}

			index++;
		}
		else {
			return nullptr;
		}

		return Tuple<UInt, UInt, bool>(min, max, many);
	}

	inline Regex::Node Regex::ParseRange(const Pattern& pattern, UInt& index) {
		const UInt startIndex = index;

		Optional<char> start = ParseChar(pattern, index);
		if (!start) return nullptr;

		if (index >= pattern.pattern.Size()) {
			index = startIndex;
			return nullptr;
		}

		if (pattern[index] != MetaChar::range) {
			index = startIndex;
			return nullptr;
		}

		index++;

		if (Optional<char> endChar = ParseChar(pattern, index)) {
			Pointer<RangeNode> range = new RangeNode();
			range->start = *start;
			range->end   = *endChar;
			return range;
		}
		else {
			throw RegexPatternError("Character expected after '" + String(MetaChar::range) + "' to end range");
		}
	}

	inline Regex::Node Regex::ParseString(const Pattern& pattern, UInt& index) {
		List<char> str;
		UInt lastIndex = index;

		while (Optional<char> c = ParseChar(pattern, index, false)) {
			str.Add(*c);
			lastIndex = index;
		}

		if (str.IsEmpty()) return nullptr;

		if (str.Size() > 1) {
			Pointer<StringNode> node = new StringNode();
			node->string = String(str);
			return node;
		}
		else {
			Pointer<CharNode> node = new CharNode();
			node->c = str[0];
			return node;
		}
	}

	inline Regex::Node Regex::ParseEscape(const Pattern& pattern, UInt& index) {
		if (pattern[index] != MetaChar::escape) return nullptr;
		index++;

		switch (pattern[index]) {
			case MetaChar::digit: {
				Pointer<RangeNode> range = new RangeNode();
				range->start = '0';
				range->end   = '9';
				index++;

				return range;
			}

			case MetaChar::lower: {
				Pointer<RangeNode> range = new RangeNode();
				range->start = 'a';
				range->end   = 'z';
				index++;

				return range;
			}

			case MetaChar::upper: {
				Pointer<RangeNode> range = new RangeNode();
				range->start = 'A';
				range->end   = 'Z';
				index++;

				return range;
			}

			case MetaChar::hex: {
				index++;
				return new HexNode();
			}

			case MetaChar::alpha: {
				index++;
				return new AlphaNode();
			}

			case MetaChar::alphanum: {
				index++;
				return new AlphaNumNode();
			}

			case MetaChar::word: {
				index++;
				return new WordNode();
			}

			case MetaChar::punct: {
				index++;
				return new PunctNode();
			}

			case MetaChar::space: {
				index++;
				return new SpaceNode();
			}

			case MetaChar::white: {
				index++;
				return new WhiteSpaceNode();
			}

			case MetaChar::bound: {
				index++;
				return new BoundaryNode();
			}
		}

		if (Optional<UInt> num = ParseInt(pattern, index)) {
			Pointer<ElementMatchNode> match = new ElementMatchNode();
			match->index = *num;
			return match;
		}

		throw RegexPatternError("Invalid escape character '" + String(pattern[index]) + "'");
	}

	inline Regex::Node Regex::ParseSet(const Pattern& pattern, UInt& index) {
		if (pattern[index] != MetaChar::setOpen) return nullptr;
		index++;

		Pointer<SetNode> set = new SetNode();

		while (pattern[index] != MetaChar::setClose) {
			if (Node node = ParseSetRange(pattern, index)) {
				node->next = new LeafNode();
				set->nodes.Add(node);
			}
			else if (Optional<char> c = ParseSetChar(pattern, index)) {
				set->chars.Add(*c);
			}
			else if (Node node = ParseSetEscape(pattern, index)) {
				node->next = new LeafNode();
				set->nodes.Add(node);
			}
			else {
				throw RegexPatternError("Unexpected character '" + String(pattern[index]) + "' in character set");
			}
		}

		index++;
		return set;
	}

	inline Regex::Node Regex::ParseSetRange(const Pattern& pattern, UInt& index) {
		const UInt startIndex = index;

		Optional<char> start = ParseSetChar(pattern, index);
		if (!start) return nullptr;

		if (pattern[index] != MetaChar::range) {
			index = startIndex;
			return nullptr;
		}

		index++;

		if (Optional<char> endChar = ParseSetChar(pattern, index)) {
			Pointer<RangeNode> range = new RangeNode();
			range->start = *start;
			range->end   = *endChar;
			return range;
		}
		else {
			throw RegexPatternError("Character expected after '" + String(MetaChar::range) + "' to end range");
		}
	}

	inline Regex::Node Regex::ParseSetEscape(const Pattern& pattern, UInt& index) {
		if (pattern[index] != MetaChar::escape) return nullptr;
		index++;

		switch (pattern[index]) {
			case MetaChar::digit: {
				Pointer<RangeNode> range = new RangeNode();
				range->start = '0';
				range->end   = '9';
				index++;

				return range;
			}

			case MetaChar::lower: {
				Pointer<RangeNode> range = new RangeNode();
				range->start = 'a';
				range->end   = 'z';
				index++;

				return range;
			}

			case MetaChar::upper: {
				Pointer<RangeNode> range = new RangeNode();
				range->start = 'A';
				range->end   = 'Z';
				index++;

				return range;
			}

			case MetaChar::hex: {
				index++;
				return new HexNode();
			}

			case MetaChar::alpha: {
				index++;
				return new AlphaNode();
			}

			case MetaChar::alphanum: {
				index++;
				return new AlphaNumNode();
			}

			case MetaChar::word: {
				index++;
				return new WordNode();
			}

			case MetaChar::punct: {
				index++;
				return new PunctNode();
			}

			case MetaChar::space: {
				index++;
				return new SpaceNode();
			}

			case MetaChar::white: {
				index++;
				return new WhiteSpaceNode();
			}
		}

		throw RegexPatternError("Invalid escape character '" + String(pattern[index]) + "'");
	}

	inline Regex::NodeLeaf Regex::ParseGroup(const Pattern& pattern, UInt& index) {
		if (pattern[index] != MetaChar::groupOpen && pattern[index] != MetaChar::hiddenOpen) {
			return NodeLeaf(nullptr, nullptr);
		}

		bool isHidden = pattern[index] == MetaChar::hiddenOpen;

		Pointer<GroupNode> group = new GroupNode();
		group->isHidden = isHidden;
		index++;

		NodeLeaf node = ParseExpression(pattern, index);
		group->next = node.value1;
		
		Pointer<GroupEndNode> groupEnd = new GroupEndNode();
		groupEnd->isHidden = isHidden;
		node.value2->next = groupEnd;

		if (isHidden && pattern[index] != MetaChar::hiddenClose) {
			throw RegexPatternError("'" + String(MetaChar::hiddenClose) + "' expected to close hidden group");
		}

		if (!isHidden && pattern[index] != MetaChar::groupClose) {
			throw RegexPatternError("'" + String(MetaChar::groupClose) + "' expected to close group");
		}

		index++;
		return NodeLeaf(group, groupEnd);
	}

	inline Optional<char> Regex::ParseChar(const Pattern& pattern, UInt& index, const bool skipPost) {
		if (index >= pattern.pattern.Size()) return nullptr;

		char c = pattern[index];

		if (c == MetaChar::escape) {
			if (index + 1 >= pattern.pattern.Size()) return nullptr;
			c = pattern[index + 1];
			if (MetaChar::IsReservedEscape(c)) return nullptr;
			index++;
		}
		else if (MetaChar::IsMetaChar(c)) {
			return nullptr;
		}
		else if (!skipPost && (index + 1 < pattern.pattern.Size() && MetaChar::IsPostChar(pattern[index + 1]))) {
			return nullptr;
		}

		index++;
		return c;
	}

	inline Optional<char> Regex::ParseSetChar(const Pattern& pattern, UInt& index) {
		if (index >= pattern.pattern.Size()) return nullptr;

		char c = pattern[index];

		if (c == MetaChar::escape) {
			if (index + 1 >= pattern.pattern.Size()) return nullptr;
			c = pattern[index + 1];
			if (MetaChar::IsReservedSetEscape(c)) return nullptr;
			index++;
		}
		else if (MetaChar::IsSetMetaChar(c)) {
			return nullptr;
		}

		index++;
		return c;
	}

	inline Optional<UInt> Regex::ParseInt(const Pattern& pattern, UInt& index) {
		if (index >= pattern.pattern.Size()) return nullptr;
		
		UInt startIndex = index;

		while ('0' <= pattern[index] && pattern[index] <= '9') {
			index++;
			if (index >= pattern.pattern.Size()) break;
		}

		if (startIndex == index) return nullptr;

		return pattern.pattern.Sub(startIndex, index - 1).ToUInt();
	}

	inline const char* Regex::RootNode::Match(const char* str, MatchInfo& info) {
		bool isStart = next.Is<StartNode>();

		do {
			if (const char* c = next->Match(str, info)) {
				return c;
			}

			str++;
			info.matchStart++;

			if (str >= info.end) return nullptr;
		}
		while (!isStart);

		return nullptr;
	}

	inline const char* Regex::LeafNode::Match(const char* str, MatchInfo& info) {
		info.matchEnd = (UInt)(str - info.str);
		return str;
	}

	inline const char* Regex::StringNode::Match(const char* str, MatchInfo& info) {
		if ((UInt)(info.end - str) < string.Size()) {
			return nullptr;
		}

		const char* c = str;

		for (UInt i = 0; i < string.Size(); i++, c++) {
			if (*c != string[i]) return nullptr;
		}

		return next->Match(c, info);
	}

	inline const char* Regex::CharNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end || *str != c) {
			return nullptr;
		}

		return next->Match(str + 1, info);
	}

	inline const char* Regex::RangeNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end) {
			return nullptr;
		}

		if (start <= *str && *str <= end) {
			return next->Match(str + 1, info);
		}

		return nullptr;
	}

	inline const char* Regex::HexNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end) {
			return nullptr;
		}

		if (MetaChar::IsHex(*str)) {
			return next->Match(str + 1, info);
		}

		return nullptr;
	}

	inline const char* Regex::AlphaNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end) {
			return nullptr;
		}

		if (MetaChar::IsAlpha(*str)) {
			return next->Match(str + 1, info);
		}

		return nullptr;
	}

	inline const char* Regex::AlphaNumNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end) {
			return nullptr;
		}

		if (MetaChar::IsAlphaNum(*str)) {
			return next->Match(str + 1, info);
		}

		return nullptr;
	}

	inline const char* Regex::WordNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end) {
			return nullptr;
		}

		if (MetaChar::IsWord(*str)) {
			return next->Match(str + 1, info);
		}

		return nullptr;
	}

	inline const char* Regex::PunctNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end) {
			return nullptr;
		}

		if (MetaChar::IsPunct(*str)) {
			return next->Match(str + 1, info);
		}

		return nullptr;
	}

	inline const char* Regex::SpaceNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end) {
			return nullptr;
		}

		if (MetaChar::IsSpace(*str)) {
			return next->Match(str + 1, info);
		}

		return nullptr;
	}

	inline const char* Regex::WhiteSpaceNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end) {
			return nullptr;
		}

		if (MetaChar::IsWhiteSpace(*str)) {
			return next->Match(str + 1, info);
		}

		return nullptr;
	}

	inline const char* Regex::BoundaryNode::Match(const char* str, MatchInfo& info) {
		if (
			(str == info.start || str == info.end) ||
			(MetaChar::IsWord(*(str - 1)) && !MetaChar::IsWord(*str)) ||
			(!MetaChar::IsWord(*(str - 1)) && MetaChar::IsWord(*str))
		) {
			return next->Match(str, info);
		}
		else {
			return nullptr;
		}
	}

	inline const char* Regex::AnyNode::Match(const char* str, MatchInfo& info) {
		if (str >= info.end) {
			return nullptr;
		}

		return next->Match(str + 1, info);
	}

	inline const char* Regex::StartNode::Match(const char* str, MatchInfo& info) {
		if (str != info.start) {
			return nullptr;
		}

		return next->Match(str, info);
	}

	inline const char* Regex::EndNode::Match(const char* str, MatchInfo& info) {
		if (str != info.end) {
			return nullptr;
		}

		return next->Match(str, info);
	}

	inline const char* Regex::InverseNode::Match(const char* str, MatchInfo& info) {
		const UInt groupSize   = info.groups.Size();
		const UInt elementSize = info.elements.Size();

		if (content->Match(str, info)) {
			if (groupSize < info.groups.Size()) {
				info.groups.RemoveAt(groupSize, info.groups.Size() - groupSize);
			}

			if (elementSize < info.elements.Size()) {
				info.elements.RemoveAt(elementSize, info.elements.Size() - elementSize);
			}

			return nullptr;
		}

		return next->Match(str + 1, info);
	}

	inline const char* Regex::EmptyNode::Match(const char* str, MatchInfo& info) {
		return next->Match(str, info);
	}

	inline const char* Regex::SetNode::Match(const char* str, MatchInfo& info) {
		if (chars.IsEmpty() && nodes.IsEmpty()) {
			return next->Match(str, info);
		}

		if (str >= info.end) return nullptr;

		bool found = chars.Contains(*str);

		if (!found) {
			for (Node node : nodes) {
				if (node->Match(str, info)) {
					found = true;
					break;
				}
			}
		}

		if (!found) return nullptr;

		return next->Match(str + 1, info);
	}

	inline const char* Regex::GroupNode::Match(const char* str, MatchInfo& info) {
		UInt current = 0;

		if (!isHidden) {
			current = info.groups.Size();
			info.groups.Add("");
		}

		if (const char* c = next->Match(str, info)) {
			if (!isHidden) {
				info.groups[current] = String(str, (UInt)(info.groupStack.Pop() - str));
			}

			return c;
		}
		else {
			if (!isHidden) info.groups.RemoveLast();
			return nullptr;
		}
	}

	inline const char* Regex::GroupEndNode::Match(const char* str, MatchInfo& info) {
		if (const char* c = next->Match(str, info)) {
			if (!isHidden) {
				info.groupStack.Push(str);
			}

			return c;
		}
		
		return nullptr;
	}

	inline const char* Regex::SelectNode::Match(const char* str, MatchInfo& info) {
		for (Node node : nodes) {
			if (const char* c = node->Match(str, info)) {
				return c;
			}
		}
		
		return nullptr;
	}

	inline const char* Regex::QuantifierNode::Match(const char* str, MatchInfo& info) {
		info.quantNums.Push(0);
		const char* c = nullptr;

		if (many) {
			if (min > 0) {
				c = content->Match(str, info);
			}
			else {
				c = content->Match(str, info);

				if (c == nullptr) {
					c = next->Match(str, info);
				}
			}
		}
		else {
			if (min > 0) {
				c = content->Match(str, info);
			}
			else {
				c = next->Match(str, info);

				if (c == nullptr) {
					c = content->Match(str, info);
				}
			}
		}

		info.quantNums.Pop();
		return c;
	}

	inline const char* Regex::QuantifierEndNode::Match(const char* str, MatchInfo& info) {
		info.quantNums.Set(info.quantNums.Peek() + 1);
		const char* c = nullptr;

		if (many) {
			if (info.quantNums.Peek() < min) {
				c = content->Match(str, info);
			}
			else if (info.quantNums.Peek() < max) {
				c = content->Match(str, info);

				if (c == nullptr) {
					c = next->Match(str, info);
				}
			}
			else {
				c = next->Match(str, info);
			}
		}
		else {
			if (info.quantNums.Peek() < min) {
				c = content->Match(str, info);
			}
			else if (info.quantNums.Peek() < max) {
				c = next->Match(str, info);

				if (c == nullptr) {
					c = content->Match(str, info);
				}
			}
			else {
				c = next->Match(str, info);
			}
		}

		info.quantNums.Set(info.quantNums.Peek() - 1);
		return c;
	}

	inline const char* Regex::PlainQuantifierNode::Match(const char* str, MatchInfo& info) {
		if (many) {
			Stack<const char*> starts;
			starts.Push(str);

			for (UInt num = 0; num < max && str <= info.end; num++) {
				if (const char* c = content->Match(str, info)) {
					str = c;
					starts.Push(str);
				}
				else if (num < min) {
					return nullptr;
				}
				else {
					break;
				}
			}

			while (starts.Size() > min) {
				if (str = next->Match(starts.Pop(), info)) {
					return str;
				}
			}

			return nullptr;
		}
		else {
			UInt num = 0;

			for (; num < min; num++) {
				str = content->Match(str, info);

				if (str == nullptr) {
					return nullptr;
				}
			}

			for (; num <= max && str <= info.end; num++) {
				if (const char* c = next->Match(str, info)) {
					return c;
				}
				
				if (num >= max) return nullptr;

				str = content->Match(str, info);

				if (str == nullptr) {
					return nullptr;
				}
			}

			return nullptr;
		}
	}

	inline const char* Regex::AnyQuantifierNode::Match(const char* str, MatchInfo& info) {
		if (many) {
			const char* minPtr = str + min;
			
			if ((ULong)info.end - (ULong)str >= (ULong)max) {
				str += max;
			}
			else {
				str = info.end;
			}

			while (str >= minPtr) {
				if (const char* c = next->Match(str, info)) {
					return c;
				}

				str--;
			}

			return nullptr;
		}
		else {
			const char* maxPtr;

			if ((ULong)info.end - (ULong)str >= (ULong)max) {
				maxPtr = str + max;
			}
			else {
				maxPtr = info.end;
			}

			str += min;

			while (str <= maxPtr) {
				if (const char* c = next->Match(str, info)) {
					return c;
				}

				str++;
			}

			return nullptr;
		}
	}

	inline const char* Regex::ElementNode::Match(const char* str, MatchInfo& info) {
		info.elementStack.Push(str);

		const char* c = next->Match(str, info);

		info.elementStack.Pop();
		return c;
	}

	inline const char* Regex::ElementEndNode::Match(const char* str, MatchInfo& info) {
		const char* start = info.elementStack.Peek();
		info.elements.Add(String(start, (UInt)(str - start)));

		if (const char* c = next->Match(str, info)) {
			return c;
		}

		info.elements.RemoveLast();
		return nullptr;
	}

	inline const char* Regex::ElementMatchNode::Match(const char* str, MatchInfo& info) {
		if (index >= info.elements.Size()) return nullptr;

		const String element = info.elements[index];

		if ((UInt)(info.end - element) < element.Size()) {
			return nullptr;
		}

		const char* c = str;

		for (UInt i = 0; i < element.Size(); i++, c++) {
			if (*c != element[i]) return nullptr;
		}

		return next->Match(c, info);
	}
}

