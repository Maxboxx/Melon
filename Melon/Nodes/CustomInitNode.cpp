#include "CustomInitNode.h"

#include "MemoryNode.h"
#include "TypeNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

CustomInitNode::CustomInitNode(const ScopeList& scope, const FileInfo& file) : Node(scope, file) {

}

CustomInitNode::~CustomInitNode() {

}

ScopeList CustomInitNode::Type() const {
	return node->Type();
}

CompiledNode CustomInitNode::Compile(CompileInfo& info) {
	CompiledNode c = node->Compile(info);

	Symbols s = Symbols::Find(Type(), file);

	UInt offset = s.size;

	info.stack.PushExpr(s.size, c);

	UInt index = info.index;

	for (UInt i = 0; i < vars.Size(); i++) {
		Symbols v = s.Get(vars[i], file);
		Symbols varType = v.GetType(file);
		const ScopeList type = varType.scope;

		if (s.type == SymbolType::Struct) {
			Pointer<MemoryNode> sn = new MemoryNode(info.stack.Offset() + v.offset);
			sn->type = type;

			c.AddInstructions(CompileAssignment(sn, expressions[i], info, expressions[i]->file).instructions);
		}

		info.index = index;
	}

	c.argument = Argument(MemoryLocation(info.stack.Offset()));
	info.stack.Pop(s.size);
	return c;
}

void CustomInitNode::IncludeScan(ParsingInfo& info) {
	node->IncludeScan(info);

	for (NodePtr expression : expressions) {
		expression->IncludeScan(info);
	}
}

Set<ScanType> CustomInitNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = node->Scan(info);

	Symbols s = Symbols::Find(Type(), file);

	if (s.type == SymbolType::None) return scanSet;

	for (const Scope& var : s.GetUnassignedVars(vars)) {
		ErrorLog::Error(CompileError(CompileError::VarNotCustomInitStart + var.ToString() + CompileError::VarNotCustomInitEnd, file));
	}

	for (UInt i = 0; i < vars.Size(); i++) {
		for (UInt u = i + 1; u < vars.Size(); u++) {
			if (vars[i] == vars[u]) {
				ErrorLog::Error(CompileError(CompileError::MultipleInitStart + vars[i].name + CompileError::MultipleInitEnd, file));
			}
		}

		Symbols v = s.Get(vars[i], file);
		Symbols varType = v.GetType(file);

		ScanAssignment(new TypeNode(varType.scope), expressions[i], info, expressions[i]->file);

		if (s.type != SymbolType::Struct) {
			ErrorLog::Error(CompileError(CompileError::InvalidCustomInit, file));
		}
	}

	if (info.Get().init && scanSet.Contains(ScanType::Self) && !info.Get().symbol.IsAssigned()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
	}

	for (const NodePtr& node : expressions) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
		}
	}

	return scanSet;
}

ScopeList CustomInitNode::FindSideEffectScope(const bool assign) {
	ScopeList list = node->GetSideEffectScope(assign);

	for (NodePtr expr : expressions) {
		list = CombineSideEffects(list, expr->GetSideEffectScope(assign));
	}

	return list;
}

NodePtr CustomInitNode::Optimize(OptimizeInfo& info) {
	if (NodePtr n = node->Optimize(info)) node = n;

	for (NodePtr expr : expressions) {
		if (NodePtr node = expr->Optimize(info)) expr = node;
	}

	return nullptr;
}

Mango CustomInitNode::ToMango() const {
	Mango m = Mango("init", MangoType::List);

	m.Add(node->ToMango());

	Symbols s = Symbols::Find(Type(), file);

	for (UInt i = 0; i < vars.Size(); i++) {
		Symbols v = s.Get(vars[i], file);
		const ScopeList type = Symbols::FindNearest(Type(), v.varType, file).scope;

		List<ScopeList> argTypes;
		argTypes.Add(expressions[i]->Type());
		Mango a = Mango(Symbols::FindFunction(type.Add(Scope::Assign), argTypes, expressions[i]->file).scope.ToString(), MangoType::List);
		a.Add(Mango("var", vars[i].ToString()));
		a.Add(expressions[i]->ToMango());
		m.Add(a);
	}

	return m;
}

StringBuilder CustomInitNode::ToMelon(const UInt indent) const {
	StringBuilder sb = node->ToMelon(indent);

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