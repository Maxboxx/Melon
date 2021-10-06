#pragma once

#include "StatementNode.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for breaking or aborting loops.
		class BreakNode : public StatementNode {
		public:
			///[item] {true} for {break}.
			///[item] {false} for {abort}.
			bool isBreak;

			/// The number of loops to break.
			Boxx::UInt loops;

			/// The boolean argument to {break}.
			bool breakBool;

			/// {true} if the break is scopewise.
			bool scopewise = false;

			BreakNode(Symbols::Symbol* const scope, const FileInfo& file);
			~BreakNode();

			virtual CompiledNode Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static Boxx::String abortInstName;
			static Boxx::String scopeBreakInstName;
			static Boxx::String breakTrueInstName;
			static Boxx::String breakFalseInstName;
		};
	}
}