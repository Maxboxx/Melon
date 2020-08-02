#include "StructFunctionParser.h"

#include "FunctionParser.h"

#include "Melon/Nodes/EmptyNode.h"
#include "Melon/Nodes/FunctionNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

NodePtr StructFunctionParser::Parse(ParsingInfo& info) {
	if (NodePtr node = FunctionParser::Parse(info, false)) {
		if (Pointer<EmptyNode> en = node.Cast<EmptyNode>()) {
			if (Pointer<FunctionNode> fn = en->node.Cast<FunctionNode>()) {
				if (!fn->s.attributes.Contains(SymbolAttribute::Static)) {
					Symbol a = Symbol(SymbolType::Variable);
					a.varType = ScopeList().Add(info.scopes.Last());
					a.attributes.Add(SymbolAttribute::Ref);
					//a.attributes.Add(SymbolAttribute::Const);	Add to class

					fn->s.args.Insert(0, ScopeList().Add(info.scopes.Last()));
					fn->s.names.Insert(0, Scope::Self);
					fn->argNames.Insert(0, Scope::Self);

					Symbol::Add(fn->func, fn->s, FileInfo(info.filename, info.Current().line), true);
					fn->s = Symbol::Find(fn->func, FileInfo(info.filename, info.Current().line));
					fn->s.Add(Scope::Self, a, FileInfo(info.filename, info.Current().line));
				}

				return en;
			}
		}
	}

	return nullptr;
}