#include "Debug.h"

#include "Expression.h"

#include "Kiwi/Instruction.h"

#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"

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
	else if (type == "printstr" && argument && argument->Type() == (TypeSymbol*)SymbolTable::String) {
		Ptr<Kiwi::Variable> value = argument->Compile(info).AsPtr<Kiwi::Variable>();
		Ptr<Kiwi::DebugPrintInstruction> print = new Kiwi::DebugPrintInstruction(new Kiwi::SubVariable(value, SymbolTable::String->Find<VariableSymbol>(Name::Items, File())->KiwiName()));
		print->type = "str";

		info.AddInstruction(print);
	}
	else if (type == "printchr" && argument && argument->Type() == (TypeSymbol*)SymbolTable::Char) {
		Ptr<Kiwi::DebugPrintInstruction> print = new Kiwi::DebugPrintInstruction(argument->Compile(info));
		print->type = "chr";

		info.AddInstruction(print);
	}
	else if (type == "free" && argument) {
		Ptr<Kiwi::Value> value = argument->Compile(info);
		info.AddInstruction(new Kiwi::FreeInstruction(value));
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
