#pragma once

#include "Types.h"

#include "Boxx/Pointer.h"
#include "Boxx/List.h"
#include "Boxx/Mango.h"
#include "Boxx/Set.h"
#include "Boxx/Stack.h"
#include "Boxx/Tuple.h"
#include "Boxx/Math.h"
#include "Boxx/Error.h"
#include "Boxx/String.h"
#include "Boxx/StringBuilder.h";

#include "Kiwi/Kiwi.h"

#include "Structs/CompiledNode.h"
#include "Structs/CompileInfo.h"
#include "Structs/OptimizeInfo.h"
#include "Structs/ScanInfo.h"
#include "Structs/ScanInfoStack.h"
#include "Structs/ScanResult.h"
#include "Structs/ScopeInfo.h"
#include "Structs/StackPtr.h"

#include "Melon/Symbols/Symbol.h"
#include "Melon/Symbols/SymbolTable.h"

#include "Melon/Optimizing/OptimizerInstruction.h"

///N Melon::Nodes
namespace Melon {
	class MelonCompiler;

	namespace Parsing {
		struct ParsingInfo;
	}

	namespace Symbols {
		class FunctionSymbol;

		namespace Nodes {
			class SymbolNode;
		}
	}

	namespace Nodes {
		class Expression;
		class Condition;
		class Statement;
		class Statements;

		class RootNode;

		/// Base for all nodes.
		class Node {
		public:
			Symbols::Symbol* const scope;

			/// Creates a node.
			Node(Symbols::Symbol* const scope, const FileInfo& file);
			~Node();

			/// Scans the node for potential extra includes.
			virtual void IncludeScan(Parsing::ParsingInfo& info);

			/// Scans the node for potential errors.
			virtual ScanResult Scan(ScanInfoStack& info);

			/// Compiles the node.
			virtual CompiledNode Compile(CompileInfo& info) = 0;

			/// Gets the byte size of the node.
			///p This is only used by the compile step.
			virtual Boxx::UInt GetSize() const;

			/// Checks if the node has side effects
			bool HasSideEffects();

			/// Checks if the node has side effects outside of the specified scope.
			bool HasSideEffects(const Symbols::NameList& scope);

			/// Gets the side effect scope.
			Symbols::NameList GetSideEffectScope(const bool assign);

			/// Converts the node to melon.
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const = 0;

			/// Gets the file info of the node.
			FileInfo File() const;

			/// Optimizes an expression.
			///A expression: The expression to optimize.
			///p The optimized expression will be assigned to this value.
			static void Optimize(Ptr<Expression>& expression, OptimizeInfo& info);

			/// Optimizes a condition.
			///A condition: The condition to optimize.
			///p The optimized condition will be assigned to this value.
			static void Optimize(Ptr<Condition>& condition, OptimizeInfo& info);

			/// Optimizes a statement.
			///A statement: The statement to optimize.
			///p The optimized statement will be assigned to this value.
			static void Optimize(Ptr<Statement>& statement, OptimizeInfo& info);

			/// Optimizes statements.
			///A statements: The statements to optimize.
			///p The optimized statements will be assigned to this value.
			static void Optimize(Ptr<Statements>& statements, OptimizeInfo& info);

			/// Optimizes a node.
			///A node: The node to optimize.
			///p The optimized node will be assigned to this value.
			static void Optimize(Ptr<Node>& node, OptimizeInfo& info);

			static CompiledNode CompileAssignment(Weak<Expression> assignable, Weak<Expression> value, CompileInfo& info, const FileInfo& file);

		protected:
			friend RootNode;
			friend Symbols::FunctionSymbol;

			FileInfo file;

			static Symbols::NameList CombineSideEffects(const Symbols::NameList& scope1, const Symbols::NameList& scope2);

			static ScanResult ScanAssignment(Weak<Expression> assignable, Weak<Expression> value, ScanInfoStack& info, const FileInfo& file);

			static RootNode* Root();

			static bool IsEmpty(Weak<Statement> statement);

			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			void Include(const Symbols::Name& name, Parsing::ParsingInfo& info);
			void Include(const Symbols::NameList& name, Parsing::ParsingInfo& info);

		private:
			friend MelonCompiler;

			Boxx::Optional<Symbols::NameList> sideEffectScope;

			static RootNode* _root;
		};

		///H Errors

		/// Used in include scan if more info is needed for the scan.
		class IncludeScanError : public Boxx::Error {
		public:
			IncludeScanError() : Error() {}
			IncludeScanError(const char* const msg) : Error(msg) {}

			virtual Boxx::String Name() const override {
				return "IncludeScanError";
			}
		};
	}
}