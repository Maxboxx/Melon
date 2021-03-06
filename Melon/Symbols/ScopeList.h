#pragma once

#include "Boxx/String.h"
#include "Boxx/Optional.h"
#include "Boxx/Map.h"

namespace Melon {
	namespace Symbols {
		class ScopeList;

		///B Scope
		/// Container for scope info
		class Scope {
		public:
			///H Members

			///T Name
			Boxx::String name;

			///T Template types
			Boxx::Optional<Boxx::List<ScopeList>> types;

			///T Arguments
			/// Arguments for a call
			Boxx::Optional<Boxx::List<ScopeList>> arguments;

			///H Constructors

			///T Empty scope
			Scope();

			///T Create scope
			///A name: The name of the scope
			explicit Scope(const Boxx::String& name);

			~Scope();

			///H Methods

			///T Add scope
			/// Increments scope number for the added scope name
			void AddScope(const Boxx::String& scope);

			///T Get scope number
			/// Gets the scope number for the specified scope
			Boxx::UInt GetScope(const Boxx::String& scope) const;

			///T To string
			/// Converts the scope to a string
			Boxx::String ToString() const;

			///T To Simple String
			/// Converts the scope to a simple string
			Boxx::String ToSimpleString() const;

			///T Copy
			Scope Copy() const;

			///T Is Empty
			/// Checks if the scope is empty
			bool IsEmpty() const;

			///H Operators

			///T Equality operators
			/// Checks if two scopes are equal
			///M
			bool operator==(const Scope& scope) const;
			bool operator!=(const Scope& scope) const;
			///M

			// Used for Map key
			bool operator<(const Scope& scope) const;

			///H Static values

			///T Scope keywords
			/// A collection of scopes for keywords
			///M
			static const Scope Self;
			static const Scope Init;
			static const Scope Final;
			static const Scope Super;
			static const Scope This;
			static const Scope Base;
			static const Scope Global;
			///M

			///T Scope operators
			/// A collection of scopes for operators
			///M
			static const Scope Add;
			static const Scope Sub;
			static const Scope Neg;
			static const Scope Mul;
			static const Scope Div;
			static const Scope IDiv;
			static const Scope Sqr;
			static const Scope Sqrt;
			static const Scope Mod;
			static const Scope Not;
			static const Scope Or;
			static const Scope And;
			static const Scope Xor;
			static const Scope Nor;
			static const Scope Nand;
			static const Scope Xnor;
			static const Scope BitNot;
			static const Scope BitOr;
			static const Scope BitAnd;
			static const Scope BitXor;
			static const Scope BitNor;
			static const Scope BitNand;
			static const Scope BitXnor;
			static const Scope ShiftLeft;
			static const Scope ShiftRight;
			static const Scope Equal;
			static const Scope NotEqual;
			static const Scope LessEqual;
			static const Scope GreaterEqual;
			static const Scope Less;
			static const Scope Greater;
			static const Scope Call;
			static const Scope Assign;
			static const Scope Len;
			static const Scope Index;
			static const Scope As;
			///M

			///T Misc
			/// A collection of scopes
			///M
			static const Scope TemplateSymbol;
			static const Scope Optional;
			static const Scope Unwrap;
			static const Scope Default;
			static const Scope HasValue;
			static const Scope Value;
			///M
		private:
			Boxx::Map<Boxx::String, Boxx::UInt> scopes;
		};

		///B ScopeList
		/// A list of scopes used as the identifier for symbols
		/// Assignments always copies the <type>ScopeList</type> to avoid accidental modifications to identifiers
		class ScopeList {
		public:
			///H constructors

			///T Empty ScopeList
			ScopeList();

			///T Absolute
			explicit ScopeList(const bool absolute);

			///T Copy ScopeList
			ScopeList(const ScopeList& scopeList);

			~ScopeList();

			///H Methods

			///T Add scope
			///R ScopeList: A copy of the <type>ScopeList</type> containing the added scope.
			/// The original <type>ScopeList</type> is not changed
			ScopeList Add(const Scope& scope) const;

			///T Add scope list
			///R ScopeList: A copy of the <type>ScopeList</type> containing the added scopes.
			/// The original <type>ScopeList</type> is not changed
			ScopeList Add(const ScopeList& scopes) const;

			///T Add next
			/// Adds a new scope with the specified name
			/// If the parent scope already contains the specified scope a number will be appended to the end of the scope name
			///R ScopeList: A copy of the <type>ScopeList</type> containing the added scope.
			/// The original <type>ScopeList</type> is not changed
			ScopeList AddNext(const Boxx::String& scope) const;

			///T Pop scope
			///R ScopeList: A copy of the <type>ScopeList</type> with the last scope removed.
			/// The original <type>ScopeList</type> is not changed
			ScopeList Pop() const;

			///T Last
			/// Get the last scope in the list
			Scope Last() const;

			///T To string
			/// Converts the <type>ScopeList</type> to a string containing the scopes
			Boxx::String ToString() const;

			///T To Simple String
			/// Converts the scope list to a simple string
			Boxx::String ToSimpleString() const;

			///T Size
			///R Boxx::UInt: The size of the list
			Boxx::UInt Size() const;

			///T Split
			/// Split up scopes for use in searches
			ScopeList Split() const;

			///T Is Template
			/// Checks if the scope is a relative template argument
			bool IsTemplate() const;

			///H Operators

			///T Index operator
			///M
			Scope operator[](const Boxx::UInt i) const;
			Scope& operator[](const Boxx::UInt i);
			///M

			///T Equality operators
			/// Checks if two scope lists contain the same scopes
			///M
			bool operator==(const ScopeList& scopeList) const;
			bool operator!=(const ScopeList& scopeList) const;
			///M

			bool operator<(const ScopeList& scopeList) const;

			void operator=(const ScopeList& scopeList);

			bool absolute = false;

			///T Types
			///M
			static const ScopeList Bool;
			static const ScopeList Byte;
			static const ScopeList UByte;
			static const ScopeList Short;
			static const ScopeList UShort;
			static const ScopeList Int;
			static const ScopeList UInt;
			static const ScopeList Long;
			static const ScopeList ULong;
			static const ScopeList Huge;
			static const ScopeList Nil;
			static const ScopeList Discard;
			static const ScopeList undefined;
			///M
		private:
			Boxx::List<Scope> scopes;
			Scope baseScope;
		};
	}
}