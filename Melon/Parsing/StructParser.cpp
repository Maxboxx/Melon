#include "StructParser.h"

#include "FunctionParser.h"
#include "TemplateParser.h"
#include "AssignmentParser.h"
#include "TypeParser.h"
#include "MemberVariableParser.h"

#include "Melon/Nodes/NameExpression.h"

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

	Ptr<StructStatement> sn = ParseName(nullptr, info, structLine);
	
	return ParseBody(sn, info, structLine, false);
}

Ptr<StructStatement> StructParser::ParseCurly(const Name& name, ParsingInfo& info) {
	const UInt structLine = info.Current().line;

	Ptr<StructStatement> sn = ParseName(name, info, structLine);

	return ParseBody(sn, info, structLine, true);
}

Ptr<StructStatement> StructParser::ParseBody(Ptr<StructStatement> statement, ParsingInfo& info, const UInt structLine, bool curly) {
	if (curly) {
		if (info.Current().type != TokenType::CurlyOpen) {
			return nullptr;
		}

		info.index++;
	}

	MapSymbol* const temp = info.scope;
	info.scope = statement->symbol;

	while (true) {
		bool found = false;

		if (Optional<MemberVariableParser::Variables> nn = MemberVariableParser::Parse(info)) {
			for (UInt i = 0; i < nn->names.Count(); i++) {
				VariableSymbol* const var = new VariableSymbol(info.GetFileInfo());
				var->type = nn->types[i];

				Name name = nn->names[i];
				statement->symbol->AddSymbol(name, var);
				statement->symbol->members.Add(name);
			}

			found = true;
		}
		else if (FunctionParser::Parse(info, statement->symbol)) {
			found = true;
		}

		if (!found) break;
	}

	if (curly) {
		if (info.Current().type != TokenType::CurlyClose) {
			ErrorLog::Error(LogMessage("error.syntax.expected.close_at", LogMessage::Quote("}"), "struct", structLine), info.GetFileInfoPrev());
			info.scope = temp;
			return nullptr;
		}
	}
	else {
		if (info.Current().type != TokenType::End) {
			ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "struct", structLine), info.GetFileInfoPrev());
			info.scope = temp;
			return nullptr;
		}
	}

	FunctionSymbol* const assign = new FunctionSymbol(info.GetFileInfo());
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	assign1->arguments.Add(statement->symbol->AbsoluteName());
	assign1->symbolNode = new StructAssignNode();
	statement->symbol->AddSymbol(Name::Assign, assign);

	info.index++;
	info.scope = temp;
	return statement;
}

Ptr<StructStatement> StructParser::ParseName(Optional<Name> name, ParsingInfo& info, const UInt structLine) {
	static Regex lower = Regex("^%l");
	static Regex underscore = Regex("%a_+%a");

	const bool hasName = (bool)name;

	if (!name) {
		if (info.Current().type != TokenType::Name) {
			ErrorLog::Error(LogMessage("error.syntax.expected.name.struct"), info.GetFileInfo(structLine));
		}

		name = Name(info.Current().value);

		if (lower.Match(info.Current().value)) {
			ErrorLog::Info(LogMessage("info.name.upper", "struct", info.Current().value), info.GetFileInfo());
		}

		if (underscore.Match(info.Current().value)) {
			ErrorLog::Info(LogMessage("info.name.under", "struct", info.Current().value), info.GetFileInfo());
		}

		info.index++;
	}

	Name structName = *name;

	StructSymbol* sym;
	bool redefine = false;
	
	if (Symbol* const s = info.scope->Contains(structName)) {
		sym = s->Cast<StructSymbol>();
		redefine = true;
	}
	else {
		sym = info.scope->AddSymbol(structName, new StructSymbol(info.GetFileInfo(structLine)));
	}
	
	if (!hasName) {
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
	}

	Ptr<StructStatement> sn = new StructStatement(info.scope, info.GetFileInfo(structLine));
	sn->name = structName;
	sn->symbol = sym;
	sym->node = sn;

	return sn;
}

