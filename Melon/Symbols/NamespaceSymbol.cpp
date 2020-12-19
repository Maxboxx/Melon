#include "NamespaceSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

NamespaceSymbol::NamespaceSymbol(const String& includedPath, const FileInfo& file) : ScopeSymbol(file) {
	this->includedPath = includedPath;
}

NamespaceSymbol::~NamespaceSymbol() {

}

String NamespaceSymbol::IncludedPath() const {
	return includedPath;
}
