#pragma once

#include "Node.h"

namespace Melon {
	namespace Nodes {

		///B LoopNode
		/// Node for loop structures
		class LoopNode : public Node {
		public:

			////B LoopType
			enum class LoopType : Boxx::UByte {
				////T Values
				////M
				None,
				If,
				While,
				For
				////M
			};

			////B LoopSegment
			//// Contains info about a loop segment
			struct LoopSegment {
				////T Type
				LoopType type;

				////T Also
				//// <key>true</key> if the segment is an also-segment
				bool also;

				////T Condition
				//// The condition of the segment
				NodePtr condition;

				////T Statements
				//// The contents of the segment
				NodePtr statements;

				~LoopSegment() {}
			};

			struct LoopScanInfo {
				Boxx::List<Symbols::Scope> unassignedVarsStart;
				Boxx::Set<Symbols::Scope> unassignedVarsAlso;
				Boxx::Set<Symbols::Scope> unassignedVars;
				bool init = false;

				bool checkAlsoAssign = false;
				bool willASegmentRun = false;

				bool ret = false;
				bool retExists = false;
				bool checkAlsoRet = false;
			};

			///T Segments
			/// All segments in the loop structure
			Boxx::List<LoopSegment> segments;

			LoopNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~LoopNode();

			///T Will A Segment Run
			/// Returns <code>true</code> if at least one segment will run
			bool WillASegmentRun() const;

			///T Scan Setup
			/// Sets up the loop scan info
			LoopScanInfo ScanSetup(ScanInfo& info) const;
				
			///T Scan First Post Contents
			/// A loop scan performed on the first segment
			void ScanFirstPostContents(LoopScanInfo& loopInfo, ScanInfo& info) const;

			///T Scan Pre Contents
			/// A loop scan performed before the scan of the loop segment content
			void ScanPreContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const;

			///T Scan Post Contents
			/// A loop scan performed after the scan of the loop segment content
			void ScanPostContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const;

			///T Scan Cleanup
			/// Cleanup for the loop scan info
			void ScanCleanup(LoopScanInfo& loopInfo, ScanInfo& info) const;

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}