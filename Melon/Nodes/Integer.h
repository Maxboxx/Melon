#pragma once

#include "Expression.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for an integer.
		class Integer : public Expression {
		public:
			/// The integer value.
			Boxx::Long number;

			/// {true} if the integer is unsigned.
			bool isUnsigned = true;

			Integer(const FileInfo& file);
			~Integer();

			virtual Symbols::TypeSymbol* Type() const override;
			virtual bool IsImmediate() const override;
			virtual Boxx::Long GetImmediate() const override;
			virtual CompiledNode Compile(OldCompileInfo& info) override;
			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;
			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
		};
	}
}