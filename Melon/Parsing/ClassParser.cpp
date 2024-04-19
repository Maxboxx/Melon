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
#include "Melon/Symbols/Nodes/ClassCastNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Parsing;

Ptr<ClassStatement> ClassParser::Parse(ParsingInfo& info) {
	const UInt startIndex = info.index;

	bool abstract = false;

	if (info.Current().type == TokenType::Abst) {
		abstract = true;
		info.index++;
	}

	if (info.Current().type != TokenType::Class) {
		info.index = startIndex;
		return nullptr;
	}

	const UInt structLine = info.Current().line;
	info.index++;
	MapSymbol* const temp = info.scope;

	Ptr<ClassStatement> sn = ParseName(info, structLine);
	info.scope = sn->symbol;
	sn->symbol->abstract = abstract;

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

	ClassSymbol* base = sn->symbol;

	while (base = base->BaseClass()) {
		FunctionSymbol* const assign2 = base->Contains<FunctionSymbol>(Name::Assign)->AddOverload(new FunctionSymbol(info.GetFileInfo()));
		assign2->arguments.Add(sn->symbol->AbsoluteName());
		assign2->symbolNode = new ClassAssignNode();

		FunctionSymbol* convert = base->Contains<FunctionSymbol>(Name::As);

		if (!convert) {
			convert = base->AddSymbol(Name::As, new FunctionSymbol(info.GetFileInfo()));
		}

		FunctionSymbol* const convert1 = convert->AddOverload(new FunctionSymbol(info.GetFileInfo()));
		convert1->arguments.Add(base->AbsoluteName());
		convert1->returnValues.Add(sn->symbol->AbsoluteName());
		convert1->isExplicit = true;
		convert1->symbolNode = new ClassCastNode(sn->symbol);
	}

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
		ErrorLog::Info(LogMessage("info.name.upper", "class", info.Current().value), info.GetFileInfo());
	}

	if (underscore.Match(info.Current().value)) {
		ErrorLog::Info(LogMessage("info.name.under", "class", info.Current().value), info.GetFileInfo());
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

	if (info.Current().type == TokenType::Ext) {
		info.index++;

		if (Optional<NameList> base = TypeParser::Parse(info)) {
			sym->baseClass = base;
		}
		else {
			ErrorLog::Error(LogMessage("No base class"), info.GetFileInfoPrev());
			return nullptr;
		}
	}

	Ptr<ClassStatement> sn = new ClassStatement(info.scope, info.GetFileInfo(structLine));
	sn->name = structName;
	sn->symbol = sym;
	sym->node = sn;

	return sn;
}