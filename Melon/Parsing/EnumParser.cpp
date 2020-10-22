#include "EnumParser.h"

#include "NewVariableParser.h"
#include "FunctionParser.h"
#include "IntegerParser.h"

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
	Symbol enumSymbol = Symbol(SymbolType::Enum);
	enumSymbol.symbolFile = info.currentFile;
	enumSymbol.symbolNamespace = info.currentNamespace;
	enumSymbol.includedNamespaces = info.includedNamespaces;
	enumSymbol.scope = info.scopes.Add(enumName);
	enumSymbol.size = 1;
	enumSymbol.isSigned = false;
	Pointer<EnumNode> en = new EnumNode(info.scopes, FileInfo(info.filename, enumLine, info.statementNumber));

	en->name = enumName;

	info.scopes = info.scopes.Add(enumName);
	Symbol::Add(info.scopes, enumSymbol, FileInfo(info.filename, info.Current().line, info.statementNumber));


	List<EnumValue> values = ParseValues(info);

	for (const EnumValue& value : values) {
		Symbol v = Symbol(SymbolType::Value);
		v.symbolFile = info.currentFile;
		v.symbolNamespace = info.currentNamespace;
		v.includedNamespaces = info.includedNamespaces;
		v.varType = info.scopes;
		v.value = value.value;

		enumSymbol.Add(value.name, v, FileInfo(info.filename, info.Current().line, info.statementNumber));
		en->values.Add(value.name);
		enumSymbol.names.Add(value.name);
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

	Symbol assign = Symbol(SymbolType::Function);
	assign.symbolFile = info.currentFile;
	assign.symbolNamespace = info.currentNamespace;
	assign.includedNamespaces = info.includedNamespaces;
	assign.arguments.Add(info.scopes);
	assign.symbolNode = new IntegerAssignNode(enumSymbol.size);
	enumSymbol.Add(Scope::Assign, assign, FileInfo(info.filename, info.Current().line, info.statementNumber));

	Symbol eq = Symbol(SymbolType::Function);
	eq.symbolFile = info.currentFile;
	eq.symbolNamespace = info.currentNamespace;
	eq.includedNamespaces = info.includedNamespaces;
	eq.arguments.Add(info.scopes);
	eq.arguments.Add(info.scopes);
	eq.returnValues.Add(ScopeList::Bool);
	eq.symbolNode = new IntegerBinaryOperatorNode(enumSymbol.size, enumSymbol.isSigned, InstructionType::Eq);
	enumSymbol.Add(Scope::Equal, eq, FileInfo(info.filename, info.Current().line, info.statementNumber));

	Symbol ne = Symbol(SymbolType::Function);
	ne.symbolFile = info.currentFile;
	ne.symbolNamespace = info.currentNamespace;
	ne.includedNamespaces = info.includedNamespaces;
	ne.arguments.Add(info.scopes);
	ne.arguments.Add(info.scopes);
	ne.returnValues.Add(ScopeList::Bool);
	ne.symbolNode = new IntegerBinaryOperatorNode(enumSymbol.size, enumSymbol.isSigned, InstructionType::Ne);
	enumSymbol.Add(Scope::NotEqual, ne, FileInfo(info.filename, info.Current().line, info.statementNumber));

	Symbol::Add(info.scopes, enumSymbol, FileInfo(info.filename, enumLine, info.statementNumber), true);
	en->symbol = Symbol::Find(enumSymbol.scope, FileInfo(info.filename, enumLine, info.statementNumber));

	info.index++;

	info.scopes = info.scopes.Pop();
	return en;
}

NodePtr EnumParser::ParseFunction(ParsingInfo& info) {
	if (NodePtr node = FunctionParser::Parse(info, false)) {
		if (Pointer<EmptyNode> en = node.Cast<EmptyNode>()) {
			if (Pointer<FunctionNode> fn = en->node.Cast<FunctionNode>()) {
				if (!fn->s.attributes.Contains(SymbolAttribute::Static)) {
					Symbol a = Symbol(SymbolType::Variable);
					a.symbolNamespace = info.currentNamespace;
					a.includedNamespaces = info.includedNamespaces;
					a.varType = info.scopes;
					a.attributes.Add(SymbolAttribute::Ref);
					//a.attributes.Add(SymbolAttribute::Const);	Add to class

					fn->s.arguments.Insert(0, info.scopes);
					fn->s.names.Insert(0, Scope::Self);
					fn->argNames.Insert(0, Scope::Self);

					Symbol::Add(fn->func, fn->s, FileInfo(info.filename, info.Current().line, info.statementNumber), true);
					fn->s = Symbol::Find(fn->func, FileInfo(info.filename, info.Current().line, info.statementNumber));
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