#include "RootNode.h"

#include "StructNode.h"
#include "StatementsNode.h"
#include "FunctionNode.h"
#include "EmptyNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/MelonCompiler.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/StructSymbol.h"

#include "Boxx/System.h"
#include "Boxx/Regex.h"
#include "Boxx/File.h"
#include "Boxx/Math.h"
#include "Boxx/ReplacementMap.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RootNode::RootNode() : Node(nullptr, FileInfo()) {

}

RootNode::~RootNode() {

}

CompiledNode RootNode::Compile(CompileInfo& info) {
	CompiledNode cn;

	Pointer<StatementsNode> statements = new StatementsNode(nullptr, FileInfo());
	statements->statements = nodes;
	UInt size = statements->GetSize();

	if (size > 0) {
		OptimizerInstruction push = Instruction(InstructionType::Push, size);
		push.important = true;
		cn.instructions.Add(push);
	}

	info.stack.PushFrame(size);

	for (const NodePtr& node : nodes) {
		for (const OptimizerInstruction& instruction : node->Compile(info).instructions) {
			cn.instructions.Add(instruction);
		}
	}

	if (size > 0) {
		OptimizerInstruction pop = Instruction(InstructionType::Pop, size);
		pop.important = true;
		cn.instructions.Add(pop);
	}

	info.stack.PopFrame(size);

	Instruction in = Instruction(InstructionType::Exit, info.stack.ptrSize);
	in.arguments.Add(Argument(0));
	cn.instructions.Add(in);

	for (const NodePtr& node : funcs) {
		for (const OptimizerInstruction& instruction : node->Compile(info).instructions) {
			cn.instructions.Add(instruction);
		}
	}

	return cn;
}

List<OptimizerInstruction> RootNode::Compile(const Set<ScopeList>& usedVariables) {
	CompiledNode c;

	List<Tuple<ScopeList, InstructionType, Long, Long>> integers;
	integers.Add(Tuple<ScopeList, InstructionType, Long, Long>(ScopeList::Byte,   InstructionType::Byte,  Math::ByteMin(),   Math::ByteMax()));
	integers.Add(Tuple<ScopeList, InstructionType, Long, Long>(ScopeList::UByte,  InstructionType::Byte,  Math::UByteMin(),  Math::UByteMax()));
	integers.Add(Tuple<ScopeList, InstructionType, Long, Long>(ScopeList::Short,  InstructionType::Short, Math::ShortMin(),  Math::ShortMax()));
	integers.Add(Tuple<ScopeList, InstructionType, Long, Long>(ScopeList::UShort, InstructionType::Short, Math::UShortMin(), Math::UShortMax()));
	integers.Add(Tuple<ScopeList, InstructionType, Long, Long>(ScopeList::Int,    InstructionType::Int,   Math::IntMin(),    Math::IntMax()));
	integers.Add(Tuple<ScopeList, InstructionType, Long, Long>(ScopeList::UInt,   InstructionType::Int,   Math::UIntMin(),   Math::UIntMax()));
	integers.Add(Tuple<ScopeList, InstructionType, Long, Long>(ScopeList::Long,   InstructionType::Long,  Math::LongMin(),   Math::LongMax()));
	integers.Add(Tuple<ScopeList, InstructionType, Long, Long>(ScopeList::ULong,  InstructionType::Long,  Math::ULongMin(),  Math::ULongMax()));

	for (const Tuple<ScopeList, InstructionType, Long, Long>& integer : integers) {
		if (usedVariables.Contains(integer.value1.Add(Scope("min")))) {
			Instruction name = Instruction(InstructionType::Static);
			name.instructionName = integer.value1.Add(Scope("min")).ToString();
			c.instructions.Add(name);

			Instruction value = Instruction(integer.value2);
			value.arguments.Add(integer.value3);
			c.instructions.Add(value);
		}

		if (usedVariables.Contains(integer.value1.Add(Scope("max")))) {
			Instruction name = Instruction(InstructionType::Static);
			name.instructionName = integer.value1.Add(Scope("max")).ToString();
			c.instructions.Add(name);

			Instruction value = Instruction(integer.value2);
			value.arguments.Add(integer.value4);
			c.instructions.Add(value);
		}
	}

	UByte index = 0;
	CompileInfo info;
	c.instructions.Add(Instruction(InstructionType::Code));
	c.AddInstructions(Compile(info).instructions);
	return c.instructions;
}

void RootNode::IncludeScan(ParsingInfo& info) {
	includeScanning = true;

	Collection<UInt> failedNodes;
	Collection<UInt> failedFuncs;

	do {
		for (UInt i = 0; i < failedNodes.Size();) {
			try {
				nodes[failedNodes[i]]->IncludeScan(info);
				failedNodes.RemoveAt(i);
			}
			catch (IncludeScanError& e) {
				i++;
			}
		}

		for (UInt i = 0; i < failedFuncs.Size();) {
			try {
				funcs[failedFuncs[i]]->IncludeScan(info);
				failedFuncs.RemoveAt(i);
			}
			catch (IncludeScanError& e) {
				i++;
			}
		}

		for (; nodeIndex < nodes.Size(); nodeIndex++) {
			try {
				nodes[nodeIndex]->IncludeScan(info);
			}
			catch (IncludeScanError& e) {
				failedNodes.Add(nodeIndex);
			}
		}

		for (; funcIndex < funcs.Size(); funcIndex++) {
			try {
				funcs[funcIndex]->IncludeScan(info);
			}
			catch (IncludeScanError& e) {
				failedFuncs.Add(nodeIndex);
			}
		}

		for (; templateIndex < SymbolTable::templateSymbols.Size(); templateIndex++) {
			SymbolTable::TemplateInfo info = SymbolTable::templateSymbols[templateIndex];
			AddTemplateSpecialization(info.name, info.scope->AbsoluteName(), info.file, false);
		}
	}
	while (
		nodeIndex < nodes.Size() ||
		funcIndex < funcs.Size() ||
		!failedNodes.IsEmpty() ||
		!failedFuncs.IsEmpty() ||
		templateIndex < SymbolTable::templateSymbols.Size()
	);

	includeScanning = false;
}

void RootNode::AddTemplateSpecialization(const ScopeList& name, const ScopeList& scope, const FileInfo& file, const bool scan) {
	Tuple<TemplateTypeSymbol*, List<ScopeList>> templateInfo = FindTemplateArgs(name, scope, file);

	if (templateInfo.value1 == nullptr) return;

	Scope templateScope = templateInfo.value1->Name().Copy();
	templateScope.types = templateInfo.value2;

	if (SymbolTable::ContainsAbsolute(templateInfo.value1->AbsoluteName().Pop().Add(templateScope))) return;

	ReplacementMap<TypeSymbol*> templateTypes;

	for (UInt i = 0; i < templateInfo.value2.Size(); i++) {
		if (TypeSymbol* const type = templateInfo.value1->TemplateArgument(i)) {
			if (type->Is<TemplateSymbol>()) {
				templateTypes.Add(type, SymbolTable::FindAbsolute<TypeSymbol>(templateInfo.value2[i], file));
			}
		}
	}

	Symbol* const s = templateInfo.value1->SpecializeTemplate(templateTypes, this);

	if (StructSymbol* const sym = s->Cast<StructSymbol>()) {
		templateInfo.value1->Parent()->Cast<TemplateTypeSymbol>()->AddTemplateVariant(sym);

		Pointer<StructNode> sn = new StructNode(SymbolTable::FindAbsolute(ScopeList(true), file), file);
		sn->name = sym->Parent()->Name();

		List<ScopeList> templateArgs;

		for (const ScopeList& arg : sym->templateArguments) {
			templateArgs.Add(arg);
		}

		sn->name.types = templateArgs;

		sn->symbol = sym;

		for (const Scope& var : sym->members) {
			sn->vars.Add(var);
		}

		nodes.Add(sn);
	}

	if (scan && !includeScanning) {
		IncludeScan(*parsingInfo);
	}
}

Tuple<TemplateTypeSymbol*, List<ScopeList>> RootNode::FindTemplateArgs(const ScopeList& name, const ScopeList& scope, const FileInfo& file) {
	List<ScopeList> templateArgs = name.Last().types.Get().Copy();

	for (ScopeList& type : templateArgs) {
		type = SymbolTable::Find(type, scope, file)->AbsoluteName();
	}

	Scope last = Scope(name.Last().name);

	TemplateTypeSymbol* const sym = SymbolTable::Find<TemplateTypeSymbol>(name.Pop().Add(last), scope, file);

	if (sym == nullptr) {
		return Tuple<TemplateTypeSymbol*, List<ScopeList>>(nullptr, templateArgs);
	}

	bool found = false;

	for (TemplateTypeSymbol* const variant : sym->templateVariants) {
		if (variant->templateArguments.Size() == templateArgs.Size()) {
			bool match = true;

			for (UInt i = 0; i < templateArgs.Size(); i++) {
				TypeSymbol* const symArg = variant->TemplateArgument(i);

				if (symArg->Is<TemplateSymbol>()) continue;
				if (symArg->AbsoluteName() == templateArgs[i]) continue;

				match = false;
				break;
			}

			if (match) {
				return Tuple<TemplateTypeSymbol*, List<ScopeList>>(variant, templateArgs);
			}
		}
	}

	// TODO: Better error
	ErrorLog::Error(SymbolError("template error", file));
	return Tuple<TemplateTypeSymbol*, List<ScopeList>>(nullptr, templateArgs);
}

ScanResult RootNode::Scan(ScanInfoStack& info) {
	info.Get().useFunction = true;

	for (const NodePtr& node : nodes) {
		node->Scan(info);
	}

	while (!info.functions.IsEmpty()) {
		Collection<NodePtr> functions = info.functions;
		info.functions = Collection<NodePtr>();

		for (const NodePtr& func : functions) {
			if (func) {
				func.Cast<FunctionNode>()->isUsed = true;
				func->Scan(info);
			}
			else {
				// TODO: Remove
				ErrorLog::Error(CompileError("func null error", file));
			}
		}
	}

	info.Get().useFunction = false;

	for (const NodePtr& node : funcs) {
		if (!info.usedFunctions.Contains(node.Cast<FunctionNode>()->sym)) {
			node.Cast<FunctionNode>()->isUsed = false;
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

NodePtr RootNode::Optimize(OptimizeInfo& info) {
	for (UInt i = 0; i < nodes.Size(); i++) {
		FileInfo file = nodes[i]->file;

		if (NodePtr node = nodes[i]->Optimize(info)) nodes[i] = node;

		nodes[i]->file = file;
	}

	if (info.usedFunctions.Size() < funcs.Size()) {
		List<NodePtr> functions = funcs;
		funcs = List<NodePtr>(info.usedFunctions.Size());

		for (const NodePtr& func : functions) {
			if (info.usedFunctions.Contains(func.Cast<FunctionNode>()->sym)) {
				funcs.Add(func);
			}
		}

		info.optimized = true;
	}

	for (UInt i = 0; i < funcs.Size(); i++) {
		if (NodePtr node = funcs[i]->Optimize(info)) funcs[i] = node;

		if (IsEmpty(funcs[i])) {
			funcs.RemoveAt(i);
			i--;
		}
	}

	return nullptr;
}

StringBuilder RootNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}

void RootNode::ToMelonFiles(const CompilerOptions& options) const {
	const String dir = options.outputDirectory + "melon/";
	System::CreateDirectory(options.outputDirectory + "melon");

	ScopeList currentNamespace = ScopeList::undefined;
	String fileDir = dir;

	Regex filename = Regex("~[/\\]+%.melon$");

	bool writeToFile = false;
	StringBuilder sb;

	for (const NodePtr& node : nodes) {
		if (node->file.currentNamespace != currentNamespace) {
			if (writeToFile) {
				FileWriter file = FileWriter(fileDir);
				file.Write(sb.ToString());
				file.Close();
			}
			else {
				writeToFile = true;
			}

			currentNamespace = node->file.currentNamespace;

			fileDir = dir;

			for (UInt i = 0; i < currentNamespace.Size(); i++) {
				if (i > 0) fileDir += "/";
				fileDir += currentNamespace[i].ToString();

				if (!System::DirectoryExists(fileDir)) {
					System::CreateDirectory(fileDir);
				}
			}

			if (fileDir.Size() != 0) fileDir += "/";

			if (Optional<Match> match = filename.Match(node->file.filename)) {
				fileDir += match.Get().match;
			}

			sb = StringBuilder();

			for (const ScopeList& include : node->file.includedNamespaces) {
				sb += "include ";
				sb += include.ToString();
				sb += "\n";
			}

			if (sb.Size() > 0) sb += "\n";
		}

		sb += node->ToMelon(0);
		sb += "\n";
	}

	if (writeToFile) {
		FileWriter file = FileWriter(fileDir);
		file.Write(sb.ToString());
		file.Close();
	}
}

String RootNode::ToString() const {
	/* TODO: node
	return Mango::Encode(ToMango(), true);
	*/

	return "";
}
