#include "ClassParser.h"

#include "FunctionParser.h"
#include "TemplateParser.h"
#include "AssignmentParser.h"
#include "TypeParser.h"
#include "MemberVariableParser.h"

#include "Melon/Nodes/NameExpression.h"

#include "Melon/Symbols/ClassSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"

#include "Melon/Symbols/Nodes/ClassAssignNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Parsing;

Ptr<ClassStatement> ClassParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Class) return nullptr;

	const UInt structLine = info.Current().line;
	info.index++;
	MapSymbol* const temp = info.scope;

	Ptr<ClassStatement> sn = ParseName(info, structLine);
	info.scope = sn->symbol;

	while (true) {
		bool found = false;

		if (Optional<MemberVariableParser::Variables> nn = MemberVariableParser::Parse(info)) {
			for (UInt i = 0; i < nn->names.Count(); i++) {
				VariableSymbol* const var = new VariableSymbol(info.GetFileInfo());
				var->type = nn->types[i];

				Name name = nn->names[i];
				sn->symbol->AddSymbol(name, var);
				sn->symbol->members.Add(name);
			}

			found = true;
		}
		else if (FunctionParser::Parse(info, sn->symbol)) {
			found = true;
		}

		if (!found) break;
	}

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "class", structLine), info.GetFileInfoPrev());
		info.scope = temp;
		return nullptr;
	}

	FunctionSymbol* const assign = new FunctionSymbol(info.GetFileInfo());
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	assign1->arguments.Add(sn->symbol->AbsoluteName());
	assign1->symbolNode = new ClassAssignNode();
	sn->symbol->AddSymbol(Name::Assign, assign);

	info.index++;
	info.scope = temp;
	return sn;
}

Ptr<ClassStatement> ClassParser::ParseName(ParsingInfo& info, const UInt structLine) {
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
	ClassSymbol* sym;
	bool redefine = false;
	
	if (Symbol* const s = info.scope->Contains(structName)) {
		sym = s->Cast<ClassSymbol>();
		redefine = true;
	}
	else {
		sym = info.scope->AddSymbol(structName, new ClassSymbol(info.GetFileInfo(structLine)));
	}
	
	if (Optional<List<NameList>> templateList = TemplateParser::ParseDefine(info)) {
		ClassSymbol* tsym = new ClassSymbol(info.GetFileInfo(structLine));

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

	Ptr<ClassStatement> sn = new ClassStatement(info.scope, info.GetFileInfo(structLine));
	sn->name = structName;
	sn->symbol = sym;
	sym->node = sn;

	return sn;
}