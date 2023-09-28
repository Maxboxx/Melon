#include "RootNode.h"

#include "StructStatement.h"
#include "FunctionBody.h"
#include "EmptyStatement.h"

#include "Melon/Parsing/Parser.h"

#include "Melon/MelonCompiler.h"

#include "Melon/Symbols/TemplateSymbol.h"
#include "Melon/Symbols/TemplateTypeSymbol.h"
#include "Melon/Symbols/StructSymbol.h"
#include "Melon/Symbols/ClassSymbol.h"
#include "Melon/Symbols/EnumSymbol.h"
#include "Melon/Symbols/VariableSymbol.h"
#include "Melon/Symbols/IntegerSymbol.h"

#include "Boxx/System.h"
#include "Boxx/Regex.h"
#include "Boxx/File.h"
#include "Boxx/Math.h"
#include "Boxx/ReplacementMap.h"
#include "Boxx/Path.h"

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

Ptr<Kiwi::Value> RootNode::Compile(CompileInfo& info) {
	Ptr<Kiwi::CodeBlock> codeBlock = new Kiwi::CodeBlock();
	info.SetCodeBlock(codeBlock);

	info.ClearRegisters();

	// Compile nodes
	for (Weak<Statements> statements : nodes) {
		statements->Compile(info);
	}

	info.program->AddCodeBlock(codeBlock);
	info.currentBlock = nullptr;

	// Compile functions
	for (Weak<FunctionBody> func : funcs) {
		info.ClearRegisters();
		func->Compile(info);
	}

	return nullptr;
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
	Symbol* const s = templateInfo.value1->InitializeSpecialize();

	// Create template specialization for struct
	if (StructSymbol* const sym = s->Cast<StructSymbol>()) {
		SpecializeType<StructSymbol, StructStatement>(sym, templateInfo.value1, templateTypes);
	}
	// Create template specialization for class
	else if (ClassSymbol* const sym = s->Cast<ClassSymbol>()) {
		SpecializeType<ClassSymbol, ClassStatement>(sym, templateInfo.value1, templateTypes);
	}
	// Create template specialization for enum
	else if (EnumSymbol* const sym = s->Cast<EnumSymbol>()) {
		SpecializeType<EnumSymbol, EnumStatement>(sym, templateInfo.value1, templateTypes);
	}
	else {
		templateInfo.value1->SpecializeTemplate(s, templateTypes, this);
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
	const String dir = Path::Combine(options.outputDirectory, "melon");
	System::CreateDirectory(dir);

	NameList currentNamespace = NameList::undefined;
	String fileDir = dir;

	bool writeToFile = false;
	StringBuilder sb;

	// Convert nodes to melon files
	for (Weak<Statements> statements : nodes) {
		// Check if the namespace changes
		if (statements->file.currentNamespace != currentNamespace) {
			// Write old namespace contents to file
			if (writeToFile) {
				FileWriter::WriteText(fileDir, sb.ToString());
			}
			else {
				writeToFile = true;
			}

			currentNamespace = statements->file.currentNamespace;

			fileDir = dir;

			// Create directory for new namespace
			for (UInt i = 0; i < currentNamespace.Size(); i++) {
				fileDir = Path::Combine(fileDir, currentNamespace[i].ToString());

				if (!System::DirectoryExists(fileDir)) {
					System::CreateDirectory(fileDir);
				}
			}

			fileDir = Path::Combine(fileDir, Path::GetFile(statements->file.filename));

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
		FileWriter::WriteText(fileDir, sb.ToString());
	}
}
