#include "RootNode.h"

#include "StructNode.h"
#include "StatementsNode.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/MelonCompiler.h"

#include "Boxx/System.h"
#include "Boxx/Regex.h"
#include "Boxx/File.h"

using namespace Boxx;
using namespace Kiwi;

using namespace Melon;
using namespace Melon::Nodes;
using namespace Melon::Symbols;
using namespace Melon::Parsing;
using namespace Melon::Optimizing;

RootNode::RootNode() : Node(ScopeList(), FileInfo()) {

}

RootNode::~RootNode() {

}

CompiledNode RootNode::Compile(CompileInfo& info) {
	CompiledNode cn;

	Pointer<StatementsNode> statements = new StatementsNode(ScopeList(), FileInfo());
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

List<OptimizerInstruction> RootNode::Compile() {
	UByte index = 0;
	CompileInfo info;
	return Compile(info).instructions;
}

void RootNode::IncludeScan(ParsingInfo& info) {
	UInt nodeIndex = 0;
	UInt funcIndex = 0;
	UInt templateIndex = 0;

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

		for (; templateIndex < Symbol::templateSymbols.Size(); templateIndex++) {
			Tuple<Symbol, List<ScopeList>> templateInfo = Symbol::FindTemplateArgs(Symbol::templateSymbols[templateIndex]);
			Scope templateScope = templateInfo.value1.scope.Last().Copy();
			templateScope.types = templateInfo.value2;
			templateScope.variant = nullptr;

			if (Symbol::Contains(templateInfo.value1.scope.Pop().Add(templateScope))) continue;

			Scope last = templateInfo.value1.scope.Last();
			last.variant = nullptr;
			last.types   = nullptr;

			Symbol templateSym = Symbol::Find(templateInfo.value1.scope.Pop().Add(last), file);
			templateSym.templateVariants.Add(Symbol(templateInfo.value1.type));

			Symbol& s = templateSym.templateVariants.Last();

			templateInfo.value1.SpecializeTemplate(s, templateInfo.value2, info);

			if (s.type == SymbolType::Struct) {
				Pointer<StructNode> sn = new StructNode(ScopeList(true), Symbol::templateSymbols[templateIndex].file);
				sn->name = s.scope.Last();

				List<ScopeList> templateArgs;

				for (const ScopeList& arg : s.templateArgs) {
					templateArgs.Add(arg);
				}

				sn->name.types = templateArgs;
				sn->name.variant = nullptr;

				sn->symbol = s;
				
				for (const Scope& var : s.names) {
					sn->vars.Add(var);
				}

				nodes.Add(sn);
			}
		}
	}
	while (
		nodeIndex < nodes.Size() ||
		funcIndex < funcs.Size() ||
		!failedNodes.IsEmpty() ||
		!failedFuncs.IsEmpty() ||
		templateIndex < Symbol::templateSymbols.Size()
	);
}

Set<ScanType> RootNode::Scan(ScanInfoStack& info) {
	Set<ScanType> scanSet = Set<ScanType>();

	for (const NodePtr& node : nodes) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);
		}
	}

	for (const NodePtr& node : funcs) {
		for (const ScanType type : node->Scan(info)) {
			scanSet.Add(type);
		}
	}

	return scanSet;
}

ScanInfoStack RootNode::Scan() {
	ScanInfoStack info;
	info.Get().scopeInfo.type = ScopeInfo::ScopeType::Main;
	Scan(info);
	return info;
}

NodePtr RootNode::Optimize(OptimizeInfo& info) {
	for (UInt i = 0; i < nodes.Size(); i++) {
		if (NodePtr node = nodes[i]->Optimize(info)) nodes[i] = node;

		if (IsEmpty(nodes[i])) {
			nodes.RemoveAt(i);
			i--;
		}
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

Mango RootNode::ToMango() const {
	Mango mango = Mango("ast", MangoType::List);

	for (const NodePtr& node : nodes) {
		mango.Add(node->ToMango());
	}

	return mango;
}

StringBuilder RootNode::ToMelon(const UInt indent) const {
	return StringBuilder();
}

void RootNode::ToMelonFiles(const CompilerOptions& options) const {
	const String dir = options.outputDirectory + "melon/";
	System::CreateDirectory(options.outputDirectory + "melon");

	ScopeList currentNamespace = ScopeList(true);
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
	}

	if (writeToFile) {
		FileWriter file = FileWriter(fileDir);
		file.Write(sb.ToString());
		file.Close();
	}
}

String RootNode::ToString() const {
	return Mango::Encode(ToMango(), true);
}