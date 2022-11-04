#pragma once

#include "Boxx/Tuple.h"

#include "Statement.h"
#include "Assignment.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for loop statements.
		class LoopStatement : public Statement {
		public:
			/// The different loop types.
			enum class LoopType : Boxx::UByte {
				/// Used for {also} and {else}.
				None,

				/// Used for {if}, {alif} and {elif}.
				If,

				/// Used for {while}, {alwhile} and {elwhile}.
				While,

				/// Used for {for}, {alfor} and {elfor}.
				For
			};

			/// Contains info about a loop segment.
			struct LoopSegment {
				/// The loop type of the segment.
				LoopType type;

				/// {true} if the segment is an {also} segment.
				bool also;

				/// The condition of the segment.
				Ptr<Condition> condition;

				/// The body of the segment.
				Ptr<Statements> statements;

				/// The init node of a for segment.
				Ptr<Assignment> init;

				/// The step node of a for segment.
				Ptr<Node> step;

				/// An optional condition operator used in for segments.
				///p The condition is automatically compared to the loop variable if this is set.
				Boxx::Optional<Symbols::Name> conditionOperator;

				/// An optional step operator used in for segments.
				///p The loop variable is automatically updated if this is set.
				Boxx::Optional<Symbols::Name> stepOperator;

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

			LoopStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~LoopStatement();

			/// {true} if at least one segment will run.
			bool WillASegmentRun() const;

			virtual Boxx::UInt GetSize() const override;
			virtual bool IsScope() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Statement> Optimize(OptimizeInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

		private:
			LoopScanInfo ScanSetup(ScanInfo& info) const;
			void ScanFirstPostContents(LoopScanInfo& loopInfo, ScanInfo& info) const;
			void ScanPreContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const;
			void ScanPostContents(LoopScanInfo& loopInfo, ScanInfo& info, const LoopSegment& segment) const;
			void ScanCleanup(LoopScanInfo& loopInfo, ScanInfo& info) const;
			ScanResult ScanForCondition(const LoopSegment& segment, ScanInfoStack& info);

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

			void AddLabelIfNeeded(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo) const;

			void CompileIfSegment(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo) const;

			void CompileWhileSegment(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo) const;
			void CompileWhileStart(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;
			void CompileWhileEnd(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;

			void CompileForSegment(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo) const;
			void CompileForStart(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;
			void CompileForEnd(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;
			
			void CompileLoopBody(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo, LoopInfo& loopInfo) const;

			void CompileNoneSegment(CompiledNode& compiled, OldCompileInfo& info, SegmentInfo& segmentInfo) const;
		};
	}
}
