#include "DotNode.h"

#include "NameNode.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

#include "Melon/Symbols/ValueSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/NamespaceSymbol.h"
#include "Melon/Symbols/FunctionSymbol.h"
#include "Melon/Symbols/EnumSymbol.h"

#include "Kiwi/Kiwi.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;

DotNode::DotNode(Symbols::Symbol* const scope, const FileInfo& file) : Expression(scope, file) {

}

DotNode::~DotNode() {

}

TypeSymbol* DotNode::Type() const {
	Symbols::Symbol* const s = Symbol();

	if (s == nullptr) return nullptr;

	if (s->Is<ValueSymbol>() || s->Is<VariableSymbol>() || s->Is<TemplateSymbol>()) {
		return s->Type();
	}
	else if (TypeSymbol* const t = s->Cast<TypeSymbol>()) {
		return t;
	}

	return nullptr;
}

Symbol* DotNode::Symbol() const {
	Symbols::Symbol* const nodeSym = expression->Symbol();

	if (nodeSym == nullptr) {
		return nullptr;
	}

	// Get variable symbol
	if (nodeSym->Is<VariableSymbol>()) {
		TypeSymbol* const type = nodeSym->Type();

		if (StructSymbol* const s = type->Cast<StructSymbol>()) {
			return s->Find(name, file);
		}
	}
	// Get other symbols
	else {
		if (name.types) {
			if (Symbols::Symbol* const s = nodeSym->Contains(Name(name.name))) {
				if (s->Is<FunctionSymbol>()) {
					return s;
				}
			}

			if (!nodeSym->Contains(name)) {
				Root()->AddTemplateSpecialization(nodeSym->AbsoluteName().Add(name), scope->AbsoluteName(), file);
			}
		}

		return nodeSym->Find(name, file);
	}

	return nullptr;
}

CompiledNode DotNode::Compile(CompileInfo& info) {
	TypeSymbol* const type = expression->Type();

	CompiledNode c = expression->Compile(info);

	// Compile enum value
	if (type->Is<EnumSymbol>()) {
		c.argument = Argument(type->Find<ValueSymbol>(name, file)->value);
		c.size = type->Size();
	}
	// Compile variable
	else {
		VariableSymbol* const var = type->Find<VariableSymbol>(name, file);

		if (var->HasAttribute(VariableAttributes::Static)) {
			c.argument = MemoryLocation(0);
			c.argument.mem.memptr = var->AbsoluteName().ToString();
		}

		c.argument.mem.offset += var->stackIndex;
		c.size = var->Type()->Size();
	}

	return c;
}

void DotNode::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);
	
	Symbols::Symbol* const s = expression->Symbol();

	if (!name.types) {
		if (s->Is<NamespaceSymbol>() && !s->Contains(name)) {
			IncludeParser::ParseInclude(s->AbsoluteName().Add(name), info);
		}
	}
}

ScanResult DotNode::Scan(ScanInfoStack& info) {
	const bool assign = info->assign;
	info->assign = false;

	// Scan node
	ScanResult result = expression->Scan(info);
	info->assign = assign;

	TypeSymbol* const type = expression->Type();
	if (type == nullptr) return result;

	Symbols::Symbol* sym = type->Find(Name(name.name), file);
	if (sym == nullptr) return result;

	if (!sym->Is<FunctionSymbol>()) {
		sym = type->Find(name, file);
	}

	// Scan variable
	if (VariableSymbol* const var = sym->Cast<VariableSymbol>()) {
		if ((var->attributes & VariableAttributes::Static) != VariableAttributes::None) {
			info.usedVariables.Add(var);
		}
	}

	// Scan assignment
	if (info->assign) {
		if (const Pointer<NameNode>& nn = expression.Cast<NameNode>()) {
			if (nn->name == Name::Self) {
				if (VariableSymbol* const var = sym->Cast<VariableSymbol>()) {
					if (info->scopeInfo.WillContinue()) {
						var->isAssigned = true;
					}

					if (info->type->IsInitialized()) {
						info->init = false;
					}
				}

				result.selfUsed = false;
			}
		}
	}
	// Scan init
	else if (info->init) {
		if (const Pointer<NameNode>& nn = expression.Cast<NameNode>()) {
			if (nn->name == Name::Self) {
				if (VariableSymbol* const var = sym->Cast<VariableSymbol>()) {
					if (!var->isAssigned) {
						ErrorLog::Error(LogMessage("error.scan.use.member", name.ToSimpleString()), file);
					}
				}

				result.selfUsed = false;
			}
		}
	}
	else {
		result.selfUsed = false;
	}

	return result;
}

NameList DotNode::FindSideEffectScope(const bool assign) {
	if (assign) {
		return expression->GetSideEffectScope(assign);
	}

	return scope->AbsoluteName();
}

_Expression_ DotNode::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);
	return nullptr;
}

StringBuilder DotNode::ToMelon(const UInt indent) const {
	StringBuilder sb = expression->ToMelon(indent);
	sb += ".";
	sb += name.ToSimpleString();
	return sb;
}