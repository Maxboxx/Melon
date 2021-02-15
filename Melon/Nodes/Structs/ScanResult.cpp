#include "ScanResult.h"

#include "Melon/Symbols/TypeSymbol.h"

using namespace Boxx;

using namespace Melon;
using namespace Melon::Nodes;

void ScanResult::SelfUseCheck(const ScanInfoStack& info, const FileInfo& file) {
	if (selfUsed && info->init && !info->type->IsInitialized()) {
		ErrorLog::Error(CompileError(CompileError::SelfInit, file));
	}
}
