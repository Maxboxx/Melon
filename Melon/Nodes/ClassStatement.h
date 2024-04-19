#pragma once

#include "Statement.h"

#include "Melon/Symbols/ClassSymbol.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for classes.
		class ClassStatement : public Statement {
		public:
			/// The name of the class.
			Symbols::Name name;

			/// The members of the class.
			Boxx::List<Symbols::Name> vars;

			/// The class symbol.
			Symbols::ClassSymbol* symbol;

			ClassStatement(Symbols::Symbol* const scope, const FileInfo& file);

			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			void CompileSymbol(Weak<Kiwi::Struct> struct_, Weak<Kiwi::Struct> vtable, Weak<Kiwi::StaticData> vdata, Symbols::ClassSymbol* sym);

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign) override;
		};
	}
}
