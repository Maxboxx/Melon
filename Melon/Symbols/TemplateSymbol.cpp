#include "TemplateSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Symbols;

TemplateSymbol::TemplateSymbol(const FileInfo& file) : Symbol(file) {

}

TemplateSymbol::~TemplateSymbol() {

}

Symbol* TemplateSymbol::Type() {
	return nullptr;
}
