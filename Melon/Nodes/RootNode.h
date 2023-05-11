#pragma once

#include "Node.h"
#include "Statements.h"
#include "ClassStatement.h"
#include "StructStatement.h"

#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/ClassStructBaseSymbol.h"

namespace Melon {
	struct CompilerOptions;

	///N Melon::Nodes
	namespace Nodes {
		class FunctionBody;

		/// The root node of the compiled program.
		class RootNode final : public Node {
		public:
			/// All statements in the main scope of the program.
			Boxx::List<Ptr<Statements>> nodes;

			/// All functions in the program.
			Boxx::List<Ptr<FunctionBody>> funcs;

			/// The id for the next function.
			Boxx::UInt funcId = 0;

			RootNode();
			~RootNode();

			virtual Ptr<Kiwi::Value> Compile(CompileInfo& info) override;

			virtual Boxx::StringBuilder ToMelon(const Boxx::UInt indent) const override;
			virtual void IncludeScan(Parsing::ParsingInfo& info) override;
			virtual ScanResult Scan(ScanInfoStack& info) override;
			void Optimize(OptimizeInfo& info);

			/// Adds a template specialization.
			void AddTemplateSpecialization(const Symbols::NameList& name, const Symbols::NameList& scope, const FileInfo& file, const bool scan = true);

			/// Scans for errors.
			ScanInfoStack Scan();

			/// Writes the optimized code to melon files.
			void ToMelonFiles(const CompilerOptions& options) const;

			/// The current parsing info.
			Parsing::ParsingInfo* parsingInfo = nullptr;

		private:
			Boxx::Tuple<Symbols::TemplateTypeSymbol*, Boxx::List<Symbols::NameList>> FindTemplateArgs(const Symbols::NameList& name, const Symbols::NameList& scope, const FileInfo& file);

			template <class Sym, class Stat>
			void SpecializeClassStruct(Sym* sym, Symbols::TemplateTypeSymbol* templateSym, const Boxx::ReplacementMap<Symbols::TypeSymbol*>& templateTypes) {
				templateSym->Parent()->Cast<Symbols::TemplateTypeSymbol>()->AddTemplateVariant(sym);
				templateSym->SpecializeTemplate(sym, templateTypes, this);

				sym->templateParent = templateSym;

				Ptr<Stat> sn = new Stat(Symbols::SymbolTable::FindAbsolute(Symbols::NameList(true), file), file);
				sn->name = sym->Parent()->Name();

				Boxx::List<Symbols::NameList> templateArgs;

				for (const Symbols::NameList& arg : sym->templateArguments) {
					templateArgs.Add(arg);
				}

				sn->name.types = templateArgs;

				sn->symbol = sym;
				sym->node = sn;

				for (const Symbols::Name& var : sym->members) {
					sn->vars.Add(var);
				}

				nodes.Add(Statements::FromStatement(sn));
			}

			Boxx::UInt nodeIndex = 0;
			Boxx::UInt funcIndex = 0;
			Boxx::UInt templateIndex = 0;

			bool includeScanning = false;
		};
	}
}