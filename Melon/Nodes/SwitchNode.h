#pragma once

#include "Node.h"

#include "Boxx/Math.h"

namespace Melon {
	namespace Nodes {
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

			SwitchNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~SwitchNode();

			struct SwitchScanInfo {
				Boxx::List<Symbols::Scope> unassignedVarsStart;
				Boxx::Set<Symbols::Scope> unassignedVars;

				bool init          = false;
				bool hasReturned   = false;
				bool hasAReturn    = false;
				bool willNotReturn = true;

				bool isFirst = true;
				bool willACaseRun = false;

				Boxx::UInt loopBreakCount  = 0;
				Boxx::UInt scopeBreakCount = 0;

				Boxx::UInt minLoopBreakCount  = Boxx::Math::UIntMax();
				Boxx::UInt minScopeBreakCount = Boxx::Math::UIntMax();
			};

			///T Scan Setup
			/// Sets up the switch scan info
			SwitchScanInfo ScanSetup(ScanInfo& info) const;

			///T Scan Pre Contents
			/// A switch scan performed before the scan of the case content
			void ScanPreContents(SwitchScanInfo& loopInfo, ScanInfo& info) const;

			///T Scan Post Contents
			/// A switch scan performed after the scan of the case content
			void ScanPostContents(SwitchScanInfo& loopInfo, ScanInfo& info) const;

			///T Scan Cleanup
			/// Cleanup for the switch scan info
			void ScanCleanup(SwitchScanInfo& loopInfo, ScanInfo& info) const;

			virtual Symbols::ScopeList Type() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}