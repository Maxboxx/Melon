#include "Debug.h"

#include "Expression.h"

#include "Kiwi/Instruction.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;

Debug::Debug(Symbols::Symbol* const scope, const FileInfo& file) : Statement(scope, file) {
	
}

void Debug::IncludeScan(Parsing::ParsingInfo& info) {
	if (argument) {
		argument->IncludeScan(info);
	}
}

ScanResult Debug::Scan(ScanInfoStack& info) {
	if (argument) {
		return argument->Scan(info);
	}

	return ScanResult();
}

Ptr<Kiwi::Value> Debug::Compile(CompileInfo& info) {
	if (type == "print" && argument) {
		Ptr<Kiwi::Value> value = argument->Compile(info);
		info.AddInstruction(new Kiwi::DebugPrintInstruction(value));
	}

	return nullptr;
}

StringBuilder Debug::ToMelon(const Boxx::UInt indent) const {
	StringBuilder str = "_debug ";
	str += type;

	if (argument) {
		str += " ";
		str += argument->ToMelon(indent);
	}

	return str;
}
