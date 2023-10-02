#pragma once

#include "Expression.h"

#include "Boxx/Optional.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for arrays.
		class Array : public Expression {
		public:
			/// The array type.
			Boxx::Optional<Symbols::NameList> type;

			/// The array items.
			Boxx::List<Ptr<Expression>> items;

			Array(Symbols::Symbol* scope, const FileInfo& file);
			~Array();

			virtual Symbols::TypeSymbol* Type(Symbols::TypeSymbol* expected = nullptr) const override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

		private:
			Ptr<Kiwi::Value> CompileArray(Symbols::TemplateTypeSymbol* type, CompileInfo& info);
			Ptr<Kiwi::Value> CompileList(Symbols::TemplateTypeSymbol* type, CompileInfo& info);
		};
	}
}