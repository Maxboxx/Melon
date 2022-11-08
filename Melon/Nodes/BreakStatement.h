#pragma once

#include "Statement.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Statement for breaking loops.
		class BreakStatement : public Statement {
		public:
			///i {true} for {break}.
			///i {false} for {abort}.
			bool isBreak;

			/// The number of loops to break.
			Boxx::UInt loops;

			/// The boolean argument to {break}.
			bool breakBool;

			/// {true} if the break is scopewise.
			bool scopewise = false;

			BreakStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~BreakStatement();

			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static Boxx::String abortInstName;
			static Boxx::String scopeBreakInstName;
			static Boxx::String breakTrueInstName;
			static Boxx::String breakFalseInstName;
		};
	}
}