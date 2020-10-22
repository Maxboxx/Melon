#pragma once

#include "Boxx/String.h"
#include "Boxx/List.h"
#include "Boxx/Map.h"
#include "Boxx/Pointer.h"
#include "Boxx/Mango.h"
#include "Boxx/Tuple.h"

#include "ScopeList.h"

#include "Melon/Errors.h"

namespace Melon {
	namespace Nodes {
		class Node;
	}

	namespace Parsing {
		struct ParsingInfo;
	}

	namespace Symbols {
		namespace Nodes {
			class SymbolNode;
		}

		///B SymbolType
		enum class SymbolType : Boxx::UByte {
			///T Values
			///M
			None,
			Type,
			Variable,
			Function,
			Method,
			Class,
			Struct,
			Enum,
			Interface,
			Scope,
			Namespace,
			Value,
			Template
			///M
		};

		///T Symbol attribute
		enum class SymbolAttribute : Boxx::UByte {
			// Variable attributes
			Const,
			Ref,
			Copy,

			// Function attributes
			Static,
			Required,
			Override,
			Debug
		};

		///B Symbol
		/// Class for symbols
		class Symbol {
		public:
			///H Members

			///T Type
			/// The symbol type
			SymbolType type = SymbolType::None;

			///T Variable type
			/// The type of the variable if <code>type</code> is <code>SymbolType::Variable</code>
			/// The type of the template argument if <code>type</code> is <code>SymbolType::Template</code>
			/// The main template symbol if the symbol has template arguments
			ScopeList varType;

			///T Scope
			/// The absolute scope of the symbol
			ScopeList scope;

			///T Size/Function
			/// The size of the type if <code>type</code> is <code>SymbolType::Type</code>, <code>SymbolType::Struct</code>, <code>SymbolType::Class</code> or <code>SymbolType::Enum</code>
			/// The function id if <code>type</code> is <code>SymbolType::Function</code> or <code>SymbolType::Method</code>
			/// The template index if <code>type</code> is <code>SymbolType::Template</code>
			///M
			union {
				Boxx::UInt size = 0;
				Boxx::UInt functionID;
				Boxx::UInt templateIndex;
			};
			///M

			///T Signed/Assigned/Explicit
			/// Whether or not the type is signed if <code>type</code> is <code>SymbolType::Type</code>
			/// Whether or not the variable has been assigned if <code>type</code> is <code>SymbolType::Variable</code>
			/// Whether or not the conversion operator is explicit if <code>type</code> is <code>SymbolType::Function</code> or <code>SymbolType::Method</code>
			///M
			union {
				bool isSigned = false;
				bool isAssigned;
				bool isExplicit;
			};
			///M

			///T Basic
			/// Wheter or not the symbol is a basic symbol
			bool basic = false;

			///T Stack
			/// The stack index of the variable if <code>type</code> is <code>SymbolType::Variable</code>
			/// The stack offset if the symbol is a member variable
			/// The value if <code>type</code> is <code>SymbolType::Value</code>
			///M
			union {
				Boxx::Long stackIndex = 0;
				Boxx::Long offset;
				Boxx::Long value;
			};
			///M

			///T Symbol node
			/// Used for basic operations instead of having to call a function
			Boxx::Pointer<Nodes::SymbolNode> symbolNode;

			///T Node
			/// A node for the symbol
			Boxx::Pointer<Melon::Nodes::Node> node;

			///T Return values
			/// The relative types for the return values
			/// Only used if <code>type</code> is <code>SymbolType::Function</code> or <code>SymbolType::Method</code>
			Boxx::List<ScopeList> returnValues;

			///T Argument values
			/// The relative types of function arguments
			Boxx::List<ScopeList> arguments;

			///T Template arguments
			Boxx::List<ScopeList> templateArgs;

			///T Names
			/// The names of arguments if <code>type</code> is <code>SymbolType::Function</code> or <code>SymbolType::Method</code>
			/// The list of non static members if <code>type</code> is <code>SymbolType::Struct</code> or <code>SymbolType::Class</code>
			Boxx::List<Scope> names;

			///T Variants
			/// A list of overloads for the symbol
			Boxx::List<Symbol> variants;

			///T Template variants
			/// A list of template variants for the symbol
			Boxx::List<Symbol> templateVariants;

			///T Scopes
			/// A map containing all child scopes
			Boxx::Map<Boxx::String, Symbol> scopes;

			///T Attributes
			/// A set of the attributes for this symbol
			Boxx::Set<SymbolAttribute> attributes;

			///T Symbol File
			Scope symbolFile;

			///T Symbol namespace
			ScopeList symbolNamespace;

			///T Symbol Path
			Boxx::String symbolPath;

			///T Included namespaces
			Boxx::Set<ScopeList> includedNamespaces;

			///T StatementNumber
			/// The statement number the symbol was created at
			/// Only used if <code>type</code> is <code>SymbolType::Variable</code> or a plain <code>SymbolType::Function</code>
			Boxx::UInt statementNumber = 0;

			///H Constructors

			///T Constructors
			///M
			Symbol();
			Symbol(const SymbolType type);
			///M

			~Symbol();

		private:
			Symbol& GetTemplate(const Scope& scope, const FileInfo& file);

		public:
			struct TemplateSymbol;

			static Boxx::Tuple<Symbol, Boxx::List<ScopeList>> FindTemplateArgs(const TemplateSymbol& symbol);

			///T Get
			/// Get a specific child symbol
			///M
			Symbol& Get(const Scope& scope, const FileInfo& file);
			Symbol Get(const Scope& scope, const FileInfo& file) const;
			Symbol Get(const ScopeList& scope, const FileInfo& file) const;
			///M

			///T Get Type
			Symbol GetType(const FileInfo& file) const;

			///T Is of Type
			/// Checks if the symbol is of the specified type
			bool IsOfType(const ScopeList& type) const;

			///T Get Return Type
			Symbol GetReturnType(const Boxx::UInt index) const;

			///T Get Argument Type
			Symbol GetArgumentType(const Boxx::UInt index) const;

			FileInfo GetFileInfo() const;

			///T Contains
			/// Checks if the symbol contains a specific child symbol
			///M
			bool Contains(const Scope& scope) const;
			bool Contains(const ScopeList& scope, const FileInfo& file) const;
			///M

			///T Add
			/// Adds a new child symbol
			bool Add(const Scope& scope, const Symbol& symbol, const FileInfo& file, const bool redefine = false);

			///T Is type
			/// Checks if the symbol can be used as a type
			bool IsType() const;

			///T Clear Assignment
			/// Clears the assign info of the symbol
			/// Used if <code>type</code> is <code>SymbolType::Variable</code>
			void ClearAssign();

			///T Assign all
			/// Sets the assign info of the symbol to true
			/// Used if <code>type</code> is <code>SymbolType::Variable</code>
			void AssignAll();

			///T Is assigned
			/// Checks if all variables have been assigned
			/// Used if <code>type</code> is <code>SymbolType::Variable</code>
			bool IsAssigned() const;

			///T Is all required vars
			/// Checks if the specified vars contain all required vars for init
			bool IsAllRequiredVars(const Boxx::List<Scope>& vars) const;

			///T Get Unassigned vars
			/// Gets a list of all unassigned vars
			///M
			Boxx::List<Scope> GetUnassignedVars() const;
			Boxx::Set<Scope> GetUnassignedVarsSet() const;
			Boxx::List<Scope> GetUnassignedVars(const Boxx::List<Scope>& vars) const;
			///M

			///T Is valid
			/// Checks if the symbol is valid
			bool IsValid() const;

			///T Has Type Args
			/// Checks if the <code>args</code> member contains types
			bool HasTypeArgs() const;

			///T Has Type Returns
			/// Checks if the <code>ret</code> member contains types
			bool HasTypeReturns() const;

			///T Specialize Template
			/// Specializes a template symbol
			///M
			void SpecializeTemplate(Symbol& symbol, const Boxx::List<ScopeList>& types, Parsing::ParsingInfo& info) const;
			void SpecializeTemplate(Symbol& symbol, const Symbol& templateSymbol, const Boxx::List<ScopeList>& types, Parsing::ParsingInfo& info) const;
			///M

			///H Static functions

			///T Replace Templates
			/// Replaces template arguments with real types
			///M
			static ScopeList ReplaceTemplates(const ScopeList& type, const Symbol& templateSymbol, const Boxx::List<ScopeList>& types);
			static ScopeList ReplaceTemplates(const ScopeList& type, const FileInfo& file);
			static ScopeList ReplaceNearestTemplates(const ScopeList& scope, const ScopeList& type, const FileInfo& file);
			///M

			///T Set Template Values
			/// Sets the values of templates
			static void SetTemplateValues(const ScopeList& scope, const FileInfo& file);

			///T Add
			/// Adds a new symbol
			static bool Add(const ScopeList& scopes, const Symbol& symbol, const FileInfo& file, const bool redefine = false);

			///T Find
			/// Find a specific symbol
			static Symbol Find(const ScopeList& scopes, const FileInfo& file);

			///T Find In Namespace
			/// Find a specific symbol in the namespaces provided by <code>file</code>
			static Symbol FindInNamespace(const ScopeList& scopes, const FileInfo& file);

			///T Contains
			/// Checks if a specific symbol exists
			///M
			static bool ContainsScope(const Scope& scope);
			static bool Contains(const ScopeList& scopes);
			///M

			///T Find nearest
			/// Find the nearest symbol to the specified scope
			///M
			static Symbol FindNearest(const ScopeList& scopes, const Scope& name, const FileInfo& file);
			static Symbol FindNearest(const ScopeList& scopes, const ScopeList& name, const FileInfo& file);
			///M

			///T Find Nearest in Namespace
			/// Find the nearest symbol to the specified scope in the namespaces provided by <code>file</code>
			///M
			static Symbol FindNearestInNamespace(const ScopeList& scopes, const Scope& name, const FileInfo& file);
			static Symbol FindNearestInNamespace(const ScopeList& scopes, const ScopeList& name, const FileInfo& file);
			///M

			///T Find nearest type
			/// Find the nearest type symbol to the specified scope
			///M
			static Symbol FindNearestType(const ScopeList& scopes, const Scope& name, const FileInfo& file);
			static Symbol FindNearestType(const ScopeList& scopes, const ScopeList& name, const FileInfo& file);
			///M

			///T Find nearest type in namespace
			/// Find the nearest type symbol to the specified scope in the namespaces provided by <code>file</code>
			///M
			static Symbol FindNearestTypeInNamespace(const ScopeList& scopes, const Scope& name, const FileInfo& file);
			static Symbol FindNearestTypeInNamespace(const ScopeList& scopes, const ScopeList& name, const FileInfo& file);
			///M

			///T Is of Type
			/// Checks if the specified scope is of the specified type
			static bool IsOfType(const ScopeList& scope, const ScopeList& type, const FileInfo& file);

			///T Find current function
			/// Find the function the current scope is in
			static Symbol FindCurrentFunction(const ScopeList& scopes, const FileInfo& file);

			///T Find binary operator
			/// Finds the binary operator for the specified operand types
			static Symbol FindOperator(const Scope& op, const ScopeList& type1, const ScopeList& type2, const FileInfo& file);

			///T Find function
			/// Finds the overload for the specified function that accepts the specified argument types
			static Symbol FindFunction(const ScopeList& func, const Boxx::List<ScopeList>& argTypes, const FileInfo& file);

			///T Find method
			/// Finds the overload for the specified method that accepts the specified argument types
			static Symbol FindMethod(const ScopeList& func, const Boxx::List<ScopeList>& argTypes, const FileInfo& file);

		private:
			static Symbol FindFunction(const ScopeList& func, const Boxx::List<ScopeList>& argTypes, const FileInfo& file, const Boxx::UInt offset, const SymbolType type);

		public:

			///T Find Implicit Conversion
			/// Finds the implicit conversion operator for a specific type
			static Symbol FindImplicitConversion(const ScopeList& from, const ScopeList& to, const FileInfo& file);

			///T Find Explicit Conversion
			/// Finds the implicit or explicit conversion operator for a specific type
			static Symbol FindExplicitConversion(const ScopeList& from, const ScopeList& to, const FileInfo& file);

			///T Has Implicit Conversion
			/// Checks if an implicit conversion operator for a specific type exists
			static bool HasImplicitConversion(const ScopeList& from, const ScopeList& to);

			///T Has Explicit Conversion
			/// Checks if an implicit or explicit conversion operator for a specific type exists
			static bool HasExplicitConversion(const ScopeList& from, const ScopeList& to);

			///T To mango
			/// Convert the symbol table to mango
			static Boxx::Mango ToMango(const bool ignoreBasic);

			///T Setup
			/// Setup the basic types for the symbol table
			static void Setup();

			static Boxx::List<TemplateSymbol> templateSymbols;

		private:
			static Symbol symbols;
			static Symbol empty;

			Boxx::List<Boxx::Mango> ToMangoList(const ScopeList& scopes);
		};

		struct Symbol::TemplateSymbol {
			ScopeList type;
			ScopeList scope;
			FileInfo file;
		};
	}
}