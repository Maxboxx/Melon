#include "EnumParser.h"

#include "NewVariableParser.h"
#include "FunctionParser.h"
#include "IntegerParser.h"

#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Symbols/Nodes/IntegerAssignNode.h"
#include "Melon/Symbols/Nodes/IntegerConvertNode.h"
#include "Melon/Symbols/Nodes/IntegerBinaryOperatorNode.h"

#include "Melon/Nodes/NewVariableNode.h"
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

	if (info.Current().type != TokenType::Name) {
		ErrorLog::Error(LogMessage("error.syntax.expected.name.enum"), info.GetFileInfo(enumLine));
	}

	const Name enumName = Name(info.Current().value);

	if (lower.Match(info.Current().value)) {
		ErrorLog::Info(LogMessage("info.name.upper", "enum", info.Current().value), info.GetFileInfo());
	}

	if (underscore.Match(info.Current().value)) {
		ErrorLog::Info(LogMessage("info.name.under", "enum", info.Current().value), info.GetFileInfo());
	}

	info.index++;

	// TODO: size
	EnumSymbol* enumSymbol = new EnumSymbol(1, false, info.GetFileInfo(enumLine));

	Ptr<EnumStatement> en = new EnumStatement(info.scope, info.GetFileInfo(enumLine));

	en->name = enumName;

	enumSymbol = info.scope->AddSymbol(enumName, enumSymbol);
	en->symbol = enumSymbol;

	info.scope = enumSymbol;

	List<EnumValue> values = ParseValues(info);

	for (const EnumValue& value : values) {
		ValueSymbol* v = new ValueSymbol(info.GetFileInfo(value.line));
		v->value = value.value;

		enumSymbol->AddSymbol(value.name, v);
		en->values.Add(value.name);
	}

	while (FunctionParser::Parse(info, enumSymbol));

	if (info.Current().type != TokenType::End) {
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "enum", enumLine), info.GetFileInfoPrev());
	}

	FunctionSymbol* const assign  = enumSymbol->AddSymbol(Name::Assign, new FunctionSymbol(info.GetFileInfo()));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	assign1->arguments.Add(enumSymbol->AbsoluteName());
	assign1->symbolNode = new IntegerAssignNode(enumSymbol->Size());

	FunctionSymbol* const eq  = enumSymbol->AddSymbol(Name::Equal, new FunctionSymbol(info.GetFileInfo()));
	FunctionSymbol* const eq1 = eq->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	eq1->arguments.Add(enumSymbol->AbsoluteName());
	eq1->arguments.Add(enumSymbol->AbsoluteName());
	eq1->returnValues.Add(NameList::Bool);
	eq1->symbolNode = new IntegerBinaryOperatorNode(Name::Equal, enumSymbol->KiwiType());

	FunctionSymbol* const ne  = enumSymbol->AddSymbol(Name::NotEqual, new FunctionSymbol(info.GetFileInfo()));
	FunctionSymbol* const ne1 = ne->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	ne1->arguments.Add(enumSymbol->AbsoluteName());
	ne1->arguments.Add(enumSymbol->AbsoluteName());
	ne1->returnValues.Add(NameList::Bool);
	ne1->symbolNode = new IntegerBinaryOperatorNode(Name::NotEqual, enumSymbol->KiwiType());

	FunctionSymbol* const as  = enumSymbol->AddSymbol(Name::As, new FunctionSymbol(info.GetFileInfo()));
	FunctionSymbol* const as1 = as->AddOverload(new FunctionSymbol(info.GetFileInfo()));
	as1->arguments.Add(enumSymbol->AbsoluteName());
	as1->returnValues.Add(enumSymbol->AbsoluteName());
	as1->isExplicit = false;

	IntegerConvertNode* const cn = new IntegerConvertNode();
	cn->size = 1;
	cn->sign = false;
	cn->targetSize = 1;

	as1->symbolNode = cn;

	info.index++;

	info.scope = info.scope->Parent<ScopeSymbol>();
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

	if (info.Next().type == TokenType::Assign) {
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
