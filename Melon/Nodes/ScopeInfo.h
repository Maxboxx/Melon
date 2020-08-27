#pragma once

#include "Boxx/Pointer.h"
#include "Boxx/List.h"
#include "Boxx/Mango.h"
#include "Boxx/Set.h"
#include "Boxx/Stack.h"

#include "Melon/Symbols/ScopeList.h"

namespace Melon {
	namespace Nodes {

		///B ScopeInfo
		/// Contains information about scopes to make sure that functions always return a value or that all member variables are assigned in the constructor
		struct ScopeInfo {
			////B ScopeType
			//// The different types of scopes
			enum class ScopeType : Boxx::UByte {
				////T Values
				////M
				Scope,
				Loop,
				Function,
				Main
				////M
			};

			ScopeType type = ScopeType::Main;

			bool hasReturned   = false;
			bool willNotReturn = true;

			Boxx::UInt loopAbortCount     = 0;
			Boxx::UInt loopBreakCount     = 0;
			Boxx::UInt scopeBreakCount    = 0;
			Boxx::UInt maxLoopBreakCount  = 0;
			Boxx::UInt maxScopeBreakCount = 0;

			Boxx::Set<Symbols::Scope> unassigned;

			///T Reset
			/// Resets the scope to default values
			void Reset();

			///T Can Abort
			/// Checks if the current loop might be aborted from an abort, continue or break
			bool CanAbort() const;

			///T Will Break
			/// Checks if the current scope will break its execution from a break, continue or abort
			bool WillBreak() const;

			///T Will Return
			/// Checks if the current scope will return
			bool WillReturn() const;

			///T Will Continue
			/// Checks if the current scope will continue its execution
			bool WillContinue() const;

			///T Can Continue
			/// Checks if the current scope might continue its execution
			bool CanContinue() const;

			///T Will Not Continue
			/// Checks if the current scope will not continue its execution
			bool WillNotContinue() const;

			///T Enter a new scope
			void EnterScope(const ScopeType type);

			///T Exit current scope
			void ExitScope();

			///T Copy Branch
			/// Copies the current scope so it can be used for different branches
			ScopeInfo CopyBranch();

			///T Branch Union
			/// Calculate the union of two branches
			static ScopeInfo BranchUnion(const ScopeInfo& branch1, const ScopeInfo& branch2);

			///T Weak Branch Union
			/// Calculate the weak union of two branches
			static ScopeInfo WeakBranchUnion(const ScopeInfo& branch1, const ScopeInfo& branch2);

			///T Branch Intersection
			/// Calculate the intersection between two branches
			static ScopeInfo BranchIntersection(const ScopeInfo& branch1, const ScopeInfo& branch2);

			///T Weak Branch Intersection
			/// Calculate the weak intersection between two branches
			static ScopeInfo WeakBranchIntersection(const ScopeInfo& branch1, const ScopeInfo& branch2);

		private:
			Boxx::Stack<ScopeType> types;
		};
	}
}