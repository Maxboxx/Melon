#include "EnumParser.h"

#include "NewVariableParser.h"
#include "FunctionParser.h"
#include "IntegerParser.h"

#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"

#include "Melon/Symbols/Nodes/IntegerAssignNode.h"
#include "Melon/Symbols/Nodes/IntegerBinaryOperatorNode.h"

#include "Melon/Nodes/EnumNode.h"
#include "Melon/Nodes/NewVariableNode.h"
#include "Melon/Nodes/EmptyNode.h"
#include "Melon/Nodes/FunctionNode.h"
#include "Melon/Nodes/IntegerNode.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Parsing;

NodePtr EnumParser::Parse(ParsingInfo& info) {
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

	Pointer<EnumNode> en = new EnumNode(info.scope, info.GetFileInfo(enumLine));

	en->name = enumName;

	enumSymbol = info.scope->AddSymbol(enumName, enumSymbol);
	en->symbol = enumSymbol;

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

	FunctionSymbol* const assign = new FunctionSymbol(info.GetFileInfo());
	assign->arguments.Add(enumSymbol->AbsoluteName());
	assign->symbolNode = new IntegerAssignNode(enumSymbol->Size());
	enumSymbol->AddSymbol(Name::Assign, assign);

	FunctionSymbol* const eq = new FunctionSymbol(info.GetFileInfo());
	eq->arguments.Add(enumSymbol->AbsoluteName());
	eq->arguments.Add(enumSymbol->AbsoluteName());
	eq->returnValues.Add(NameList::Bool);
	eq->symbolNode = new IntegerBinaryOperatorNode(enumSymbol->Size(), enumSymbol->IsSigned(), InstructionType::Eq);
	enumSymbol->AddSymbol(Name::Equal, eq);

	FunctionSymbol* const ne = new FunctionSymbol(info.GetFileInfo());
	ne->arguments.Add(enumSymbol->AbsoluteName());
	ne->arguments.Add(enumSymbol->AbsoluteName());
	ne->returnValues.Add(NameList::Bool);
	ne->symbolNode = new IntegerBinaryOperatorNode(enumSymbol->Size(), enumSymbol->IsSigned(), InstructionType::Ne);
	enumSymbol->AddSymbol(Name::NotEqual, ne);

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

		if (NodePtr num = IntegerParser::Parse(info)) {
			value.value = num.Cast<IntegerNode>()->number;
		}
		else {
			ErrorLog::Error(LogMessage("error.syntax.expected.after", "integer", LogMessage::Quote("=")), info.GetFileInfoPrev());
		}
	}
	else {
		value.value = currentValue++;
	}

	return value;
}
