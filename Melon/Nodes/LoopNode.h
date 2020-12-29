#pragma once

#include "Boxx/Tuple.h"

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

				bool IsLoop() const {
					return type == LoopType::While || type == LoopType::For;
				}
			};

			struct LoopScanInfo {
				ScopeInfo scope;
				ScopeInfo mainSegment;
				Boxx::List<ScopeInfo> alsoSegments;
				Boxx::List<ScopeInfo> elseSegments;

				bool init            = false;
				bool willASegmentRun = false;

				~LoopScanInfo() {}
			};

			///T Segments
			/// All segments in the loop structure
			Boxx::List<LoopSegment> segments;

			LoopNode(Symbols::Symbol* const scope, const FileInfo& file);
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

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::ScopeList FindSideEffectScope(const bool assign);

		private:
			///T Get Next Segments
			/// Gets the index for the next true and false segment
			void GetNextSegments(const Boxx::UInt segment, Boxx::UInt& nextTrue, Boxx::UInt& nextFalse) const;

			///T Is Segment Last
			/// Checks if the specified segment is the last also or else segment or is the only segment of the loop structure
			bool IsSegmentLast(const Boxx::UInt segment) const;

			struct SegmentInfo {
				Boxx::UInt index;
				Boxx::Array<Boxx::UInt> segmentLabels;
				Boxx::List<Boxx::Tuple<Boxx::UInt, Boxx::UInt>> jumps;
				Boxx::Long stackTop;
			};

			struct LoopInfo {
				Boxx::Long stack = 0;
				Boxx::UInt loopEndLbl = 0;
				Boxx::UInt loopEndJmp = 0;
				Boxx::UInt loopLbl = 0;
			};

			///T Add Label If Needed
			void AddLabelIfNeeded(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;

			///T Compile If Segment
			void CompileIfSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;

			///T Compile While Segment
			void CompileWhileSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;
			void CompileWhileStart(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;
			void CompileWhileEnd(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;

			///T Compile For Segment
			void CompileForSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;
			void CompileForStart(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;
			void CompileForEnd(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;
			
			void CompileLoopBody(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;

			///T Compile None Segment
			void CompileNoneSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;
		};
	}
}