#pragma once

#include "TemplateTypeSymbol.h"

///N Melon::Symbols
namespace Melon {
	namespace Nodes {
		class EnumStatement;
	}

	namespace Symbols {
		/// Contains information about an enum.
		class EnumSymbol : public TemplateTypeSymbol {
		public:
			/// A list of names for all values.
			Boxx::List<Symbols::Name> members;

			EnumSymbol(const FileInfo& file);
			~EnumSymbol();

			/// The size of the identifier.
			TypeSymbol* IdentifierType() const;

			/// The kiwi type for the value.
			Kiwi::Type ValueKiwiType() const;

			/// The size of the identifier.
			Boxx::UInt IdentifierSize() const;

			/// The size of the biggest value.
			Boxx::UInt ValueSize() const;

			/// True if the enum is a plain enum.
			bool IsPlain();

			virtual void UpdateSize();

			virtual EnumSymbol* InitializeSpecialize() override;
			virtual void SpecializeTemplate(Symbol* sym, const Boxx::ReplacementMap<TypeSymbol*>& replacement, Melon::Nodes::RootNode* const root) override;

			/// The enum statement for this symbol.
			Weak<Melon::Nodes::EnumStatement> node;
		};
	}
}
