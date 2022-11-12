#pragma once

#include "Statement.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for continuing loops.
		class ContinueStatement : public Statement {
		public:
			/// The number of loops to continue.
			Boxx::UInt loops;

			ContinueStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~ContinueStatement();

			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			static Boxx::String continueInstName;
		};
	}
}