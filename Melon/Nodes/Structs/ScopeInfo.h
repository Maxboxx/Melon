#pragma once

#include "Boxx/Pointer.h"
#include "Boxx/List.h"
#include "Boxx/Mango.h"
#include "Boxx/Set.h"
#include "Boxx/Stack.h"

#include "Melon/Symbols/NameList.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		///H Structs

		/// Contains information about scopes to make sure that functions always return a value,
		/// or that all member variables are assigned in the constructor.
		struct ScopeInfo {
			/// The different types of scopes.
			enum class ScopeType : Boxx::UByte {
				/// A regular scope.
				Scope,

				/// A loop scope.
				Loop,

				/// A function scope.
				Function,

				/// The main scope of a file.
				Main
			};

			/// The scope type.
			ScopeType type = ScopeType::Main;

			/// {true} if the scope has returned.
			bool hasReturned = false;

			/// {true} if the scope will not return.
			bool willNotReturn = true;

			/// The minimum abort count.
			Boxx::UInt loopAbortCount = 0;

			/// The minimum break count.
			Boxx::UInt loopBreakCount = 0;

			/// The minimum scopewise break count.
			Boxx::UInt scopeBreakCount = 0;

			/// The maximum break count.
			Boxx::UInt maxLoopBreakCount = 0;

			/// The maximum scopewise break count.
			Boxx::UInt maxScopeBreakCount = 0;

			/// All unassigned members.
			Boxx::Set<Symbols::Name> unassigned;

			/// Resets the scope to default values.
			void Reset();

			/// Checks if the current loop might be aborted from an abort, continue or break.
			bool CanAbort() const;

			/// Checks if the current scope will break its execution from a break, continue or abort.
			bool WillBreak() const;

			/// Checks if the current scope will return.
			bool WillReturn() const;

			/// Checks if the current scope will continue its execution.
			bool WillContinue() const;

			/// Checks if the current scope might continue its execution.
			bool CanContinue() const;

			/// Checks if the current scope will not continue its execution.
			bool WillNotContinue() const;

			/// Enters a new scope of the specified type.
			void EnterScope(const ScopeType type);

			/// Exits the current scope.
			void ExitScope();

			/// Copies the current scope so it can be used for different branches.
			ScopeInfo CopyBranch();

			/// Calculate the union of two branches.
			static ScopeInfo BranchUnion(const ScopeInfo& branch1, const ScopeInfo& branch2);

			/// Calculate the weak union of two branches.
			static ScopeInfo WeakBranchUnion(const ScopeInfo& branch1, const ScopeInfo& branch2);

			/// Calculate the intersection between two branches.
			static ScopeInfo BranchIntersection(const ScopeInfo& branch1, const ScopeInfo& branch2);

			/// Calculate the weak intersection between two branches.
			static ScopeInfo WeakBranchIntersection(const ScopeInfo& branch1, const ScopeInfo& branch2);

		private:
			Boxx::Stack<ScopeType> types;
		};
	}
}
