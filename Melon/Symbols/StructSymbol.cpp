#include "StructSymbol.h"

#include "SymbolTable.h"
#include "TypeSymbol.h"
#include "VariableSymbol.h"

#include "Melon/Nodes/RootNode.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

StructSymbol::StructSymbol(const FileInfo& file) : ClassStructBaseSymbol(file) {

}

StructSymbol::~StructSymbol() {

}

StructSymbol* StructSymbol::InitializeSpecialize() {
	return new StructSymbol(file);
}
