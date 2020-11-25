#include "EnumParser.h"

#include "NewVariableParser.h"
#include "FunctionParser.h"
#include "IntegerParser.h"

#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/ValueSymbol.h"

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

	EnumSymbol* enumSymbol = new EnumSymbol(info.GetFileInfo(enumLine));
	enumSymbol->size = 1;
	enumSymbol->isSigned = false;

	Pointer<EnumNode> en = new EnumNode(info.scopes, FileInfo(info.filename, enumLine, info.statementNumber, info.currentNamespace, info.includedNamespaces));

	en->name = enumName;

	enumSymbol = info.scope->AddSymbol(enumName.name, enumSymbol)->Cast<EnumSymbol>();

	List<EnumValue> values = ParseValues(info);

	for (const EnumValue& value : values) {
		ValueSymbol* v = new ValueSymbol(info.GetFileInfo(value.line));
		v->value = value.value;

		enumSymbol->AddSymbol(value.name.name, v);
		en->values.Add(value.name);
	}

	while (true) {
		bool found = false;

		if (NodePtr node = ParseFunction(info)) {
			found = true;
		}

		if (!found) break;
	}

	if (info.Current().type != TokenType::End)
		ErrorLog::Error(SyntaxError(SyntaxError::EndExpected("enum", enumLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber)));

	Symbols assign = Symbols(SymbolType::Function);
	assign.symbolFile = info.currentFile;
	assign.symbolNamespace = info.currentNamespace;
	assign.includedNamespaces = info.includedNamespaces;
	assign.arguments.Add(info.scopes);
	assign.symbolNode = new IntegerAssignNode(enumSymbol.size);
	enumSymbol.Add(Scope::Assign, assign, FileInfo(info.filename, info.Current().line, info.statementNumber));

	Symbols eq = Symbols(SymbolType::Function);
	eq.symbolFile = info.currentFile;
	eq.symbolNamespace = info.currentNamespace;
	eq.includedNamespaces = info.includedNamespaces;
	eq.arguments.Add(info.scopes);
	eq.arguments.Add(info.scopes);
	eq.returnValues.Add(ScopeList::Bool);
	eq.symbolNode = new IntegerBinaryOperatorNode(enumSymbol.size, enumSymbol.isSigned, InstructionType::Eq);
	enumSymbol.Add(Scope::Equal, eq, FileInfo(info.filename, info.Current().line, info.statementNumber));

	Symbols ne = Symbols(SymbolType::Function);
	ne.symbolFile = info.currentFile;
	ne.symbolNamespace = info.currentNamespace;
	ne.includedNamespaces = info.includedNamespaces;
	ne.arguments.Add(info.scopes);
	ne.arguments.Add(info.scopes);
	ne.returnValues.Add(ScopeList::Bool);
	ne.symbolNode = new IntegerBinaryOperatorNode(enumSymbol.size, enumSymbol.isSigned, InstructionType::Ne);
	enumSymbol.Add(Scope::NotEqual, ne, FileInfo(info.filename, info.Current().line, info.statementNumber));

	Symbols::Add(info.scopes, enumSymbol, FileInfo(info.filename, enumLine, info.statementNumber), true);
	en->symbol = Symbols::Find(enumSymbol.scope, FileInfo(info.filename, enumLine, info.statementNumber));

	info.index++;

	info.scopes = info.scopes.Pop();
	return en;
}

NodePtr EnumParser::ParseFunction(ParsingInfo& info) {
	if (NodePtr node = FunctionParser::Parse(info, false)) {
		if (Pointer<EmptyNode> en = node.Cast<EmptyNode>()) {
			if (Pointer<FunctionNode> fn = en->node.Cast<FunctionNode>()) {
				if (!fn->s.attributes.Contains(SymbolAttribute::Static)) {
					Symbols a = Symbols(SymbolType::Variable);
					a.symbolNamespace = info.currentNamespace;
					a.includedNamespaces = info.includedNamespaces;
					a.varType = info.scopes;
					a.attributes.Add(SymbolAttribute::Ref);
					//a.attributes.Add(SymbolAttribute::Const);	Add to class

					fn->s.arguments.Insert(0, info.scopes);
					fn->s.names.Insert(0, Scope::Self);
					fn->argNames.Insert(0, Scope::Self);

					Symbols::Add(fn->func, fn->s, FileInfo(info.filename, info.Current().line, info.statementNumber), true);
					fn->s = Symbols::Find(fn->func, FileInfo(info.filename, info.Current().line, info.statementNumber));
					fn->s.Add(Scope::Self, a, FileInfo(info.filename, info.Current().line, info.statementNumber));
				}

				return en;
			}
		}
	}

	return nullptr;
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
