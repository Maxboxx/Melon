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

			///T Segments
			/// All segments in the loop structure
			Boxx::List<LoopSegment> segments;

			LoopNode(const Symbols::ScopeList& scope, const FileInfo& file);
			~LoopNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual Boxx::Set<ScanType> Scan(ScanInfo& info) const override;
			virtual Boxx::Mango ToMango() const override;
		};
	}
}