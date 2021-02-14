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

	if (info.Current().type != TokenType::Name)
		ErrorLog::Error(SyntaxError(SyntaxError::EnumName, FileInfo(info.filename, enumLine, info.statementNumber)));

	const Scope enumName = Scope(info.Current().value);

	if (lower.Match(info.Current().value)) {
		ErrorLog::Info(InfoError(InfoError::UpperName("enum", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber)));
	}

	if (underscore.Match(info.Current().value)) {
		ErrorLog::Info(InfoError(InfoError::UpperUnderscoreName("enum", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber)));
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

	if (info.Current().type != TokenType::End)
		ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("enum", enumLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

	FunctionSymbol* const assign = new FunctionSymbol(info.GetFileInfo(info.Current().line));
	assign->arguments.Add(enumSymbol->AbsoluteName());
	assign->symbolNode = new IntegerAssignNode(enumSymbol->Size());
	enumSymbol->AddSymbol(Scope::Assign, assign);

	FunctionSymbol* const eq = new FunctionSymbol(info.GetFileInfo(info.Current().line));
	eq->arguments.Add(enumSymbol->AbsoluteName());
	eq->arguments.Add(enumSymbol->AbsoluteName());
	eq->returnValues.Add(ScopeList::Bool);
	eq->symbolNode = new IntegerBinaryOperatorNode(enumSymbol->Size(), enumSymbol->IsSigned(), InstructionType::Eq);
	enumSymbol->AddSymbol(Scope::Equal, eq);

	FunctionSymbol* const ne = new FunctionSymbol(info.GetFileInfo(info.Current().line));
	ne->arguments.Add(enumSymbol->AbsoluteName());
	ne->arguments.Add(enumSymbol->AbsoluteName());
	ne->returnValues.Add(ScopeList::Bool);
	ne->symbolNode = new IntegerBinaryOperatorNode(enumSymbol->Size(), enumSymbol->IsSigned(), InstructionType::Ne);
	enumSymbol->AddSymbol(Scope::NotEqual, ne);

	info.index++;

	info.scope = info.scope->Parent<ScopeSymbol>();
	return en;
}

List<EnumParser::EnumValue> EnumParser::ParseValues(ParsingInfo& info) {
	List<EnumValue> values;
	ULong currentValue = 0;

	if (Optional<EnumValue> value = ParseValue(info, currentValue)) {
		values.Add((EnumValue)value);

		while (info.Current().type == TokenType::Comma) {
			info.index++;

			if (Optional<EnumValue> value = ParseValue(info, currentValue)) {
				values.Add((EnumValue)value);
			}
			else {
				ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("name", "','"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
				break;
			}
		}
	}

	return values;
}

Optional<EnumParser::EnumValue> EnumParser::ParseValue(ParsingInfo& info, ULong& currentValue) {
	if (info.Current().type != TokenType::Name) return nullptr;

	EnumValue value;
	value.name = Scope(info.Current().value);
	value.line = info.Current().line;

	if (info.Next().type == TokenType::Assign) {
		info.index++;

		if (NodePtr num = IntegerParser::Parse(info)) {
			value.value = num.Cast<IntegerNode>()->number;
		}
		else {
			ErrorLog::Error(SyntaxError(SyntaxError::ExpectedAfter("integer", "'='"), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));
		}
	}
	else {
		value.value = currentValue++;
	}

	return value;
}
