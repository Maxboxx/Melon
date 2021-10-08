#include "ObjectInitNode.h"

#include "MemoryNode.h"
#include "TypeNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/TypeSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

ObjectInitNode::ObjectInitNode(Symbols::Symbol* const scope, const FileInfo& file) : ExpressionNode(scope, file) {

}

ObjectInitNode::~ObjectInitNode() {

}

TypeSymbol* ObjectInitNode::Type() const {
	Symbols::Symbol* const sym = expression->Symbol();

	if (sym == nullptr) return nullptr;

	if (TypeSymbol* const t = sym->Cast<TypeSymbol>()) {
		return t;
	}

	return nullptr;
}

CompiledNode ObjectInitNode::Compile(CompileInfo& info) {
	CompiledNode c = expression->Compile(info);
	
	TypeSymbol* const type = Type();

	UInt offset = type->Size();
	info.stack.PushExpr(offset, c);

	UInt index = info.index;

	// Compile vars
	for (UInt i = 0; i < vars.Size(); i++) {
		VariableSymbol* const var = type->Find<VariableSymbol>(vars[i], file);

		if (type->Is<StructSymbol>()) {
			Pointer<MemoryNode> sn = new MemoryNode(info.stack.Offset() + var->stackIndex);
			sn->type = var->Type()->AbsoluteName();

			c.AddInstructions(CompileAssignment(sn, expressions[i], info, expressions[i]->File()).instructions);
		}

		info.index = index;
	}

	c.argument = Argument(MemoryLocation(info.stack.Offset()));
	info.stack.Pop(type->Size());
	return c;
}

void ObjectInitNode::IncludeScan(ParsingInfo& info) {
	expression->IncludeScan(info);

	for (Expression& expression : expressions) {
		expression->IncludeScan(info);
	}
}

ScanResult ObjectInitNode::Scan(ScanInfoStack& info) {
	ScanResult result = expression->Scan(info);
	result.SelfUseCheck(info, expression->File());
	
	TypeSymbol* const type = Type();

	if (type == nullptr) return result;

	// Check if object init is valid
	if (StructSymbol* const s = type->Cast<StructSymbol>()) {
		for (const Name& member : s->members) {
			bool found = false;

			for (const Name& var : vars) {
				if (var == member) {
					found = true;
					break;
				}
			}

			if (found) continue;

			ErrorLog::Error(LogMessage("error.scan.init.object_member", member.ToSimpleString()), file);
		}
	}
	else {
		ErrorLog::Error(LogMessage("error.scan.use.object"), file);
	}

	// Scan assignments
	for (UInt i = 0; i < vars.Size(); i++) {
		for (UInt u = i + 1; u < vars.Size(); u++) {
			if (vars[i] == vars[u]) {
				ErrorLog::Error(LogMessage("error.scan.init.multiple", vars[i].name), file);
			}
		}

		VariableSymbol* const v = type->Find<VariableSymbol>(vars[i], file);
		TypeSymbol* const varType = v->Type();

		ScanAssignment(new TypeNode(varType->AbsoluteName()), expressions[i], info, expressions[i]->File());
	}

	// Scan expressions
	for (const Expression& node : expressions) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->File());
		result |= r;
	}

	return result;
}

NameList ObjectInitNode::FindSideEffectScope(const bool assign) {
	NameList list = expression->GetSideEffectScope(assign);

	for (Expression& expr : expressions) {
		list = CombineSideEffects(list, expr->GetSideEffectScope(assign));
	}

	return list;
}

Expression ObjectInitNode::Optimize(OptimizeInfo& info) {
	Node::Optimize(expression, info);

	for (Expression& expr : expressions) {
		Node::Optimize(expr, info);
	}

	return nullptr;
}

StringBuilder ObjectInitNode::ToMelon(const UInt indent) const {
	StringBuilder sb = expression->ToMelon(indent);

	if (vars.IsEmpty()) {
		sb += " {}";
		return sb;
	}

	sb += " {\n";

	String tabs = String('\t').Repeat(indent + 1);

	for (UInt i = 0; i < vars.Size(); i++) {
		if (i > 0) sb += ",\n";
		sb += tabs;
		sb += vars[i].ToString();
		sb += " = ";
		sb += expressions[i]->ToMelon(indent + 1);
	}

	sb += "\n";
	sb += String('\t').Repeat(indent);
	sb += "}";
	return sb;
}
