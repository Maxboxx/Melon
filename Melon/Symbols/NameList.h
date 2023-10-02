#pragma once

#include "Boxx/String.h"
#include "Boxx/Optional.h"
#include "Boxx/Map.h"

///N Melon::Symbols
namespace Melon {
	namespace Symbols {
		class NameList;

		///H Names

		/// Contains a symbol name.
		class Name {
		public:
			/// The symbol name.
			Boxx::String name;

			/// The template types of the symbol.
			Boxx::Optional<Boxx::List<NameList>> types;

			/// The argument types for the symbol.
			Boxx::Optional<Boxx::List<NameList>> arguments;

			/// Creates an empty name.
			Name();

			/// Creates from name.
			explicit Name(const Boxx::String& name);

			~Name();

			/// Increments the scope number for the specified scope name.
			void AddScope(const Boxx::String& scope);

			/// Gets the scope number for the specified scope name.
			Boxx::UInt GetScope(const Boxx::String& scope) const;

			/// Converts the name to a string.
			Boxx::String ToString() const;

			/// Converts the name to a simple string.
			Boxx::String ToSimpleString() const;

			/// Copies the name.
			Name Copy() const;

			/// {true} if the name is empty.
			bool IsEmpty() const;

			/// Checks if two names are equal.
			bool operator==(const Name& scope) const;

			/// Checks if two names are not equal.
			bool operator!=(const Name& scope) const;

			bool operator<(const Name& scope) const;

			///H Name Keywords

			/// The {self} name.
			static const Name Self;

			/// The {init} name.
			static const Name Init;

			/// The {final} name.
			static const Name Final;

			/// The {super} name.
			static const Name Super;

			/// The {this} name.
			static const Name This;

			/// The {base} name.
			static const Name Base;

			/// The {global} name.
			static const Name Global;

			///H Operator Names
			
			/// The {+} name.
			static const Name Add;

			/// The {-} name.
			static const Name Sub;

			/// The {-} name.
			static const Name Neg;

			/// The {*} name.
			static const Name Mul;

			/// The {/} name.
			static const Name Div;

			/// The {//} name.
			static const Name IDiv;

			/// The {*} name.
			static const Name Sqr;

			/// The {/} name.
			static const Name Sqrt;

			/// The {%} name.
			static const Name Mod;

			/// The {not} name.
			static const Name Not;

			/// The {or} name.
			static const Name Or;

			/// The {and} name.
			static const Name And;

			/// The {xor} name.
			static const Name Xor;

			/// The {nor} name.
			static const Name Nor;

			/// The {nand} name.
			static const Name Nand;

			/// The {xnor} name.
			static const Name Xnor;

			/// The {~} name.
			static const Name BitNot;

			/// The {|} name.
			static const Name BitOr;

			/// The {&} name.
			static const Name BitAnd;

			/// The {^} name.
			static const Name BitXor;

			/// The {~|} name.
			static const Name BitNor;

			/// The {~&} name.
			static const Name BitNand;

			/// The {~^} name.
			static const Name BitXnor;

			/// The {<<} name.
			static const Name ShiftLeft;

			/// The {>>} name.
			static const Name ShiftRight;

			/// The {==} name.
			static const Name Equal;
				
			/// The {!=} name.
			static const Name NotEqual;

			/// The {<=} name.
			static const Name LessEqual;

			/// The {>=} name.
			static const Name GreaterEqual;

			/// The {<} name.
			static const Name Less;

			/// The {>} name.
			static const Name Greater;

			/// The {()} name.
			static const Name Call;

			/// The {=} name.
			static const Name Assign;

			/// The {#} name.
			static const Name Len;

			/// The {[]} name.
			static const Name Index;

			/// The {as} name.
			static const Name As;

			///H Misc Names

			/// The {<template>} name.
			static const Name TemplateSymbol;

			/// The {?} name.
			static const Name Optional;

			/// The {!} name.
			static const Name Unwrap;

			/// The {??} name.
			static const Name Default;

			/// The {<hasValue>} name.
			static const Name HasValue;

			/// The {<value>} name.
			static const Name Value;

			/// The array name.
			static const Name Array;

			/// The list name.
			static const Name List;

			/// The {<length>} name.
			static const Name Length;

			/// The {<capacity>} name.
			static const Name Capacity;

			/// The {<items>} name.
			static const Name Items;
		private:
			Boxx::Map<Boxx::String, Boxx::UInt> scopes;
		};

		/// A list of name used as the identifier for symbols.
		///p Assignments always copies the {NameList} to avoid accidental modifications to identifiers.
		class NameList {
		public:
			/// Creates an empty name list.
			NameList();

			/// Creates an empty name list.
			///A absolute
			///i {true} if the name is absolute.
			///i {false} if the name is relative.
			explicit NameList(const bool absolute);

			/// Create from name.
			explicit NameList(const Name& name);

			/// Create from names.
			NameList(const Name& name1, const Name& name2);

			/// Create from names.
			NameList(const Name& name1, const Name& name2, const Name& name3);

			/// Create from name.
			///A absolute
			///i {true} if the name is absolute.
			///i {false} if the name is relative.
			NameList(const bool absolute, const Name& name);

			/// Create from names.
			///A absolute
			///i {true} if the name is absolute.
			///i {false} if the name is relative.
			NameList(const bool absolute, const Name& name1, const Name& name2);

			/// Create from names.
			///A absolute
			///i {true} if the name is absolute.
			///i {false} if the name is relative.
			NameList(const bool absolute, const Name& name1, const Name& name2, const Name& name3);

			NameList(const NameList& nameList);

			~NameList();

			/// Adds a name to the name list and returns the new name list.
			///p The original name list is not modified.
			NameList Add(const Name& name) const;

			/// Adds a list of names to the name list and returns the new name list.
			///p The original name list is not modified.
			NameList Add(const NameList& names) const;

			/// Adds a new scope with the specified name to the name list and returns the new name list.
			///p If the parent name already contains the specified scope, a number will be appended to the end of the scope name.
			///p The original name list will not be modified.
			NameList AddNext(const Boxx::String& scope) const;

			/// Returns a copy of the name list with the last name removed from it.
			NameList Pop() const;

			/// Get the last name in the list.
			Name Last() const;

			/// Converts the name list to a string.
			Boxx::String ToString() const;

			/// Converts the name list to a simple string.
			Boxx::String ToSimpleString() const;

			/// Gets the size of the name list.
			Boxx::UInt Size() const;

			/// Splits up names for use in searches.
			NameList Split() const;

			/// {true} if the name list is a relative template type.
			bool IsTemplate() const;

			/// {true} if the name list has template lists.
			bool HasTemplates() const;

			/// Gets the name at the specified index.
			Name operator[](const Boxx::UInt i) const;
			Name& operator[](const Boxx::UInt i);

			/// Checks if two name lists are equal.
			bool operator==(const NameList& scopeList) const;

			/// Checks if two name lists are not equal.
			bool operator!=(const NameList& scopeList) const;

			bool operator<(const NameList& scopeList) const;

			void operator=(const NameList& scopeList);

			/// {true} if the name list is absolute.
			bool absolute = false;

			///H Type Identifiers.

			/// A name List for the {bool} type.
			static const NameList Bool;

			/// A name List for the {byte} type.
			static const NameList Byte;

			/// A name List for the {tiny} type.
			static const NameList Tiny;

			/// A name List for the {utiny} type.
			static const NameList UTiny;

			/// A name List for the {short} type.
			static const NameList Short;

			/// A name List for the {ushort} type.
			static const NameList UShort;

			/// A name List for the {int} type.
			static const NameList Int;

			/// A name List for the {uint} type.
			static const NameList UInt;

			/// A name List for the {long} type.
			static const NameList Long;

			/// A name List for the {ulong} type.
			static const NameList ULong;

			/// A name List for the {huge} type.
			static const NameList Huge;

			/// A name List for the {char} type.
			static const NameList Char;

			/// A name List for the {string} type.
			static const NameList String;

			/// A name List for the {nil} type.
			static const NameList Nil;

			/// A name List for the {_} type.
			static const NameList Discard;

			/// A name List for the {any} type.
			static const NameList Any;

			/// A name List for undefined types.
			static const NameList undefined;
		private:
			Boxx::List<Name> names;
			Name baseName;
		};
	}
}