#pragma once

#include "Statement.h"

///N Melon::Nodes
namespace Melon {
	namespace Nodes {
		/// Node for statements.
		class Statements final : public Node {
		public:
			/// The list of statements.
			Boxx::List<Ptr<Statement>> statements;

			Statements(Symbols::Symbol* const scope, const FileInfo& file);
			~Statements();

			virtual Boxx::UInt GetSize() const override;

			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			virtual Ptr<Statements> Optimize(OptimizeInfo& info);
			virtual CompiledNode Compile(OldCompileInfo& info) override;

			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;

			/// Create from statement.
			static Ptr<Statements> FromStatement(Ptr<Statement> statement);

		protected:
			virtual Symbols::NameList FindSideEffectScope(const bool assign);

		private:
			static bool HasSpaceAround(Weak<Statement> statement);
		};
	}
}
