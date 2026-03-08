#include "Debug.h"

#include "Expression.h"

#include "Kiwi/Instruction.h"

#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

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

	if (argument2) {
		argument2->IncludeScan(info);
	}
}

ScanResult Debug::Scan(ScanInfoStack& info) {
	if (argument) {
		return argument->Scan(info);
	}

	if (argument2) {
		return argument2->Scan(info);
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
	else if (type.Sub(0, 3) == "byte" && argument && argument2) {
		Int offset = type.Sub(4).ToInt();

		Ptr<Kiwi::Variable> value = argument->Compile(info).AsPtr<Kiwi::Variable>();

		Ptr<Kiwi::DebugByteInstruction> getByte = new Kiwi::DebugByteInstruction(value, argument2->Compile(info), offset);
		info.AddInstruction(getByte);
	}
	else if (type == "writebytes" && argument && argument2) {
		Ptr<Kiwi::Variable> name = argument->Compile(info).AsPtr<Kiwi::Variable>();
		Ptr<Kiwi::Variable> data = argument2->Compile(info).AsPtr<Kiwi::Variable>();

		TypeSymbol* type1 = argument->Type();
		TypeSymbol* type2 = argument2->Type();

		Ptr<Kiwi::DebugFileWriteInstruction> write = new Kiwi::DebugFileWriteInstruction(
			new Kiwi::SubVariable(name->Copy(), type1->Find(Name::Items, File())->KiwiName()),
			new Kiwi::SubVariable(data->Copy(), type2->Find(Name::Items, File())->KiwiName()),
			new Kiwi::SubVariable(name->Copy(), type1->Find(Name::Length, File())->KiwiName()),
			new Kiwi::SubVariable(data->Copy(), type2->Find(Name::Length, File())->KiwiName())
		);

		info.AddInstruction(write);
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
