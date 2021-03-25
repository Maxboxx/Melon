#pragma once

#include "Boxx/Tuple.h"

#include "Node.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for loop structures.
		class LoopNode : public Node {
		public:
			/// The different loop types.
			enum class LoopType : Boxx::UByte {
				/// Used for {also} and {else}.
				None,

				/// Used for {if}, {alsoif} and {elseif}.
				If,

				/// Used for {while}, {alsowhile} and {elsewhile}.
				While,

				/// Used for {for}, {alsofor} and {elsefor}.
				For
			};

			/// Contains info about a loop segment.
			struct LoopSegment {
				/// The loop type of the segment.
				LoopType type;

				/// {true} if the segment is an {also} segment.
				bool also;

				/// The condition of the segment.
				NodePtr condition;

				/// The body of the segment.
				NodePtr statements;

				~LoopSegment() {}

				/// Checks if the segment is a loop.
				bool IsLoop() const {
					return type == LoopType::While || type == LoopType::For;
				}
			};

			/// Scan info for loops.
			struct LoopScanInfo {
				/// The initial scope info.
				ScopeInfo scope;

				/// The scope info of the main loop segment.
				ScopeInfo mainSegment;

				/// The scope info for {also} segments.
				Boxx::List<ScopeInfo> alsoSegments;

				/// The scope info for {else} segments.
				Boxx::List<ScopeInfo> elseSegments;

				/// {true} if the loop is in a constructor.
				bool init = false;

				/// {true} if at least one segment is guaranteed to run.
				bool willASegmentRun = false;

				~LoopScanInfo() {}
			};

			/// All segments of the loop structure.
			Boxx::List<LoopSegment> segments;

			LoopNode(Symbols::Symbol* const scope, const FileInfo& file);
			~LoopNode();

			/// {true} if at least one segment will run.
			bool WillASegmentRun() const;

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual NodePtr Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

		private:
			LoopScanInfo ScanSetup(ScanInfo& info) const;
			void ScanFirstPostContents(LoopScanInfo& loopInfo, ScanInfo& info) const;
			void ScanPreContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const;
			void ScanPostContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const;
			void ScanCleanup(LoopScanInfo& loopInfo, ScanInfo& info) const;

			void GetNextSegments(const Boxx::UInt segment, Boxx::UInt& nextTrue, Boxx::UInt& nextFalse) const;
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

			void AddLabelIfNeeded(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;

			void CompileIfSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;

			void CompileWhileSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;
			void CompileWhileStart(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;
			void CompileWhileEnd(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;

			void CompileForSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;
			void CompileForStart(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;
			void CompileForEnd(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;
			
			void CompileLoopBody(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;

			void CompileNoneSegment(CompiledNode& compiled, CompileInfo& info, SegmentInfo& segmentInfo) const;
		};
	}
}
