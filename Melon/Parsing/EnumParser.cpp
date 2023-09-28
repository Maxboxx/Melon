#include "EnumParser.h"

#include "FunctionParser.h"
#include "IntegerParser.h"
#include "TypeParser.h"
#include "TemplateParser.h"

#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/IntegerSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"

#include "Melon/Symbols/Nodes/EnumAssignNode.h"
#include "Melon/Symbols/Nodes/IntegerConvertNode.h"
#include "Melon/Symbols/Nodes/EnumCompareNode.h"

#include "Melon/Nodes/EmptyStatement.h"
#include "Melon/Nodes/FunctionBody.h"
#include "Melon/Nodes/Integer.h"

#include "Kiwi/Old/Kiwi.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Parsing;

Ptr<EnumStatement> EnumParser::Parse(ParsingInfo& info) {
	static Regex lower = Regex("^%l");
	static Regex underscore = Regex("%a_+%a");

	if (info.Current().type != TokenType::Enum) return nullptr;

	const UInt enumLine = info.Current().line;
	info.index++;

	MapSymbol* temp = info.scope;

	Ptr<EnumStatement> en = ParseName(info, enumLine);

	EnumSymbol* enumSymbol = en->symbol;

	info.scope = enumSymbol;

	List<EnumValue> values = ParseValues(info);

	for (const EnumValue& value : values) {
		ValueSymbol* v = new ValueSymbol(info.GetFileInfo(value.line));
		v->value = value.value;
		v->type = value.type;

		enumSymbol->AddSymbol(value.name, v);
		enumSymbol->members.Add(value.name);
		en->vars.Add(value.name);
	}

	while (FunctionParser::Parse(info, enumSymbol));

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "enum", enumLine), info.GetFileInfoPrev());
		info.scope = temp;
		return nullptr;
	}

	FunctionSymbol* const assign  = enumSymbol->AddSymbol(Name::Assign, new FunctionSymbol(info.GetFileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	assign1->arguments.Add(enumSymbol->AbsoluteName());
	assign1->symbolNode = new EnumAssignNode();

	FunctionSymbol* const eq  = enumSymbol->AddSymbol(Name::Equal, new FunctionSymbol(info.GetFileInfo()));
	FunctionSymbol* const eq1 = eq->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	eq1->arguments.Add(enumSymbol->AbsoluteName());
	eq1->arguments.Add(enumSymbol->AbsoluteName());
	eq1->returnValues.Add(NameList::Bool);
	eq1->symbolNode = new EnumCompareNode(Name::Equal);

	FunctionSymbol* const ne  = enumSymbol->AddSymbol(Name::NotEqual, new FunctionSymbol(info.GetFileInfo()));
	FunctionSymbol* const ne1 = ne->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	ne1->arguments.Add(enumSymbol->AbsoluteName());
	ne1->arguments.Add(enumSymbol->AbsoluteName());
	ne1->returnValues.Add(NameList::Bool);
	ne1->symbolNode = new EnumCompareNode(Name::NotEqual);

	info.index++;

	info.scope = temp;
	return en;
}

List<EnumParser::EnumValue> EnumParser::ParseValues(ParsingInfo& info) {
	List<EnumValue> values;
	ULong currentValue = 0;

	if (Optional<EnumValue> value = ParseValue(info, currentValue)) {
		values.Add(*value);

		while (info.Current().type == TokenType::Comma) {
			info.index++;

			if (Optional<EnumValue> value = ParseValue(info, currentValue)) {
				values.Add(*value);
			}
			else {
				ErrorLog::Error(LogMessage("error.syntax.expected.after", "name", LogMessage::Quote(",")), info.GetFileInfoPrev());
				break;
			}
		}
	}

	return values;
}

Optional<EnumParser::EnumValue> EnumParser::ParseValue(ParsingInfo& info, ULong& currentValue) {
	if (info.Current().type != TokenType::Name) return nullptr;

	EnumValue value;
	value.name = Name(info.Current().value);
	value.line = info.Current().line;

	info.index++;

	if (info.Current().type == TokenType::ParenOpen) {
		info.index++;

		if (Optional<NameList> type = TypeParser::Parse(info)) {
			value.type = type;
		}
		else {
			// TODO: error
			ErrorLog::Error(LogMessage("enum type error"), info.GetFileInfo());
		}

		if (info.Current().type == TokenType::ParenClose) {
			info.index++;
		}
		else {
			// TODO: error
			ErrorLog::Error(LogMessage("enum type error"), info.GetFileInfo());
		}
	}

	if (info.Current().type == TokenType::Assign) {
		info.index++;

		if (Ptr<Integer> num = IntegerParser::Parse(info)) {
			value.value = num->number;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "integer", LogMessage::Quote("=")), info.GetFileInfoPrev());
		}
	}
	else {
		// TODO: pick unique value
		value.value = currentValue++;
	}

	return value;
}

Ptr<EnumStatement> EnumParser::ParseName(ParsingInfo& info, const UInt enumLine) {
	static Regex lower = Regex("^%l");
	static Regex underscore = Regex("%a_+%a");

	if (info.Current().type != TokenType::Name) {
		ErrorLog::Error(LogMessage("error.syntax.expected.name.struct"), info.GetFileInfo(enumLine));
	}

	Name enumName = Name(info.Current().value);

	if (lower.Match(info.Current().value)) {
		ErrorLog::Info(LogMessage("info.name.upper", "struct", info.Current().value), info.GetFileInfo());
	}

	if (underscore.Match(info.Current().value)) {
		ErrorLog::Info(LogMessage("info.name.under", "struct", info.Current().value), info.GetFileInfo());
	}

	info.index++;
	EnumSymbol* sym;
	bool redefine = false;

	if (Symbol* const s = info.scope->Contains(enumName)) {
		sym = s->Cast<EnumSymbol>();
		redefine = true;
	}
	else {
		sym = info.scope->AddSymbol(enumName, new EnumSymbol(info.GetFileInfo(enumLine)));
	}

	if (Optional<List<NameList>> templateList = TemplateParser::ParseDefine(info)) {
		EnumSymbol* tsym = new EnumSymbol(info.GetFileInfo(enumLine));

		for (const NameList& arg : *templateList) {
			if (arg[0].IsEmpty()) {
				tsym->AddSymbol(arg[1], new TemplateSymbol(info.GetFileInfo(enumLine)));
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
		info.scope->AddSymbol(enumName, sym);
	}

	Ptr<EnumStatement> sn = new EnumStatement(info.scope, info.GetFileInfo(enumLine));
	sn->name = enumName;
	sn->symbol = sym;
	sym->node = sn;

	return sn;
}