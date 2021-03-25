#pragma once

#include "Boxx/String.h"
#include "Boxx/Optional.h"
#include "Boxx/Map.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		class ScopeList;

		///H Scope

		/// Container for scope info.
		class Scope {
		public:
			/// The scope name.
			Boxx::String name;

			/// The template types of the scope.
			Boxx::Optional<Boxx::List<ScopeList>> types;

			/// The argument types for the scope.
			Boxx::Optional<Boxx::List<ScopeList>> arguments;

			/// Creates an empty scope.
			Scope();

			/// Creates a scope with a name.
			explicit Scope(const Boxx::String& name);

			~Scope();

			/// Increments the scope number for the specified scope name.
			void AddScope(const Boxx::String& scope);

			/// Gets the scope number for the specified scope name.
			Boxx::UInt GetScope(const Boxx::String& scope) const;

			/// Converts the scope to a string.
			Boxx::String ToString() const;

			/// Converts the scope to a simple string.
			Boxx::String ToSimpleString() const;

			/// Copies the scope.
			Scope Copy() const;

			/// {true} if the scope is empty.
			bool IsEmpty() const;

			/// Checks if two scopes are equal.
			bool operator==(const Scope& scope) const;

			/// Checks if two scopes are not equal.
			bool operator!=(const Scope& scope) const;

			bool operator<(const Scope& scope) const;

			///H Scope Keywords

			/// The {self} scope.
			static const Scope Self;

			/// The {init} scope.
			static const Scope Init;

			/// The {final} scope.
			static const Scope Final;

			/// The {super} scope.
			static const Scope Super;

			/// The {this} scope.
			static const Scope This;

			/// The {base} scope.
			static const Scope Base;

			/// The {global} scope.
			static const Scope Global;

			///H Operator Scopes
			
			/// The {+} scope.
			static const Scope Add;

			/// The {-} scope.
			static const Scope Sub;

			/// The {-} scope.
			static const Scope Neg;

			/// The {*} scope.
			static const Scope Mul;

			/// The {/} scope.
			static const Scope Div;

			/// The {//} scope.
			static const Scope IDiv;

			/// The {*} scope.
			static const Scope Sqr;

			/// The {/} scope.
			static const Scope Sqrt;

			/// The {%} scope.
			static const Scope Mod;

			/// The {not} scope.
			static const Scope Not;

			/// The {or} scope.
			static const Scope Or;

			/// The {and} scope.
			static const Scope And;

			/// The {xor} scope.
			static const Scope Xor;

			/// The {nor} scope.
			static const Scope Nor;

			/// The {nand} scope.
			static const Scope Nand;

			/// The {xnor} scope.
			static const Scope Xnor;

			/// The {~} scope.
			static const Scope BitNot;

			/// The {|} scope.
			static const Scope BitOr;

			/// The {&} scope.
			static const Scope BitAnd;

			/// The {^} scope.
			static const Scope BitXor;

			/// The {~|} scope.
			static const Scope BitNor;

			/// The {~&} scope.
			static const Scope BitNand;

			/// The {~^} scope.
			static const Scope BitXnor;

			/// The {<<} scope.
			static const Scope ShiftLeft;

			/// The {>>} scope.
			static const Scope ShiftRight;

			/// The {==} scope.
			static const Scope Equal;
				
			/// The {!=} scope.
			static const Scope NotEqual;

			/// The {<=} scope.
			static const Scope LessEqual;

			/// The {>=} scope.
			static const Scope GreaterEqual;

			/// The {<} scope.
			static const Scope Less;

			/// The {>} scope.
			static const Scope Greater;

			/// The {()} scope.
			static const Scope Call;

			/// The {=} scope.
			static const Scope Assign;

			/// The {#} scope.
			static const Scope Len;

			/// The {[]} scope.
			static const Scope Index;

			/// The {as} scope.
			static const Scope As;

			///H Misc Scopes

			/// The {<>} scope.
			static const Scope TemplateSymbol;

			/// The {?} scope.
			static const Scope Optional;

			/// The {!} scope.
			static const Scope Unwrap;

			/// The {??} scope.
			static const Scope Default;

			/// The {<hasValue>} scope.
			static const Scope HasValue;

			/// The {<value>} scope.
			static const Scope Value;
		private:
			Boxx::Map<Boxx::String, Boxx::UInt> scopes;
		};

		/// A list of scopes used as the identifier for symbols.
		///p Assignments always copies the {ScopeList} to avoid accidental modifications to identifiers.
		class ScopeList {
		public:
			/// Creates an empty scope list.
			ScopeList();

			/// Creates an empty scope list.
			///A absolute
			///i {true} if the scope is absolute.
			///i {false} if the scope is relative.
			explicit ScopeList(const bool absolute);

			ScopeList(const ScopeList& scopeList);

			~ScopeList();

			/// Adds a scope to the scope list and returns the new scope list.
			///p The original scope list is not modified.
			ScopeList Add(const Scope& scope) const;

			/// Adds a list of scopes to the scope list and returns the new scope list.
			///p The original scope list is not modified.
			ScopeList Add(const ScopeList& scopes) const;

			/// Adds a new scope with the specified name to the scopelist and returns the new scope list.
			///p If the parent scope already contains the specified scope, a number will be appended to the end of the scope name.
			///p The original scope list will not be modified.
			ScopeList AddNext(const Boxx::String& scope) const;

			/// Returns a copy of the scope list with the last scope removed from it.
			ScopeList Pop() const;

			/// Get the last scope in the list.
			Scope Last() const;

			/// Converts the scope list to a string.
			Boxx::String ToString() const;

			/// Converts the scope list to a simple string.
			Boxx::String ToSimpleString() const;

			/// Gets the size of the scope list.
			Boxx::UInt Size() const;

			/// Splits up scopes for use in searches.
			ScopeList Split() const;

			/// {true} if the scope list is a relative template type.
			bool IsTemplate() const;

			/// Gets the scope at the specified index.
			Scope operator[](const Boxx::UInt i) const;
			Scope& operator[](const Boxx::UInt i);

			/// Checks if two scope lists are equal.
			bool operator==(const ScopeList& scopeList) const;

			/// Checks if two scope lists are not equal.
			bool operator!=(const ScopeList& scopeList) const;

			bool operator<(const ScopeList& scopeList) const;

			void operator=(const ScopeList& scopeList);

			/// {true} if the scope list is absolute.
			bool absolute = false;

			///H Type Identifiers.

			/// A scope List for the {bool} type.
			static const ScopeList Bool;

			/// A scope List for the {byte} type.
			static const ScopeList Byte;

			/// A scope List for the {ubyte} type.
			static const ScopeList UByte;

			/// A scope List for the {short} type.
			static const ScopeList Short;

			/// A scope List for the {ushort} type.
			static const ScopeList UShort;

			/// A scope List for the {int} type.
			static const ScopeList Int;

			/// A scope List for the {uint} type.
			static const ScopeList UInt;

			/// A scope List for the {long} type.
			static const ScopeList Long;

			/// A scope List for the {ulong} type.
			static const ScopeList ULong;

			/// A scope List for the {huge} type.
			static const ScopeList Huge;

			/// A scope List for the {nil} type.
			static const ScopeList Nil;

			/// A scope List for the {_} type.
			static const ScopeList Discard;

			/// A scope List for undefined types.
			static const ScopeList undefined;
		private:
			Boxx::List<Scope> scopes;
			Scope baseScope;
		};
	}
}