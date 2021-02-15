#include "AssignNode.h"

#include "NewVariableNode.h"
#include "RefNode.h"
#include "MemoryNode.h"
#include "NameNode.h"
#include "TypeNode.h"
#include "EmptyNode.h"
#include "StatementsNode.h"
#include "DiscardNode.h"

#include "Melon/Parsing/Parser.h"
#include "Melon/Parsing/IncludeParser.h"

#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/NamespaceSymbol.h"

#include "Melon/Symbols/Nodes/SymbolNode.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon::Nodes;
using namespace Melon::Optimizing;
using namespace Melon::Parsing;
using namespace Melon::Symbols;
using namespace Melon::Symbols::Nodes;

AssignNode::AssignNode(Symbol* const scope, const FileInfo& file) : Node(scope, file) {

}

AssignNode::~AssignNode() {

}

List<Pair<TypeSymbol*, NodePtr>> AssignNode::Values() const {
	List<Pair<TypeSymbol*, NodePtr>> types;

	for (UInt i = 0; i < vars.Size(); i++) {
		if (i + 1 >= values.Size()) {
			List<TypeSymbol*> returnTypes = values[i]->Types();

			for (UInt u = 0; u < vars.Size() - i; u++) {
				types.Add(Pair<TypeSymbol*, NodePtr>(returnTypes[u], values[i]));
			}

			break;
		}
		else {
			types.Add(Pair<TypeSymbol*, NodePtr>(values[i]->Type(), values[i]));
		}
	}

	return types;
}

UInt AssignNode::GetSize() const {
	UInt size = 0;

	for (UInt i = 0; i < vars.Size(); i++) {
		if (types[i] == ScopeList::Discard || vars[i].Is<DiscardNode>()) continue;

		if (Symbol* const s = vars[i]->GetSymbol()) {
			if (VariableSymbol* const var = s->Cast<VariableSymbol>()) {
				if ((var->attributes & VariableAttributes::Ref) != VariableAttributes::None) {
					size += StackPtr::ptrSize;
				}
				else {
					size += var->Type()->Size();
				}
			}
		}
	}

	return size;
}

CompiledNode AssignNode::Compile(CompileInfo& info) {
	CompiledNode c;

	UInt varSize = 0;

	// Calculate size and stack index of variables
	for (UInt i = 0; i < vars.Size(); i++) {
		if (types[i] == ScopeList::Discard || vars[i].Is<DiscardNode>()) continue;
		VariableSymbol* const var = vars[i]->GetSymbol()->Cast<VariableSymbol>();

		// Ref variables
		if (var->HasAttribute(VariableAttributes::Ref)) {
			varSize += info.stack.ptrSize;
			info.stack.Push(info.stack.ptrSize);
		}
		// Regular variables
		else {
			UInt size = vars[i]->Type()->Size();
			varSize += size;
			info.stack.Push(size);
		}

		var->stackIndex = info.stack.top;
	}

	// Setup
	List<Pair<TypeSymbol*, NodePtr>> values = Values();
	List<UInt> returnOffsets;
	const UInt frame = info.stack.frame;

	// Compile assignments
	for (UInt i = 0; i < vars.Size(); i++) {
		const UInt regIndex = info.index; 

		// Assign values normally
		if (i < this->values.Size()) {
			// Regular assignment
			if (!vars[i].Is<DiscardNode>()) {
				info.important = true;
				c.AddInstructions(CompileAssignment(vars[i], values[i].value, info, vars[i]->file).instructions);
				info.important = false;
			}
			// Discard assignment
			else {
				// TODO: Cast to type
				c.AddInstructions(values[i].value->Compile(info).instructions);
			}

			// Calculate offsets for extra return values
			if (i + 1 >= this->values.Size()) {
				UInt size = info.stack.top + values[i].key->Size();

				for (UInt u = i + 1; u < values.Size(); u++) {
					size += values[u].key->Size();
					returnOffsets.Add(size);
				}
			}
			else {
				info.stack.PopExpr(frame, c);
			}
		}
		// Assign extra return values
		else if (!vars[i].Is<DiscardNode>()) {
			Pointer<MemoryNode> sn = new MemoryNode(info.stack.Offset(returnOffsets[i - this->values.Size()]));
			sn->type = values[i].key->AbsoluteName();

			info.important = true;
			c.AddInstructions(CompileAssignment(vars[i], sn, info, vars[i]->file).instructions);
			info.important = false;
		}

		// TODO: compile values for discard vars

		info.index = regIndex;
	}

	info.stack.PopExpr(frame, c);

	return c;
}

void AssignNode::IncludeScan(ParsingInfo& info) {
	// Check type names for includes
	for (const ScopeList& type : types) {
		if (type == ScopeList::Discard) continue;

		// Include multiple levels of namespaces
		while (Symbol* s = SymbolTable::Find(type, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates)) {
			bool done = true;

			for (UInt i = 1; i < type.Size(); i++) {
				if (s->Is<NamespaceSymbol>()) {
					if (Symbol* const sym = s->Contains(type[i])) {
						s = sym;
					}
					else {
						IncludeParser::ParseInclude(s->AbsoluteName().Add(type[i]), info);
						done = false;
						break;
					}
				}
			}

			if (done) break;
		}
	}

	// Scan vars
	for (NodePtr var : vars) {
		var->IncludeScan(info);
	}

	// Scan values
	for (NodePtr value : values) {
		value->IncludeScan(info);
	}
}

ScanResult AssignNode::Scan(ScanInfoStack& info) {
	ScanResult result;
	info.Assign(true);

	// Check for errors among values
	UInt errorCount = ErrorLog::ErrorCount();
	List<Pair<TypeSymbol*, NodePtr>> values = Values();
	bool errors = errorCount < ErrorLog::ErrorCount();

	// Scan all vars
	for (UInt i = 0; i < vars.Size(); i++) {
		if (vars[i].Is<DiscardNode>()) continue;

		NodePtr node = vars[i];
		node->Type();

		// Find variable
		VariableSymbol* var = nullptr;
		
		if (Symbol* const sym = node->GetSymbol()) {
			var = sym->Cast<VariableSymbol>();
		}

		// Check for const assign
		if (types[i] == ScopeList::Discard && var && (var->attributes & VariableAttributes::Const) != VariableAttributes::None) {
			ErrorLog::Error(SymbolError(SymbolError::ConstAssign, node->file));
		}

		// Scan node
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->file);
		result |= r;

		// Check for completed init
		if (info.Init()) {
			if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
				if (nn->name == Scope::Self) {
					info.Type()->CompleteInit();
					result.selfUsed = false;
				}
			}
		}

		// Scan assignment
		if (!errors) {
			ScanAssignment(node, new TypeNode(values[i].key->AbsoluteName()), info, node->file);
		}
	}

	info.Assign(false);

	// Scan values
	for (const NodePtr& node : this->values) {
		ScanResult r = node->Scan(info);
		r.SelfUseCheck(info, node->file);
		result |= r;
	}

	return result;
}

ScopeList AssignNode::FindSideEffectScope(const bool assign) {
	ScopeList list = vars[0] ? vars[0]->GetSideEffectScope(true) : scope->AbsoluteName();

	for (UInt i = 1; i < vars.Size(); i++) {
		if (vars[i]) {
			list = CombineSideEffects(list, vars[i]->GetSideEffectScope(true));
		}
	}

	for (UInt i = 0; i < values.Size(); i++) {
		list = CombineSideEffects(list, values[i]->GetSideEffectScope(assign));
	}

	return list;
}

NodePtr AssignNode::Optimize(OptimizeInfo& info) {
	bool removed = false;

	// Check if removal of vars/values can be done
	for (UInt i = 0; i < vars.Size(); i++) {
		if (vars[i].Is<DiscardNode>()) {
			removed = true;
			continue;
		}

		// Remove unused vars
		if (Symbol* const sym = vars[i]->GetSymbol()) {
			if (sym->Is<VariableSymbol>() && !vars[i]->HasSideEffects(scope->AbsoluteName()) && !info.usedVariables.Contains(sym->Cast<VariableSymbol>())) {
				vars[i] = new DiscardNode(vars[i]->scope, vars[i]->file);
				info.optimized = true;
				removed = true;
			}
		}
	}

	// Remove values
	if (removed) for (UInt i = values.Size(); i > 0;) {
		i--;

		// Remove values without side effects
		if (vars[i].Is<DiscardNode>() && !values[i]->HasSideEffects(scope->AbsoluteName())) {
			// Check if the value is part of a multiple return
			if (i >= values.Size() - 1) {
				bool isDiscard = true;
				
				// Only remove value if multiple return is not affected
				for (UInt u = i + 1; u < vars.Size(); u++) {
					if (!vars[u].Is<DiscardNode>()) {
						isDiscard = false;
						break;
					}
				}

				if (isDiscard) values[i] = nullptr;
			}
			else {
				values[i] = nullptr;
			}
		}
	}

	// Remove vars and values
	if (removed) for (UInt i = vars.Size(); i > 0;) {
		i--;

		if (vars[i].Is<DiscardNode>()) {
			// Remove multiple return vars
			if (i >= values.Size()) {
				if (i == vars.Size() - 1 || !values.Last()) {
					vars.RemoveAt(i);
					types.RemoveAt(i);
				}
			}
			// Remove regular vars/values
			else if (!values[i]) {
				vars.RemoveAt(i);
				values.RemoveAt(i);
				types.RemoveAt(i);
			}
		}
	}

	// Remove assignment if everything is removed
	if (removed && vars.IsEmpty()) {
		return new EmptyNode();
	}

	// Optimize vars
	for (NodePtr& var : vars) {
		if (NodePtr node = var->Optimize(info)) var = node;
	}

	// Optimize values
	for (NodePtr& value : values) {
		if (NodePtr node = value->Optimize(info)) value = node;
	}

	return nullptr;
}

StringBuilder AssignNode::ToMelon(const UInt indent) const {
	StringBuilder sb;

	Optional<ScopeList> type = types[0];

	// Check if all types are equal
	for (UInt i = 1; i < types.Size(); i++) {
		if (types[0] != types[i]) {
			type = nullptr;
		}
	}

	// Single/No types
	if (type) {
		if (type.Get() != ScopeList::Discard) {
			sb += type.Get().ToSimpleString();
			sb += ": ";
		}
	}
	// Multiple types
	else {
		for (UInt i = 0; i < types.Size(); i++) {
			if (i > 0) sb += ", "; 
			sb += types[i].ToSimpleString();
		}

		sb += ": ";
	}
	
	// Vars
	for (UInt i = 0; i < vars.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += vars[i]->ToMelon(indent);
	}

	sb += " = ";

	// Values
	for (UInt i = 0; i < values.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += values[i]->ToMelon(indent);
	}

	return sb;
}