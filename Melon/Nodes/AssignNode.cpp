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

	/* TODO: node
	for (UInt i = 0; i < vars.Size(); i++) {
		if (types[i] == ScopeList::Discard || vars[i].Is<DiscardNode>()) continue;

		if (vars[i]->GetSymbol().attributes.Contains(SymbolAttribute::Ref)) {
			varSize += info.stack.ptrSize;
			info.stack.Push(info.stack.ptrSize);
		}
		else {
			UInt size = Symbols::Find(vars[i]->Type(), file).size;
			varSize += size;
			info.stack.Push(size);
		}

		Symbols::Find(Symbols::ReplaceTemplates(scope, file), file).Get(vars[i].Cast<NameNode>()->name, file).stackIndex = info.stack.top;
	}
	*/

	List<Pair<TypeSymbol*, NodePtr>> values = Values();
	List<UInt> returnOffsets;
	const UInt frame = info.stack.frame;

	for (UInt i = 0; i < vars.Size(); i++) {
		const UInt regIndex = info.index; 

		if (i < this->values.Size()) {
			if (!vars[i].Is<DiscardNode>()) {
				info.important = true;
				c.AddInstructions(CompileAssignment(vars[i], values[i].value, info, vars[i]->file).instructions);
				info.important = false;
			}
			else {
				// TODO: Cast to type
				c.AddInstructions(values[i].value->Compile(info).instructions);
			}

			if (i + 1 >= this->values.Size()) {
				/* TODO: node
				UInt size = info.stack.top + Symbols::Find(values[i].key, values[i].value->file).size;

				for (UInt u = i + 1; u < values.Size(); u++) {
					size += Symbols::Find(values[u].key, values[i].value->file).size;
					returnOffsets.Add(size);
				}
				*/
			}
			else {
				info.stack.PopExpr(frame, c);
			}
		}
		else if (!vars[i].Is<DiscardNode>()) {
			Pointer<MemoryNode> sn = new MemoryNode(info.stack.Offset(returnOffsets[i - this->values.Size()]));
			//sn->type = values[i].key;

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
	for (const ScopeList& type : types) {
		if (type == ScopeList::Discard) continue;

		while (true) {
			Symbol* s = SymbolTable::Find(type, scope->AbsoluteName(), file, SymbolTable::SearchOptions::ReplaceTemplates);

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

	for (NodePtr var : vars) {
		var->IncludeScan(info);
	}

	for (NodePtr value : values) {
		value->IncludeScan(info);
	}
}

Set<ScanType> AssignNode::Scan(ScanInfoStack& info) {
	info.Get().assign = true;

	UInt errorCount = ErrorLog::ErrorCount();

	Set<ScanType> scanSet;

	List<Pair<ScopeList, NodePtr>> values;// = Values();

	bool errors = errorCount < ErrorLog::ErrorCount();

	for (UInt i = 0; i < vars.Size(); i++) {
		if (vars[i].Is<DiscardNode>()) continue;

		NodePtr node = vars[i];
		node->Type();

		//if (types[i] == ScopeList::Discard && node->GetSymbol().attributes.Contains(SymbolAttribute::Const)) {
		//	ErrorLog::Error(SymbolError(SymbolError::ConstAssign, node->file));
		//}

		//if (types[i] != ScopeList::Discard && !node.Is<NameNode>()) {
			for (const ScanType type : node->Scan(info)) {
				scanSet.Add(type);

				/* TODO: node
				if (type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
					ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
				}
				*/
			}
		//}

		if (info.Get().init) {
			if (const Pointer<NameNode>& nn = node.Cast<NameNode>()) {
				if (nn->name == Scope::Self) {
					/* TODO: node
					Symbols::Find(nn->Type(), file).AssignAll();
					scanSet.Remove(ScanType::Self);
					*/
				}
			}
		}

		if (!errors) {
			ScanAssignment(node, new TypeNode(values[i].key), info, node->file);
		}
	}

	info.Get().assign = false;

	for (const NodePtr& node : this->values) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);

			/* TODO: node
			if (info.Get().init && type == ScanType::Self && !info.Get().symbol.IsAssigned()) {
				ErrorLog::Error(CompileError(CompileError::SelfInit, node->file));
			}
			*/
		}
	}

	return scanSet;
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

	for (UInt i = 0; i < vars.Size(); i++) {
		if (vars[i].Is<DiscardNode>()) {
			removed = true;
			continue;
		}

		/* TODO: node
		if (vars[i]->GetSymbol().IsVariable() && !vars[i]->HasSideEffects(scope) && !info.usedVariables.Contains(vars[i]->GetSymbol().scope)) {
			vars[i] = new DiscardNode(vars[i]->scope, vars[i]->file);
			info.optimized = true;
			removed = true;
		}
		*/
	}

	if (removed) for (UInt i = values.Size(); i > 0;) {
		i--;

		if (vars[i].Is<DiscardNode>() && !values[i]->HasSideEffects(scope->AbsoluteName())) {
			if (i >= values.Size() - 1) {
				bool isDiscard = true;
			
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

	if (removed) for (UInt i = vars.Size(); i > 0;) {
		i--;

		if (vars[i].Is<DiscardNode>()) {
			if (i >= values.Size()) {
				if (i == vars.Size() - 1 || !values.Last()) {
					vars.RemoveAt(i);
					types.RemoveAt(i);
				}
			}
			else if (!values[i]) {
				vars.RemoveAt(i);
				values.RemoveAt(i);
				types.RemoveAt(i);
			}
		}
	}

	if (removed && vars.IsEmpty()) {
		return new EmptyNode();
	}

	for (NodePtr& var : vars) {
		if (NodePtr node = var->Optimize(info)) var = node;
	}

	for (NodePtr& value : values) {
		if (NodePtr node = value->Optimize(info)) value = node;
	}

	return nullptr;
}

StringBuilder AssignNode::ToMelon(const UInt indent) const {
	StringBuilder sb;

	Optional<ScopeList> type = types[0];

	for (UInt i = 1; i < types.Size(); i++) {
		if (types[0] != types[i]) {
			type = nullptr;
		}
	}

	if (type) {
		if (type.Get() != ScopeList::Discard) {
			sb += type.Get().ToSimpleString();
			sb += ": ";
		}
	}
	else {
		for (UInt i = 0; i < types.Size(); i++) {
			if (i > 0) sb += ", "; 
			sb += types[i].ToSimpleString();
		}

		sb += ": ";
	}
	
	for (UInt i = 0; i < vars.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += vars[i]->ToMelon(indent);
	}

	sb += " = ";

	for (UInt i = 0; i < values.Size(); i++) {
		if (i > 0) sb += ", ";
		sb += values[i]->ToMelon(indent);
	}

	return sb;
}