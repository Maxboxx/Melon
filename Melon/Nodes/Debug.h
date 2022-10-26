#pragma once

#include "Statement.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for a debug statement.
		class Debug : public Statement {
		public:
			/// The type of the debug statement.
			Boxx::String type;

			/// The argument.
			Ptr<Expression> argument;

			Debug(Symbols::Symbol* const scope, const FileInfo& file);

			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}