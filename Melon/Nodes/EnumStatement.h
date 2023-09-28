#pragma once

#include "Statement.h"

namespace Melon {
	namespace Symbols {
		class EnumSymbol;
	}

	///N Melon::Nodes
	namespace Nodes {
		/// Node for enums.
		class EnumStatement : public Statement {
		public:
			/// The name of the enum.
			Symbols::Name name;

			/// The values of the enum.
			Boxx::List<Symbols::Name> vars;

			/// The enum symbol.
			Symbols::EnumSymbol* symbol;

			EnumStatement(Symbols::Symbol* const scope, const FileInfo& file);
			~EnumStatement();

			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}