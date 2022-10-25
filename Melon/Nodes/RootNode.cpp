#include "RootNode.h"

#include "StructStatement.h"
#include "FunctionBody.h"
#include "EmptyStatement.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/MelonCompiler.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

#include "Boxx/System.h"
#include "Boxx/Regex.h"
#include "Boxx/File.h"
#include "Boxx/Math.h"
#include "Boxx/ReplacementMap.h"

using namespace Boxx;
using namespace KiwiOld;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RootNode::RootNode() : Node(nullptr, FileInfo()) {

}

RootNode::~RootNode() {

}

CompiledNode RootNode::Compile(OldCompileInfo& info) {
	CompiledNode cn;

	// Get size of statements
	UInt size = 0;

	for (Weak<Statements> statements : nodes) {
		size += statements->GetSize();
	}

	// Push
	if (size > 0) {
		OptimizerInstruction push = Instruction(InstructionType::Push, size);
		push.important = true;
		cn.instructions.Add(push);
	}

	info.stack.PushFrame(size);

	// Compile nodes
	for (Weak<Statements> statements : nodes) {
		for (const OptimizerInstruction& instruction : statements->Compile(info).instructions) {
			cn.instructions.Add(instruction);
		}
	}

	// Pop
	if (size > 0) {
		OptimizerInstruction pop = Instruction(InstructionType::Pop, size);
		pop.important = true;
		cn.instructions.Add(pop);
	}

	info.stack.PopFrame(size);

	// Add exit instruction
	Instruction in = Instruction(InstructionType::Exit, info.stack.ptrSize);
	in.arguments.Add(Argument(0));
	cn.instructions.Add(in);

	// Compile functions
	for (Weak<FunctionBody> func : funcs) {
		for (const OptimizerInstruction& instruction : func->Compile(info).instructions) {
			cn.instructions.Add(instruction);
		}
	}

	return cn;
}

Ptr<Kiwi::Value> RootNode::Compile(CompileInfo& info) {
	Ptr<Kiwi::InstructionBlock> codeBlock = new Kiwi::InstructionBlock();
	info.currentBlock = codeBlock;

	// Compile nodes
	for (Weak<Statements> statements : nodes) {
		statements->Compile(info);
	}

	info.program->AddCodeBlock(codeBlock);
	info.currentBlock = nullptr;

	// Compile functions
	for (Weak<FunctionBody> func : funcs) {
		func->Node::Compile(info);
	}

	return nullptr;
}

List<OptimizerInstruction> RootNode::Compile(const Set<VariableSymbol*>& usedVariables) {
	CompiledNode c;

	// Static values for integers
	List<Tuple<IntegerSymbol*, InstructionType, Long, Long>> integers;
	integers.Add(Tuple<IntegerSymbol*, InstructionType, Long, Long>(SymbolTable::Byte,   InstructionType::Byte,  Math::ByteMin(),   Math::ByteMax()));
	integers.Add(Tuple<IntegerSymbol*, InstructionType, Long, Long>(SymbolTable::UByte,  InstructionType::Byte,  Math::UByteMin(),  Math::UByteMax()));
	integers.Add(Tuple<IntegerSymbol*, InstructionType, Long, Long>(SymbolTable::Short,  InstructionType::Short, Math::ShortMin(),  Math::ShortMax()));
	integers.Add(Tuple<IntegerSymbol*, InstructionType, Long, Long>(SymbolTable::UShort, InstructionType::Short, Math::UShortMin(), Math::UShortMax()));
	integers.Add(Tuple<IntegerSymbol*, InstructionType, Long, Long>(SymbolTable::Int,    InstructionType::Int,   Math::IntMin(),    Math::IntMax()));
	integers.Add(Tuple<IntegerSymbol*, InstructionType, Long, Long>(SymbolTable::UInt,   InstructionType::Int,   Math::UIntMin(),   Math::UIntMax()));
	integers.Add(Tuple<IntegerSymbol*, InstructionType, Long, Long>(SymbolTable::Long,   InstructionType::Long,  Math::LongMin(),   Math::LongMax()));
	integers.Add(Tuple<IntegerSymbol*, InstructionType, Long, Long>(SymbolTable::ULong,  InstructionType::Long,  Math::ULongMin(),  Math::ULongMax()));

	// Compile static memory
	for (const Tuple<IntegerSymbol*, InstructionType, Long, Long>& integer : integers) {
		if (usedVariables.Contains(integer.value1->Find<VariableSymbol>(Name("min"), file))) {
			Instruction name = Instruction(InstructionType::Static);
			name.instructionName = integer.value1->Find<VariableSymbol>(Name("min"), file)->AbsoluteName().ToString();
			c.instructions.Add(name);

			Instruction value = Instruction(integer.value2);
			value.arguments.Add(integer.value3);
			c.instructions.Add(value);
		}

		if (usedVariables.Contains(integer.value1->Find<VariableSymbol>(Name("max"), file))) {
			Instruction name = Instruction(InstructionType::Static);
			name.instructionName = integer.value1->Find<VariableSymbol>(Name("max"), file)->AbsoluteName().ToString();
			c.instructions.Add(name);

			Instruction value = Instruction(integer.value2);
			value.arguments.Add(integer.value4);
			c.instructions.Add(value);
		}
	}

	// Compile code
	UByte index = 0;
	OldCompileInfo info;
	c.instructions.Add(Instruction(InstructionType::Code));
	c.AddInstructions(Compile(info).instructions);
	return c.instructions;
}

void RootNode::IncludeScan(ParsingInfo& info) {
	includeScanning = true;

	Collection<UInt> failedNodes;
	Collection<UInt> failedFuncs;

	do {
		// Create template symbols
		for (; templateIndex < SymbolTable::templateSymbols.Count(); templateIndex++) {
			SymbolTable::TemplateInfo info = SymbolTable::templateSymbols[templateIndex];
			AddTemplateSpecialization(info.name, info.scope->AbsoluteName(), info.file, false);
		}

		// Scan failed nodes
		for (UInt i = 0; i < failedNodes.Count();) {
			try {
				nodes[failedNodes[i]]->IncludeScan(info);
				failedNodes.RemoveAt(i);
			}
			catch (IncludeScanError& e) {
				i++;
			}
		}

		// Scan failed functions
		for (UInt i = 0; i < failedFuncs.Count();) {
			try {
				funcs[failedFuncs[i]]->IncludeScan(info);
				failedFuncs.RemoveAt(i);
			}
			catch (IncludeScanError& e) {
				i++;
			}
		}

		// Scan nodes
		for (; nodeIndex < nodes.Count(); nodeIndex++) {
			try {
				nodes[nodeIndex]->IncludeScan(info);
			}
			catch (IncludeScanError& e) {
				failedNodes.Add(nodeIndex);
			}
		}

		// Scan functions
		for (; funcIndex < funcs.Count(); funcIndex++) {
			try {
				funcs[funcIndex]->IncludeScan(info);
			}
			catch (IncludeScanError& e) {
				failedFuncs.Add(nodeIndex);
			}
		}
	}
	// Chech if scan is complete
	while (
		nodeIndex < nodes.Count() ||
		funcIndex < funcs.Count() ||
		!failedNodes.IsEmpty() ||
		!failedFuncs.IsEmpty() ||
		templateIndex < SymbolTable::templateSymbols.Count()
	);

	includeScanning = false;
}

void RootNode::AddTemplateSpecialization(const NameList& name, const NameList& scope, const FileInfo& file, const bool scan) {
	// Find template args
	Tuple<TemplateTypeSymbol*, List<NameList>> templateInfo = FindTemplateArgs(name, scope, file);

	if (templateInfo.value1 == nullptr) return;

	Name templateScope = templateInfo.value1->Name().Copy();
	templateScope.types = templateInfo.value2;

	// Check if the type has already been specialized
	if (SymbolTable::ContainsAbsolute(templateInfo.value1->AbsoluteName().Pop().Add(templateScope))) return;

	ReplacementMap<TypeSymbol*> templateTypes;

	// Create replacement map for template arguments
	for (UInt i = 0; i < templateInfo.value2.Count(); i++) {
		if (TypeSymbol* const type = templateInfo.value1->TemplateArgument(i)) {
			if (type->Is<TemplateSymbol>()) {
				if (TypeSymbol* const t = SymbolTable::FindAbsolute<TypeSymbol>(templateInfo.value2[i], file)) {
					templateTypes.Add(type, t);
				}
			}
		}
	}

	// Specialize template type
	Symbol* const s = templateInfo.value1->SpecializeTemplate(templateTypes, this);

	// Create template specialization for struct
	if (StructSymbol* const sym = s->Cast<StructSymbol>()) {
		templateInfo.value1->Parent()->Cast<TemplateTypeSymbol>()->AddTemplateVariant(sym);
		sym->templateParent = templateInfo.value1;

		Ptr<StructStatement> sn = new StructStatement(SymbolTable::FindAbsolute(NameList(true), file), file);
		sn->name = sym->Parent()->Name();

		List<NameList> templateArgs;

		for (const NameList& arg : sym->templateArguments) {
			templateArgs.Add(arg);
		}

		sn->name.types = templateArgs;

		sn->symbol = sym;
		sym->node = sn;

		for (const Name& var : sym->members) {
			sn->vars.Add(var);
		}

		nodes.Add(Statements::FromStatement(sn));
	}

	// Scan the new type
	if (scan && !includeScanning) {
		IncludeScan(*parsingInfo);
	}
}

Tuple<TemplateTypeSymbol*, List<NameList>> RootNode::FindTemplateArgs(const NameList& name, const NameList& scope, const FileInfo& file) {
	List<NameList> templateArgs = name.Last().types->Copy();

	// Find absolute name for template types
	for (NameList& type : templateArgs) {
		if (Symbol* const s = SymbolTable::Find(type, scope, file)) {
			type = s->AbsoluteName();
		}
		else {
			return Tuple<TemplateTypeSymbol*, List<NameList>>(nullptr, List<NameList>());
		}
	}

	NameList list = name.Last().name.Length() == 0 ? name.Pop() : name.Pop().Add(Name(name.Last().name));

	// Find base template type symbol
	TemplateTypeSymbol* const sym = SymbolTable::Find<TemplateTypeSymbol>(list, scope, file);

	if (sym == nullptr) {
		return Tuple<TemplateTypeSymbol*, List<NameList>>(nullptr, templateArgs);
	}

	bool found = false;

	// Find best variant
	for (TemplateTypeSymbol* const variant : sym->templateVariants) {
		if (variant->templateArguments.Count() == templateArgs.Count()) {
			bool match = true;

			for (UInt i = 0; i < templateArgs.Count(); i++) {
				TypeSymbol* const symArg = variant->TemplateArgument(i);

				if (symArg->Is<TemplateSymbol>()) continue;
				if (symArg->AbsoluteName() == templateArgs[i]) continue;

				match = false;
				break;
			}

			if (match) {
				return Tuple<TemplateTypeSymbol*, List<NameList>>(variant, templateArgs);
			}
		}
	}

	// TODO: Better error
	ErrorLog::Error(LogMessage::Message("template error"), file);
	return Tuple<TemplateTypeSymbol*, List<NameList>>(nullptr, templateArgs);
}

ScanResult RootNode::Scan(ScanInfoStack& info) {
	info->useFunction = true;

	// Scan nodes
	for (Weak<Statements> statements : nodes) {
		statements->Scan(info);
	}

	// Scan functions
	while (!info.functions.IsEmpty()) {
		Collection<Weak<FunctionBody>> functions = info.functions;
		info.functions = Collection<Weak<FunctionBody>>();

		for (Weak<FunctionBody> func : functions) {
			if (func) {
				func->isUsed = true;
				func->Scan(info);
			}
			else {
				// TODO: Remove
				ErrorLog::Error(LogMessage::Message("func null error (this should not happen)"), file);
			}
		}
	}

	info->useFunction = false;

	// Scan unused functions
	for (Weak<FunctionBody> node : funcs) {
		if (!info.usedFunctions.Contains(node->sym)) {
			node->isUsed = false;
			node->Scan(info);
		}
	}

	return ScanResult();
}

ScanInfoStack RootNode::Scan() {
	ScanInfoStack info;
	info.Get().scopeInfo.type = ScopeInfo::ScopeType::Main;
	Scan(info);
	return info;
}

void RootNode::Optimize(OptimizeInfo& info) {
	// Optimize nodes
	for (UInt i = 0; i < nodes.Count(); i++) {
		FileInfo file = nodes[i]->file;
		Node::Optimize(nodes[i], info);
		nodes[i]->file = file;
	}

	// Remove unused functions
	if (info.usedFunctions.Count() < funcs.Count()) {
		List<Ptr<FunctionBody>> functions = funcs;
		funcs = List<Ptr<FunctionBody>>(info.usedFunctions.Count());

		for (Ptr<FunctionBody> func : functions) {
			if (info.usedFunctions.Contains(func->sym)) {
				funcs.Add(func);
			}
		}

		info.optimized = true;
	}

	// Optimize functions
	for (UInt i = 0; i < funcs.Count(); i++) {
		funcs[i]->Optimize(info);
	}
}

StringBuilder RootNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}

void RootNode::ToMelonFiles(const CompilerOptions& options) const {
	// Create output directory
	const String dir = options.outputDirectory + "melon/";
	System::CreateDirectory(options.outputDirectory + "melon");

	NameList currentNamespace = NameList::undefined;
	String fileDir = dir;

	Regex filename = Regex("~[/\\]+%.melon$");

	bool writeToFile = false;
	StringBuilder sb;

	// Convert nodes to melon files
	for (Weak<Statements> statements : nodes) {
		// Check if the namespace changes
		if (statements->file.currentNamespace != currentNamespace) {
			// Write old namespace contents to file
			if (writeToFile) {
				FileWriter file = FileWriter(fileDir);
				file.Write(sb.ToString());
				file.Close();
			}
			else {
				writeToFile = true;
			}

			currentNamespace = statements->file.currentNamespace;

			fileDir = dir;

			// Create directory for new namespace
			for (UInt i = 0; i < currentNamespace.Size(); i++) {
				if (i > 0) fileDir += "/";
				fileDir += currentNamespace[i].ToString();

				if (!System::DirectoryExists(fileDir)) {
					System::CreateDirectory(fileDir);
				}
			}

			// Get file name
			if (fileDir.Length() != 0) fileDir += "/";

			if (Optional<Match> match = filename.Match(statements->file.filename)) {
				fileDir += match->match;
			}

			sb = StringBuilder();

			// Convert includes to string
			for (const NameList& include : statements->file.includedNamespaces) {
				sb += "include ";
				sb += include.ToString();
				sb += "\n";
			}

			if (sb.Length() > 0) sb += "\n";
		}

		// Convert node to melon string
		StringBuilder s = statements->ToMelon(0);

		if (s.Length() > 0) {
			sb += s;
			sb += "\n";
		}
	}

	// Write last text to file
	if (writeToFile) {
		FileWriter file = FileWriter(fileDir);
		file.Write(sb.ToString());
		file.Close();
	}
}
