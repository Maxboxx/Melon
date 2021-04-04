#include "StructParser.h"

#include "NewVariableParser.h"
#include "FunctionParser.h"
#include "TemplateParser.h"

#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"

#include "Melon/Symbols/Nodes/StructAssignNode.h"

#include "Melon/Nodes/StructNode.h"
#include "Melon/Nodes/NewVariableNode.h"
#include "Melon/Nodes/EmptyNode.h"
#include "Melon/Nodes/FunctionNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;
using namespace Melon::Parsing;

NodePtr StructParser::Parse(ParsingInfo& info) {
	if (info.Current().type != TokenType::Struct) return nullptr;

	const UInt structLine = info.Current().line;
	info.index++;
	MapSymbol* const temp = info.scope;

	Pointer<StructNode> sn = ParseName(info, structLine);
	info.scope = sn->symbol;

	while (true) {
		bool found = false;

		if (NodePtr node = ParseVariable(info)) {
			Pointer<NewVariableNode> nn = node.Cast<NewVariableNode>();

			for (UInt i = 0; i < nn->names.Size(); i++) {
				VariableSymbol* const var = new VariableSymbol(info.GetFileInfo(info.Current().line));

				if (nn->types.Size() == 1)
					var->type = nn->types[0];
				else
					var->type = nn->types[i];

				var->attributes = nn->attributes[i];
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
		ErrorLog::Error(LogMessage("error.syntax.expected.end_at", "struct", structLine), FileInfo(info.filename, info.Current(-1).line, info.statementNumber));
		info.scope = temp;
		return nullptr;
	}

	FunctionSymbol* const assign = new FunctionSymbol(info.GetFileInfo(info.Current().line));
	FunctionSymbol* const assign1 = assign->AddOverload(new FunctionSymbol(info.GetFileInfo(info.Current().line)));
	assign1->arguments.Add(sn->symbol->AbsoluteName());
	assign1->symbolNode = new StructAssignNode();
	sn->symbol->AddSymbol(Name::Assign, assign);

	info.index++;
	info.scope = temp;
	return sn;
}

Pointer<StructNode> StructParser::ParseName(ParsingInfo& info, const UInt structLine) {
	static Regex lower = Regex("^%l");
	static Regex underscore = Regex("%a_+%a");

	if (info.Current().type != TokenType::Name) {
		ErrorLog::Error(LogMessage("error.syntax.expected.name.struct"), FileInfo(info.filename, structLine, info.statementNumber));
	}

	Name structName = Name(info.Current().value);

	if (lower.Match(info.Current().value)) {
		ErrorLog::Info(LogMessage("info.name.upper", "struct", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber));
	}

	if (underscore.Match(info.Current().value)) {
		ErrorLog::Info(LogMessage("info.name.under", "struct", info.Current().value), FileInfo(info.filename, info.Current().line, info.statementNumber));
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

	Pointer<StructNode> sn = new StructNode(info.scope, info.GetFileInfo(structLine));
	sn->name = structName;
	sn->symbol = sym;
	sym->node = sn;

	return sn;
}

NodePtr StructParser::ParseVariable(ParsingInfo& info) {
	if (NodePtr node = NewVariableParser::Parse(info)) {
		return node;
	}

	return nullptr;
}