#pragma once

#include "Node.h"

#include "Boxx/Math.h"

namespace Melon {
	namespace Nodes {
		class MemoryNode;

		///B SwitchNode
		/// Node for both switch-statements and switch-expressions
		class SwitchNode : public Node {
		public:

			///T Expression
			/// <key>true</key> if the switch is an expression
			bool expr;

			///T Match expression
			NodePtr match;

			///T Nodes
			/// The nodes for all the cases
			Boxx::List<NodePtr> nodes;

			///T Cases
			/// All case expressions
			Boxx::List<Boxx::List<NodePtr>> cases;

			///T Default
			NodePtr def;

			SwitchNode(Symbols::Symbol* const scope, const FileInfo& file);
			~SwitchNode();
			
			////B SwitchScanInfo
			//// Scan info for switch nodes
			struct SwitchScanInfo {
				////T Init
				//// True if the switch is in a constructor
				bool init = false;
				
				////T Scope
				//// Scope info
				ScopeInfo scope;

				////T Cases
				//// Scope info for all cases
				Boxx::List<ScopeInfo> cases;

				////T Will a Case Run
				//// True if at least one case is guaranteed to run
				bool willACaseRun = false;
			};

			////B SwitchCompileInfo
			//// Compile info for switch nodes
			struct SwitchCompileInfo {
				////T Compiled Node
				CompiledNode cn;

				////T Match
				//// The memory location of the match value
				Boxx::Pointer<MemoryNode> match;

				////T End Jumps
				//// Indices for jump to end
				Boxx::List<Boxx::UInt> endJumps;

				////T Expr Jumps
				//// Indicies for jump to case contents
				Boxx::List<Boxx::List<Boxx::UInt>> caseJumps;

				////T Expr Index
				//// The current case index
				Boxx::UInt caseIndex;

				////T Default jump
				//// The jump to the default case
				Boxx::UInt defaultJump;

				////T Result Argument
				Kiwi::Argument result;

				////T Result Node
				Boxx::Pointer<MemoryNode> resultNode;
			};

			virtual Symbols::TypeSymbol* Type() const override;
			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);

			///T Scan Setup
			/// Sets up the switch scan info
			SwitchScanInfo ScanSetup(ScanInfo& info) const;

			///T Scan Pre Contents
			/// A switch scan performed before the scan of the case content
			void ScanPreContents(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			///T Scan Post Contents
			/// A switch scan performed after the scan of the case content
			void ScanPostContents(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			///T Scan Cleanup
			/// Cleanup for the switch scan info
			void ScanCleanup(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			ScanResult ScanNodes(ScanInfoStack& info) const;

			void CompileCaseMatches(SwitchCompileInfo& switchInfo, CompileInfo& info);
			void CompileCaseBodies(SwitchCompileInfo& switchInfo, CompileInfo& info);
			void CompileDefault(SwitchCompileInfo& switchInfo, CompileInfo& info);
		};
	}
}