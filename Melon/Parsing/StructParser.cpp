#include "StructParser.h"

#include "NewVariableParser.h"
#include "FunctionParser.h"
#include "TemplateParser.h"

#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"

#include "Melon/Symbols/Nodes/StructAssignNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Parsing;

Ptr<StructStatement> StructParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Struct) return nullptr;

	const UInt structLine = info.Current().line;
	info.index++;
	MapSymbol* const temp = info.scope;

	Ptr<StructStatement> sn = ParseName(info, structLine);
	info.scope = sn->symbol;

	while (true) {
		bool found = false;

		if (Ptr<NewVariableNode> nn = ParseVariable(info)) {
			for (UInt i = 0; i < nn->names.Count(); i++) {
				VariableSymbol* const var = new VariableSymbol(info.GetFileInfo());

				if (nn->types.Count() == 1)
					var->type = nn->types[0];
				else
					var->type = nn->types[i];

				var->modifiers = nn->modifiers[i];
				sn->symbol->AddSymbol(nn->names[i], var);
				sn->symbol->members.Add(nn->names[i]);
			}

			found = true;
		}
		else if (FunctionParser::Parse(info, sn->symbol)) {
			found = true;
		}

		if (!found) break;
	}

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "struct", structLine), info.GetFileInfoPrev());
		info.scope = temp;
		return nullptr;
	}

	FunctionSymbol* const assign = new FunctionSymbol(info.GetFileInfo());
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	assign1->arguments.Add(sn->symbol->AbsoluteName());
	assign1->symbolNode = new StructAssignNode();
	sn->symbol->AddSymbol(Name::Assign, assign);

	info.index++;
	info.scope = temp;
	return sn;
}

Ptr<StructStatement> StructParser::ParseName(ParsingInfo& info, const UInt structLine) {
	static Regex lower = Regex("^%l");
	static Regex underscore = Regex("%a_+%a");

	if (info.Current().type != TokenType::Name) {
		ErrorLog::Error(LogMessage("error.syntax.expected.name.struct"), info.GetFileInfo(structLine));
	}

	Name structName = Name(info.Current().value);

	if (lower.Match(info.Current().value)) {
		ErrorLog::Info(LogMessage("info.name.upper", "struct", info.Current().value), info.GetFileInfo());
	}

	if (underscore.Match(info.Current().value)) {
		ErrorLog::Info(LogMessage("info.name.under", "struct", info.Current().value), info.GetFileInfo());
	}

	info.index++;
	StructSymbol* sym;
	bool redefine = false;
	
	if (Symbol* const s = info.scope->Contains(structName)) {
		sym = s->Cast<StructSymbol>();
		redefine = true;
	}
	else {
		sym = info.scope->AddSymbol(structName, new StructSymbol(info.GetFileInfo(structLine)));
	}
	
	if (Optional<List<NameList>> templateList = TemplateParser::ParseDefine(info)) {
		StructSymbol* tsym = new StructSymbol(info.GetFileInfo(structLine));

		for (const NameList& arg : *templateList) {
			if (arg[0].IsEmpty()) {
				tsym->AddSymbol(arg[1], new TemplateSymbol(info.GetFileInfo(structLine)));
			}

			tsym->templateArguments.Add(arg);
		}

		Name templateScope = Name("");
		templateScope.types = *templateList;

		sym->AddTemplateVariant(tsym);
		sym = tsym;
		info.statementNumber++;
	}
	else if (redefine) {
		info.scope->AddSymbol(structName, sym);
	}

	Ptr<StructStatement> sn = new StructStatement(info.scope, info.GetFileInfo(structLine));
	sn->name = structName;
	sn->symbol = sym;
	sym->node = sn;

	return sn;
}

Ptr<NewVariableNode> StructParser::ParseVariable(ParsingInfo& info) {
	if (Ptr<NewVariableNode> node = NewVariableParser::Parse(info)) {
		return node;
	}

	return nullptr;
}