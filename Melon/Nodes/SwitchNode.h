#pragma once

#include "Node.h"

#include "Boxx/Math.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		class MemoryNode;

		/// Node for both {switch} statements and {switch} expressions.
		class SwitchNode : public Node {
		public:
			/// {true} if the {switch} is an expression.
			bool expr;

			/// The match expression.
			NodePtr match;

			/// The nodes for all the cases.
			Boxx::List<NodePtr> nodes;

			/// All case expressions.
			Boxx::List<Boxx::List<NodePtr>> cases;

			/// The default node.
			NodePtr def;

			SwitchNode(Symbols::Symbol* const scope, const FileInfo& file);
			~SwitchNode();

			/// Scan info for switch nodes.
			struct SwitchScanInfo {
				/// {true} if the {switch} is in a constructor.
				bool init = false;
				
				/// Scope info.
				ScopeInfo scope;

				/// Scope info for all cases.
				Boxx::List<ScopeInfo> cases;

				/// {true} if at least one case is guaranteed to run.
				bool willACaseRun = false;
			};

			/// Compile info for {switch} nodes.
			struct SwitchCompileInfo {
				/// The compiled node.
				CompiledNode cn;

				/// The memory location of the match value.
				Boxx::Pointer<MemoryNode> match;

				/// Indices for jump to end.
				Boxx::List<Boxx::UInt> endJumps;

				/// Indicies for jump to case contents.
				Boxx::List<Boxx::List<Boxx::UInt>> caseJumps;

				/// The current case index.
				Boxx::UInt caseIndex;

				/// The jump to the default case.
				Boxx::UInt defaultJump;

				/// Result argument.
				Kiwi::Argument result;

				/// Result node.
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
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

			SwitchScanInfo ScanSetup(ScanInfo& info) const;

			void ScanPreContents(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			void ScanPostContents(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			void ScanCleanup(SwitchScanInfo& switchInfo, ScanInfo& info) const;

			ScanResult ScanNodes(ScanInfoStack& info) const;

			void CompileCaseMatches(SwitchCompileInfo& switchInfo, CompileInfo& info);
			void CompileCaseBodies(SwitchCompileInfo& switchInfo, CompileInfo& info);
			void CompileDefault(SwitchCompileInfo& switchInfo, CompileInfo& info);
		};
	}
}
