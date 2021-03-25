#pragma once

#include "Types.h"
#include "String.h"
#include "StringBuilder.h"
#include "Set.h"
#include "File.h"
#include "Array.h"
#include "List.h"
#include "Map.h"
#include "Regex.h"
#include "Tuple.h"
#include "Optional.h"
#include "System.h"

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Dragonfruit

	///[Title] DragonfruitBlockMode
	/// The mode to use for blocks.
	///[Block] DragonfruitBlockMode
	enum class DragonfruitBlockMode : UByte {
		/// Blocks must be manually closed.
		Manual,

		/// Blocks are based on indentation.
		Indent
	};

	///[Title] DragonfruitInfo
	/// Contains information about the language.
	///[Block] DragonfruitInfo
	struct DragonfruitInfo {
		/// The start of a documentation comment.
		String docComment = "///";

		/// The separator for namespaces.
		String namespaceSeparator = ".";

		/// The file extensions to generate documentation for.
		List<String> extensions;

		/// The mode to use for blocks.
		DragonfruitBlockMode blockMode = DragonfruitBlockMode::Manual;

		/// The tab size to use.
		///[para] This value is only used if both spaces and tabs are used for indentation.
		UInt tabSize = 4;

		/// Regex patterns to use for automatically detecting blocks.
		///[para] The first group match is the block name, and any extra group matches are inherited blocks.
		List<Regex> blockPatterns;

		/// Regex patterns to use for automatically creating titles.
		///[para] The first group match is the title to use.
		List<Regex> titlePatterns;

		/// Regex patterns to use for automatically creating code.
		///[para] All group matches will be concatenated.
		List<Regex> codePatterns;

		/// Regex patterns to use for automatically creating headings in blocks.
		///[para] The pair key is the heading name and the value is the pattern.
		List<Pair<String, Regex>> blockHeadingPatterns;

		~DragonfruitInfo() {}
	};

	///[Title] DragonfruitDescType
	/// The description type of a {DragonfruitDesc}.
	///[Block] DragonfruitDescType
	enum class DragonfruitDescType : UByte {
		/// Used for the description root.
		Description,

		/// The description is a text.
		Text,

		/// The description is code.
		Code,

		/// The description is a list.
		List,

		/// The description is a list item.
		Item,

		/// The description is a paragraph.
		Paragraph
	};

	///[Title] DragonfruitDesc
	/// A documentation description.
	///[Block] DragonfruitDesc
	struct DragonfruitDesc {
		/// The description type.
		DragonfruitDescType type = DragonfruitDescType::Text;

		/// The text for the description.
		String text;

		/// All description segments.
		List<DragonfruitDesc> segments;

		DragonfruitDesc() {}

		explicit DragonfruitDesc(const String& text) {
			type = DragonfruitDescType::Text;
			AddText(text);
		}

		explicit DragonfruitDesc(const DragonfruitDescType type) {
			this->type = type;
		}

		DragonfruitDesc(const DragonfruitDescType type, const String& text) {
			this->type = type;
			AddText(text);
		}

		void AddText(const String& text) {
			switch (type) {
				case DragonfruitDescType::Description:
				case DragonfruitDescType::Item: {
					if (segments.IsEmpty()) {
						segments.Add(DragonfruitDesc(DragonfruitDescType::Paragraph, text));	
					}
					else {
						segments.Last().AddText(text);
					}

					break;
				}

				case DragonfruitDescType::Text: {
					if (this->text.Size() > 0) {
						this->text += " " + text;
					}
					else {
						this->text = text;
					}

					break;
				}

				case DragonfruitDescType::Paragraph: {
					bool escaped = false;
					bool isCode  = false;

					List<char> chars;

					for (UInt i = 0; i < text.Size(); i++) {
						if (text[i] == '\\') {
							escaped = !escaped;
						}
						else if (!escaped && text[i] == '}') {
							segments.Add(DragonfruitDesc(DragonfruitDescType::Code));
							segments.Last().text = String(chars);
							chars.Clear();
						}
						else if (i == text.Size() - 1 || (!escaped && text[i] == '{')) {
							if (text[i] != '{') chars.Add(text[i]);

							if (!chars.IsEmpty()) {
								if (segments.IsEmpty() || segments.Last().type != DragonfruitDescType::Text) {
									segments.Add(DragonfruitDesc(String(chars)));	
								}
								else {
									segments.Last().AddText(String(chars));
								}
							}

							chars.Clear();
						}
						else {
							escaped = false;
							chars.Add(text[i]);
						}
					}

					break;
				}

				case DragonfruitDescType::List: {
					if (!segments.IsEmpty()) {
						segments.Last().AddText(text);
					}

					break;
				}
			}
		}

		void AddItem(const UInt i, const String& text) {
			switch (type) {
				case DragonfruitDescType::List: {
					if (segments.IsEmpty()) {
						segments.Add(DragonfruitDesc(DragonfruitDescType::Item));
					}

					segments.Last().AddItem(i, text);

					break;
				}

				default: {
					if (segments.IsEmpty() || segments.Last().type != DragonfruitDescType::List) {
						segments.Add(DragonfruitDesc(DragonfruitDescType::List));
					}

					if (i == 1) {
						segments.Last().segments.Add(DragonfruitDesc(DragonfruitDescType::Item, text));
					}
					else if (i > 0) {
						segments.Last().AddItem(i - 1, text);
					}

					break;
				}
			}
		}

		void AddInItem(const UInt i, const DragonfruitDesc& desc) {
			if (i == 0) {
				segments.Add(desc);
				return;
			}

			switch (type) {
				case DragonfruitDescType::List: {
					if (segments.IsEmpty()) {
						segments.Add(DragonfruitDesc(DragonfruitDescType::Item));
					}

					segments.Last().AddInItem(i, desc);

					break;
				}

				default: {
					if (segments.IsEmpty() || segments.Last().type != DragonfruitDescType::List) {
						segments.Add(DragonfruitDesc(DragonfruitDescType::List));
					}

					if (i == 1) {
						if (segments.Last().segments.IsEmpty()) {
							segments.Last().segments.Add(DragonfruitDesc(DragonfruitDescType::Item));
						}

						segments.Last().segments.Last().segments.Add(desc);
					}
					else if (i > 0) {
						segments.Last().AddInItem(i - 1, desc);
					}

					break;
				}
			}
		}

		~DragonfruitDesc() {}
	};

	///[Title] DragonfruitType
	/// The different types of documentation sections.
	///[Block] DragonfruitType
	enum class DragonfruitType : UByte {
		/// No type.
		None,

		/// A namespace section.
		Namespace,

		/// A heading section.
		Heading,

		/// A summary section.
		Summary,

		/// A category section.
		Category,

		/// A value section.
		Value,

		/// The main section.
		Main
	};

	class DragonfruitBuilder;

	///[Title] Dragonfruit
	/// Class for generating dragonfruit documentation.
	///[Block] Dragonfruit
	class Dragonfruit {
	public:
		///[Heading] Members

		/// The type of documentation section.
		DragonfruitType type = DragonfruitType::None;

		/// The title of the documentation section.
		String title;

		/// The block name of the section.
		String block;

		/// The file of the section.
		String file;

		/// Lines of code for the section.
		List<String> code;

		/// The description of the section.
		DragonfruitDesc description = DragonfruitDesc(DragonfruitDescType::Description);

		/// A list of all subsections to the documentation section.
		List<Dragonfruit> sections;

		Dragonfruit();
		~Dragonfruit();

		///[Heading] Methods

		/// Generates HTML files for the documentation.
		///[Arg] directory: The directory to generate the files in.
		void GenerateHTML(const String& directory) const;

		/// Generates HTML files for the documentation.
		///[Arg] directory: The directory to generate the files in.
		///[Arg] builder: The builder to use for creating the HTML files.
		void GenerateHTML(const String& directory, const DragonfruitBuilder& builder) const;

		/// Merges two documentation sections.
		void Merge(const Dragonfruit& dragonfruit);

		///[Heading] Static functions

		/// Generates documentation for all files in the specified directory.
		///[Arg] directory: The directory to generate documentation for.
		///[Arg] info: Info to use for generating documentation.
		static Dragonfruit Generate(const String& directory, const DragonfruitInfo& info);
		
		/// Generates documentation for a specific file.
		///[Arg] filename: The name of the file to generate documentation for.
		///[Arg] info: Info to use for generating documentation.
		///[Error] FileError: Thrown if a file error occurs.
		static Dragonfruit GenerateFromFile(const String& filename, const DragonfruitInfo& info);

	private:
		static Dragonfruit Generate(const String& directory, const DragonfruitInfo& info, const String& dir);
		static Dragonfruit GenerateFromFile(const String& filename, const DragonfruitInfo& info, const String& file);

		void GenerateHTMLFile(const DragonfruitBuilder& builder, const String& dir, const String& rootDir) const;
		void GenerateBody(FileWriter& file, const DragonfruitBuilder& builder, const String& dir, const String& rootDir) const;

		struct DocComment {
			String commentType = "";
			String content = "";
			UInt indent = 0;

			~DocComment() {}
		};

		struct ParseInfo;

		static List<DocComment> Import(const String& filename, const DragonfruitInfo& info);

		static void ParseLine(const String& line, ParseInfo& info, DragonfruitInfo& docInfo);
		static Optional<DocComment> ParseDocComment(const String& line, const ParseInfo& info, const DragonfruitInfo& docInfo);
		static void ParseComment(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);

		static bool ParseTitle(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);
		static bool ParseBlock(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);
		static bool ParseNamespace(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);
		static bool ParseHeading(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);
		static bool ParseCategory(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);
		static bool ParseValue(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);
		static bool ParseMulti(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);
		static bool ParseCode(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);

		static bool ParseImport(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);
		static bool ParseSettings(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);

		static bool ParsePara(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo, const UInt items);
		static bool ParseItem(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo, const UInt items);

		static bool ParseDescription(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo);
	};

	///[Title] DragonfruitBuilder
	/// A class used for building HTML files from documentation.
	///[Block] DragonfruitBuilder
	class DragonfruitBuilder {
	public:
		/// Returns the file extension to use html files.
		virtual String FileExtension() const {
			return "html";
		}

		/// Generates extra files in the root directory.
		///[Arg] dir: The root directory for the documents.
		virtual void GenerateRootFiles(const String& dir) const {
			FileWriter file = FileWriter(dir + "style.css");
			file.Write(R"(
html {font-family: sans-serif;}
code, pre {background-color: #e4e6e8;font-family: 'Consolas', monospace;}
pre {font-size: 17px;padding: 5px 10px;margin: 5px;}
code {padding: 1px 4px;}
.value {display: inline-block;margin-top: 2px;margin-bottom: -5px;margin-left: 0px;padding: 2px 4px;font-size: 17px;}
pre, namespace-item {display: inline-block;}
heading-list {display: block;padding-top: 20px;margin-bottom: 40px;border-top: solid 1px #888;}
heading-title {display: block;font-size: 20px;font-weight: bold;margin-top: 20px;}
heading-section a {color: inherit;text-decoration: none;display: block;}
heading-section ul {list-style-type: none;margin: 5px;padding: 0px;}
heading-section li {margin: 0px;background-color: #e4e6e8;}
heading-section li:nth-child(2n), heading-section li:nth-child(2n) pre {background-color: #eff2f4;}
heading-section :is(li:hover, li:hover pre) {background-color: #ccddf4;}
namespace-item {padding: 5px 10px;margin: 5px;font-size: 17px;}
summary-title {display: block;padding: 10px 15px;font-size: 17px;}
summary-content {display: block;padding: 20px;border-top: solid 1px #888;}
summary-content > summary-title {font-size: 17px;font-weight: bold;padding: 0px;}
summary-file {display: block; font-size: 14px;}
summary-file::before {content: 'file: ';}
document-summary > summary-title {font-size: 22px;font-weight: bold;padding: 0px;}
document-summary {display: block;padding-bottom: 20px;}
document-summary summary-content {border-top: none;}
summary-content > pre {margin-left: 0px;}
description-text {display: block;margin-left: 10px;margin-top: -5px;}
category-section {display: block;margin-top: 20px;}
category-title {display: block;font-size: 16px;font-weight: bold;}
value-section {display: block;margin-left: 10px;margin-top: 10px;})");
			file.Close();
		}

		/// Generates the start of a document.
		///[Arg] title: The title of the document.
		///[Arg] rootDir: The root directory for the documents.
		virtual String DocumentStart(const String& title, const String& rootDir) const {
			return "<!DOCTYPE html><html><head><title>" + EscapeHTML(title) + "</title><meta charset='utf-8'/><link rel='stylesheet' href='" + EscapeHTML(rootDir) + "style.css'/></head><body>";
		}

		/// Generates the end of a document.
		///[Arg] rootDir: The root directory for the documents.
		virtual String DocumentEnd(const String& rootDir) const {
			return "</body></html>";
		}

		/// Generates the start of the heading list.
		virtual String HeadingListStart() const {
			return "<heading-list>";
		}

		/// Generates the end of the heading list.
		virtual String HeadingListEnd() const {
			return "</heading-list>";
		}

		/// Generates the start of a heading section.
		///[Arg] heading: The heading title.
		virtual String HeadingStart(const String& heading) const {
			if (heading.Size() > 0) {
				return "<heading-section><heading-title>" + EscapeHTML(heading) + "</heading-title><ul>";
			}
			else {
				return "<heading-section><ul>";
			}
		}

		/// Generates the end of a heading section.
		///[Arg] heading: The heading title.
		virtual String HeadingEnd(const String& heading) const {
			return "</ul></heading-section>";
		}

		/// Generates a block item.
		///[Arg] item: The block item.
		///[Arg] id: The id of the item.
		virtual String Item(const Dragonfruit& item, const UInt id) const {
			switch (item.type) {
				case DragonfruitType::Namespace: return "<li><a href='./" + EscapeHTML(item.title.Lower()) + "/index." + FileExtension() + "'><namespace-item>" + EscapeHTML(item.title) + "</namespace-item></a></li>";

				case DragonfruitType::Summary: {
					StringBuilder str;
					
					if (item.block.Size() > 0) {
						str += "<li><a href='";
						str += EscapeHTML(item.block.Lower());
						str += ".";
						str += FileExtension();
						str += "'><summary-item>";
					}
					else {
						str += "<li><a href='#summary";
						str += String::ToString(id);
						str += "'><summary-item>";
					}

					if (item.title.Size() > 0) {
						str += "<summary-title>";
						str += EscapeHTML(item.title);
						str += "</summary-title>";
					}
					else if (!item.code.IsEmpty()) {
						str += "<pre>";

						for (UInt i = 0; i < item.code.Size(); i++) {
							if (i > 0) str += "\n";
							str += EscapeHTML(item.code[i]);
						}

						str += "</pre>";
					}

					str += "</summary-item></a></li>";
					return (String)str;
				}
			}

			return "";
		}

		/// Generates a document summary.
		///[Arg] section: The summary section.
		virtual String DocumentSummary(const Dragonfruit& section) const {
			switch (section.type) {
				case DragonfruitType::Main: return "";

				case DragonfruitType::Namespace: {
					StringBuilder str = "<document-summary><summary-title>Namespace: ";
					str += EscapeHTML(section.title);
					str += "</summary-title></document-summary>";
					return (String)str;
				}

				default: {
					StringBuilder str = "<document-summary>";
					str += Summary(section, nullptr);
					str += "</document-summary>";
					return (String)str;
				}
			}
		}

		/// Generates a summary of a summary item.
		///[Arg] section: The summary item.
		///[Arg] id: The id of the item.
		virtual String Summary(const Dragonfruit& section, const Optional<UInt> id) const {
			StringBuilder str;
			
			if (id) {
				str += "<summary-content id='summary";
				str += String::ToString(*id);
				str += "'>";
			}
			else {
				str += "<summary-content>";
			}

			if (section.title.Size() > 0) {
				str += "<summary-title>";
				str += EscapeHTML(section.title);
				str += "</summary-title>";
			}

			if (section.file.Size() > 0 && section.block.Size() > 0) {
				str += "<summary-file>";
				str += section.file;
				str += "</summary-file>";
			}

			str += "<pre>";

			for (UInt i = 0; i < section.code.Size(); i++) {
				if (i > 0) str += "\n";
				str += EscapeHTML(section.code[i]);
			}

			str += "</pre>";

			str += Description(section.description);

			for (const Dragonfruit& s : section.sections) {
				if (s.type == DragonfruitType::Category) {
					str += Category(s);
				}
			}

			str += "</summary-content>";
			return (String)str;
		}

		/// Generates a category.
		///[Arg] section: The category section.
		virtual String Category(const Dragonfruit& section) const {
			StringBuilder str = "<category-section><category-title>";
			str += EscapeHTML(section.title);
			str += "</category-title>";

			str += Description(section.description);

			for (const Dragonfruit& s : section.sections) {
				if (s.type == DragonfruitType::Value) {
					str += Value(s);
				}
			}

			str += "</category-section>";
			return (String)str;
		}

		/// Generates a category value.
		///[Arg] section: The value section.
		virtual String Value(const Dragonfruit& section) const {
			StringBuilder str = "<value-section>";
			
			if (section.title.Size() > 0) {
				str += "<code class='value'>";
				str += EscapeHTML(section.title);
				str += "</code>";
			}

			str += Description(section.description);
			str += "</value-section>";
			return (String)str;
		}

		/// Generates a description.
		///[Arg] description: The description.
		virtual String Description(const DragonfruitDesc& description) const {
			switch (description.type) {
				case DragonfruitDescType::Description: {
					StringBuilder str = "<description-text>";

					for (const DragonfruitDesc& desc : description.segments) {
						str += Description(desc);
					}

					str += "</description-text>";
					return (String)str;
				}

				case DragonfruitDescType::Paragraph: {
					StringBuilder str = "<p>";

					for (const DragonfruitDesc& desc : description.segments) {
						str += Description(desc);
					}

					str += "</p>";
					return (String)str;
				}

				case DragonfruitDescType::Text: {
					return EscapeHTML(description.text);
				}

				case DragonfruitDescType::Code: {
					return "<code>" + EscapeHTML(description.text) + "</code>";
				}

				case DragonfruitDescType::List: {
					StringBuilder str = "<ul>";

					for (const DragonfruitDesc& desc : description.segments) {
						str += Description(desc);
					}

					str += "</ul>";
					return (String)str;
				}

				case DragonfruitDescType::Item: {
					StringBuilder str = "<li>";

					for (const DragonfruitDesc& desc : description.segments) {
						str += Description(desc);
					}

					str += "</li>";
					return (String)str;
				}
			}

			return "";
		}

		///[Heading] Static functions

		/// Escapes characters in {str} with special meaning in HTML.
		static String EscapeHTML(const String& str) {
			return str.Replace("&", "&amp;").Replace("<", "&lt;").Replace(">", "&gt;").Replace("\"", "&quot;").Replace("'", "&#39;");
		}
	};

	struct Dragonfruit::ParseInfo {
		Regex commentPattern;

		List<Dragonfruit> completed;
		List<Dragonfruit> sections;
		List<UInt> indents;
		Dragonfruit current;
		UInt currentIndent = 0;

		String currentCategory = "";

		String directory;
		String file;
		Map<String, List<DocComment>> imported;

		bool multi = false;
		bool ignoreCode = false;

		~ParseInfo() {}

		void End() {
			if (!sections.IsEmpty()) {
				sections.Last().sections.Add(current);
				current = sections.Last();
				sections.RemoveLast();
			}
		}

		bool MergeSections(List<Dragonfruit>& sections) {
			for (Int i = sections.Size() - 1; i >= 0; i--) {
				if (sections[i].type == current.type && sections[i].title == current.title) {
					for (const Dragonfruit& section : current.sections) {
						sections[i].sections.Add(section);
					}

					return true;
				}
			}

			return false;
		}

		void EndComplete() {
			if (!sections.IsEmpty()) {
				if (current.type == DragonfruitType::Heading) {
					if (MergeSections(sections.Last().sections)) return;
				}

				sections.Last().sections.Add(current);

				if (sections.Last().type == DragonfruitType::Heading && sections.Last().block.Size() > 0) {
					sections.Last().block = "";
					current = sections.Last();
					sections.RemoveLast();
					EndComplete();
				}
			}
			else {
				completed.Add(current);
			}
		}

		void EndValue() {
			if (current.type == DragonfruitType::Value) {
				End();
			}
		}

		void EndCategory() {
			EndValue();

			if (current.type == DragonfruitType::Category) {
				currentCategory = "";
				End();
			}
		}

		void EndSummary(const bool ignoreBlock = false) {
			EndCategory();

			if (current.type == DragonfruitType::Summary && !current.code.IsEmpty()) {
				if (!ignoreBlock && current.block.Size() != 0) {
					sections.Add(current);
					indents.Add(currentIndent);
					currentIndent = 0;
				}
				else {
					EndComplete();
				}
			}

			current = Dragonfruit();
			ignoreCode = false;
			multi = false;
		}

		void EndSummary(const DragonfruitInfo& info) {
			EndCategory();

			if (current.type == DragonfruitType::Summary && !current.code.IsEmpty()) {
				String code = current.code[0];

				for (UInt i = 1; i < current.code.Size(); i++) {
					code += "\n" + current.code[i];
				}

				if (current.block.Size() == 0) {
					for (const Regex& pattern : info.blockPatterns) {
						if (Optional<Match> match = pattern.Match(code)) {
							if (!match->groups.IsEmpty()) {
								current.block = match->groups[0];
								break;
							}
						}
					}
				}

				if (current.title.Size() == 0) {
					for (const Regex& pattern : info.titlePatterns) {
						if (Optional<Match> match = pattern.Match(code)) {
							if (!match->groups.IsEmpty()) {
								current.title = match->groups[0];
								break;
							}
						}
					}
				}

				for (const Regex& pattern : info.codePatterns) {
					if (Optional<Match> match = pattern.Match(code)) {
						if (!match->groups.IsEmpty()) {
							current.code.Clear();

							String code = "";

							for (const String& str : match->groups) {
								code += str;
							}

							current.code.Add(code);
							break;
						}
					}
				}

				if (!sections.IsEmpty() && sections.Last().type == DragonfruitType::Summary && sections.Last().block.Size() > 0) {
					for (const Pair<String, Regex>& pattern : info.blockHeadingPatterns) {
						if (Optional<Match> match = pattern.value.Match(code)) {
							Dragonfruit heading;
							heading.type = DragonfruitType::Heading;
							heading.title = pattern.key;
							heading.block = pattern.key;

							sections.Add(heading);
							break;
						}
					}
				}
			}

			EndSummary();
		}

		void EndHeading() {
			if (!sections.IsEmpty()) {
				if (sections.Last().type == DragonfruitType::Heading) {
					current = sections.Last();
					sections.RemoveLast();
					EndComplete();
					MergeLast();
				}
			}

			current = Dragonfruit();
		}

		void EndBlock() {
			EndHeading();

			if (!sections.IsEmpty()) {
				if (sections.Last().type == DragonfruitType::Summary) {
					current = sections.Last();
					sections.RemoveLast();
					indents.RemoveLast();
					EndSummary(true);
				}
			}

			current = Dragonfruit();
		}

		void EndNamespace() {
			EndBlock();

			if (!sections.IsEmpty()) {
				if (sections.Last().type == DragonfruitType::Namespace) {
					current = sections.Last();
					sections.RemoveLast();
					EndComplete();
					MergeLast();
				}
			}

			current = Dragonfruit();
		}

		void MergeHeading(const UInt index) {
			Dragonfruit current = completed[index];

			for (UInt i = 0; i < completed.Size(); i++) {
				if (i == index) continue;

				if (completed[i].type == DragonfruitType::Heading && completed[i].title == current.title) {
					for (const Dragonfruit& section : current.sections) {
						completed[i].sections.Add(section);
					}

					completed.RemoveAt(index);
					return;
				}
			}
		}

		void MergeNamespace(List<Dragonfruit>& sections) {
			Dragonfruit current = sections[sections.Size() - 1];

			for (UInt i = 0; i < sections.Size() - 1; i++) {
				if (sections[i].type == DragonfruitType::Namespace && sections[i].title == current.title) {
					sections.RemoveLast();

					for (UInt u = 0; u < current.sections.Size(); u++) {
						sections[i].sections.Add(current.sections[u]);

						if (current.sections[i].type == DragonfruitType::Namespace) {
							MergeNamespace(sections[i].sections);
						}
					}

					return;
				}
			}
		}

		void MergeLast() {
			if (completed.Size() <= 1) return;

			switch (completed.Last().type) {
				case DragonfruitType::Heading: {
					MergeHeading(completed.Size() - 1);
					break;
				}

				case DragonfruitType::Namespace: {
					MergeNamespace(completed);
					break;
				}
			}
		}
	};

	inline Dragonfruit::Dragonfruit() {

	}

	inline Dragonfruit::~Dragonfruit() {

	}

	inline void Dragonfruit::Merge(const Dragonfruit& dragonfruit) {
		for (const Dragonfruit& d : dragonfruit.sections) {
			switch (d.type) {
				case DragonfruitType::Namespace:
				case DragonfruitType::Heading: {
					bool found = false;

					for (Dragonfruit& d2 : sections) {
						if (d2.type == d.type && d2.title == d.title) {
							d2.Merge(d);
							found = true;
							break;
						}
					}

					if (!found) sections.Add(d);
					break;
				}

				default: {
					sections.Add(d);
					break;
				}
			}
		}
	}

	inline void Dragonfruit::GenerateHTML(const String& directory) const {
		GenerateHTML(directory, DragonfruitBuilder());
	}

	inline void Dragonfruit::GenerateHTML(const String& directory, const DragonfruitBuilder& builder) const {
		String dir = directory;

		if (dir[dir.Size() + 1] != '/' && dir[dir.Size() + 1] != '\\') {
			dir += '/';
		}

		System::DeleteDirectory(dir);
		System::CreateDirectory(dir);

		GenerateHTMLFile(builder, dir, "./");
		builder.GenerateRootFiles(dir);
	}

	inline void Dragonfruit::GenerateHTMLFile(const DragonfruitBuilder& builder, const String& dir, const String& rootDir) const {
		String newDir, newRoot;
		String filename;
		
		switch (type) {
			case DragonfruitType::Main: {
				newDir   = dir;
				newRoot  = rootDir;
				filename = newDir + "index." + builder.FileExtension();
				break;
			}

			case DragonfruitType::Namespace: {
				newDir   = dir + title.Lower() + "/";
				newRoot  = rootDir == "./" ? String("../") : rootDir + "../";
				filename = newDir + "index." + builder.FileExtension();

				System::CreateDirectory(newDir);
				break;
			}

			default: {
				newDir   = dir;
				newRoot  = rootDir;
				filename = newDir + block.Lower() + "." + builder.FileExtension();
				break;
			}
		}

		FileWriter index = FileWriter(filename);
		index.Write(builder.DocumentStart(title, newRoot));
		GenerateBody(index, builder, newDir, newRoot);
		index.Write(builder.DocumentEnd(newRoot));
		index.Close();
	}

	inline void Dragonfruit::GenerateBody(FileWriter& file, const DragonfruitBuilder& builder, const String& dir, const String& rootDir) const {
		file.Write(builder.DocumentSummary(*this));

		bool hasItems = false;

		List<Dragonfruit> displayedSections;

		file.Write(builder.HeadingListStart());

		for (const Dragonfruit& section : sections) {
			if (section.type != DragonfruitType::Heading && section.type != DragonfruitType::Namespace && section.type != DragonfruitType::Category) {
				if (!hasItems) {
					file.Write(builder.HeadingStart(""));
					hasItems = true;
				}

				file.Write(builder.Item(section, displayedSections.Size()));
				displayedSections.Add(section);
			}
		}

		if (hasItems) {
			file.Write(builder.HeadingEnd(""));
		}

		for (const Dragonfruit& section : sections) {
			if (section.type == DragonfruitType::Heading) {
				file.Write(builder.HeadingStart(section.title));

				for (const Dragonfruit& s : section.sections) {
					file.Write(builder.Item(s, displayedSections.Size()));
					displayedSections.Add(s);
				}

				file.Write(builder.HeadingEnd(section.title));
			}
		}

		bool hasNamespaces = false;

		for (const Dragonfruit& section : sections) {
			if (section.type == DragonfruitType::Namespace) {
				if (!hasNamespaces) {
					file.Write(builder.HeadingStart("Namespaces"));
					hasNamespaces = true;
				}

				file.Write(builder.Item(section, displayedSections.Size()));
				displayedSections.Add(section);
			}
		}

		if (hasNamespaces) {
			file.Write(builder.HeadingEnd("Namespaces"));
		}

		file.Write(builder.HeadingListEnd());

		for (UInt i = 0; i < displayedSections.Size(); i++) {
			if (displayedSections[i].type != DragonfruitType::Namespace && displayedSections[i].block.Size() == 0) {
				file.Write(builder.Summary(displayedSections[i], i));
			}
			
			if (displayedSections[i].type == DragonfruitType::Namespace || displayedSections[i].block.Size() > 0) {
				displayedSections[i].GenerateHTMLFile(builder, dir, rootDir);
			}
		}
	}

	inline Dragonfruit Dragonfruit::Generate(const String& directory, const DragonfruitInfo& info) {
		return Generate(directory, info, "");
	}

	inline Dragonfruit Dragonfruit::Generate(const String& directory, const DragonfruitInfo& info, const String& dir) {
		String currentDir = directory[directory.Size() - 1] == '/' || directory[directory.Size() - 1] == '\\' ? directory : directory + '/';

		Dragonfruit doc;
		doc.type = DragonfruitType::Main;

		for (const String& file : System::GetFilesInDirectory(currentDir)) {
			for (const String& ext : info.extensions) {
				if (file.Size() > ext.Size() && file.Find('.' + ext, file.Size() - ext.Size() - 1)) {
					doc.Merge(GenerateFromFile(currentDir + file, info, dir + file));
					break;
				}
			}
		}

		for (const String& d : System::GetDirectoriesInDirectory(currentDir)) {
			doc.Merge(Generate(currentDir + d + "/", info, dir + d + "/"));
		}

		return doc;
	}

	inline Dragonfruit Dragonfruit::GenerateFromFile(const String& filename, const DragonfruitInfo& info) {
		return GenerateFromFile(filename, info, filename);
	}

	inline Dragonfruit Dragonfruit::GenerateFromFile(const String& filename, const DragonfruitInfo& info, const String& file) {
		FileReader fileReader;

		try {
			fileReader = FileReader(filename);
		}
		catch (FileNotFoundError e) {
			throw e;
		}

		DragonfruitInfo docInfo = info;
		ParseInfo parseInfo;
		parseInfo.file = file;
		parseInfo.directory = Regex::Match("^(.*%/)~%/+$|()", filename)->groups[0];
		parseInfo.commentPattern = Regex("^(%s*)" + Regex::Escape(info.docComment) + "{([%up]|i+%l?)|%[%s*(%a+{%s+%a+}*)%s*%]}?{%s+(./)%n*|()%n*}$");

		for (const String& line : fileReader) {
			ParseLine(line, parseInfo, docInfo);
		}

		fileReader.Close();

		while (!parseInfo.sections.IsEmpty()) {
			parseInfo.EndNamespace();
		}

		Dragonfruit doc;
		doc.type = DragonfruitType::Main;
		doc.sections = parseInfo.completed;
		return doc;
	}

	inline List<Dragonfruit::DocComment> Dragonfruit::Import(const String& filename, const DragonfruitInfo& info) {
		static Regex pattern = Regex("^{([%up]|i+%l?)|%[%s*(%a+{%s+%a+}*)%s*%]}?{%s+(./)%n*|()%n*}$");

		FileReader file;

		try {
			file = FileReader(filename);
		}
		catch (FileError& e) {
			return List<DocComment>();
		}

		List<DocComment> comments;

		for (const String& line : file) {
			Optional<Match> match = pattern.Match(line);

			if (match && match->length > 0) {
				DocComment comment;

				if (match->groups.Size() == 2) {
					comment.commentType = match->groups[0];
					comment.content     = match->groups[1].Trim();
				}
				else {
					comment.content = match->groups[0].Trim();
				}

				comments.Add(comment);
			}
		}

		return comments;
	}

	inline void Dragonfruit::ParseLine(const String& line, ParseInfo& info, DragonfruitInfo& docInfo) {
		Optional<DocComment> comment = ParseDocComment(line, info, docInfo);

		if (comment && !info.multi) {
			ParseComment(*comment, info, docInfo);
		}
		else if (info.current.type != DragonfruitType::None) {
			if (info.multi) {
				if (comment && (comment->commentType == "M" || comment->commentType == "Multi" || comment->commentType == "Multiline")) {
					info.EndSummary(docInfo);
				}
				else if (!info.ignoreCode) {
					info.EndCategory();
					info.current.code.Add(line.Trim());
				}
			}
			else if (info.ignoreCode) {
				info.EndSummary(docInfo);
			}
			else {
				info.EndCategory();
				info.current.code.Add(line.Trim());
				info.EndSummary(docInfo);
			}
		}
	}

	inline Optional<Dragonfruit::DocComment> Dragonfruit::ParseDocComment(const String& line, const ParseInfo& info, const DragonfruitInfo& docInfo) {
		Optional<Match> match = info.commentPattern.Match(line);

		if (match) {
			DocComment comment;

			for	(const char c : match->groups[0]) {
				if (c == '\t') {
					comment.indent += docInfo.tabSize - comment.indent % docInfo.tabSize; 
				}
				else {
					comment.indent++;
				}
			}

			if (match->groups.Size() == 3) {
				comment.commentType = match->groups[1];
				comment.content     = match->groups[2].Trim();
			}
			else {
				comment.content = match->groups[1].Trim();
			}

			return comment;
		}

		return nullptr;
	}

	inline void Dragonfruit::ParseComment(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		static Regex itemPattern = Regex("^{({(item)%b%s*}+)|((i)+){$|~i}}");

		if (docInfo.blockMode == DragonfruitBlockMode::Indent) {
			while (!info.indents.IsEmpty() && comment.indent <= info.indents.Last()) {
				info.EndBlock();
			}
		}

		if (comment.commentType.Size() > 0) {
			if (ParseTitle(comment, info, docInfo)) return;
			if (ParseBlock(comment, info, docInfo)) return;
			if (ParseNamespace(comment, info, docInfo)) return;
			if (ParseHeading(comment, info, docInfo)) return;
			if (ParseCategory(comment, info, docInfo)) return;
			if (ParseValue(comment, info, docInfo)) return;
			if (ParseImport(comment, info, docInfo)) return;
			if (ParseSettings(comment, info, docInfo)) return;
			if (ParseCode(comment, info, docInfo)) return;
			if (ParseMulti(comment, info, docInfo)) return;

			UInt items = 0;
			DocComment c = comment;

			if (Optional<Match> match = itemPattern.Match(c.commentType)) {
				items = match->groups.Size() - 1;
				c.commentType = c.commentType.Sub(match->groups[0].Size());

				if (c.commentType.Size() == 0) {
					if (ParseItem(c, info, docInfo, items)) return;
				}
			}

			if (ParsePara(c, info, docInfo, items)) return;
		}
		else {
			if (ParseDescription(comment, info, docInfo)) return;
		}
	}

	inline bool Dragonfruit::ParseTitle(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		if (comment.commentType == "T" || comment.commentType == "Title") {
			if (info.current.type == DragonfruitType::None || info.current.type == DragonfruitType::Summary) {
				info.current.type  = DragonfruitType::Summary;
				info.current.title = comment.content;
				info.current.file  = info.file;
				info.currentIndent = comment.indent;
			}

			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseBlock(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		if (comment.commentType == "B" || comment.commentType == "Block") {
			info.EndCategory();

			if (info.current.type != DragonfruitType::None) {
				info.current.block = comment.content.Size() == 0 ? String("_") : comment.content.Split(":")[0].Trim();
			}
			else {
				info.EndBlock();
			}

			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseNamespace(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		if (comment.commentType == "N" || comment.commentType == "Namespace") {
			while (!info.sections.IsEmpty()) {
				info.EndNamespace();
			}

			Array<String> names = comment.content.Split(docInfo.namespaceSeparator);

			for (const String& name : names) {
				info.current.type = DragonfruitType::Namespace;
				info.current.title = name.Trim();

				if (info.current.title.Size() > 0) {
					info.sections.Add(info.current);
				}

				info.current = Dragonfruit();
			}

			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseHeading(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		if (comment.commentType == "H" || comment.commentType == "Heading") {
			info.EndSummary();
			info.EndHeading();
			info.current.type = DragonfruitType::Heading;
			info.current.title = comment.content;

			if (info.current.title.Size() > 0) {
				info.sections.Add(info.current);
			}

			info.current = Dragonfruit();
			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseCategory(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		if (comment.commentType == "C" || comment.commentType == "Category") {
			info.EndCategory();

			if (info.current.type == DragonfruitType::None) {
				info.current.type  = DragonfruitType::Summary;
				info.current.file  = info.file;
				info.currentIndent = comment.indent;
			}
			
			if (info.current.type == DragonfruitType::Summary) {
				info.sections.Add(info.current);
				info.current = Dragonfruit();
				info.current.type = DragonfruitType::Category;
				info.current.title = comment.content;
				info.currentCategory = comment.content;
			}

			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseValue(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		static Regex split = Regex("^(./~\\|)%:(.*)$");
		bool isValue = false;
		bool prioTitle = true;

		if (comment.commentType == "A" || comment.commentType == "Arg" || comment.commentType == "Argument" || comment.commentType == "Param" || comment.commentType == "Parameter") {
			isValue = true;

			if (info.currentCategory != "Arguments") {
				DocComment c;
				c.commentType = "C";
				c.content     = "Arguments";
				ParseCategory(c, info, docInfo);
			}
		}
		else if (comment.commentType == "R" || comment.commentType == "Return" || comment.commentType == "Returns") {
			isValue = true;
			prioTitle = false;

			if (info.currentCategory != "Returns") {
				DocComment c;
				c.commentType = "C";
				c.content     = "Returns";
				ParseCategory(c, info, docInfo);
			}
		}
		else if (comment.commentType == "E" || comment.commentType == "Error" || comment.commentType == "Exception") {
			isValue = true;

			if (info.currentCategory != "Errors") {
				DocComment c;
				c.commentType = "C";
				c.content     = "Errors";
				ParseCategory(c, info, docInfo);
			}
		}
		else if (comment.commentType == "W" || comment.commentType == "Warning") {
			isValue = true;
			prioTitle = false;

			if (info.currentCategory != "Warnings") {
				DocComment c;
				c.commentType = "C";
				c.content     = "Warnings";
				ParseCategory(c, info, docInfo);
			}
		}
		else if (comment.commentType == "V" || comment.commentType == "Var" || comment.commentType == "Value") {
			isValue = true;
		}

		if (isValue) {
			info.EndValue();

			if (info.current.type == DragonfruitType::Category) {
				info.sections.Add(info.current);
				info.current = Dragonfruit();
				info.current.type = DragonfruitType::Value;

				if (Optional<Match> match = split.Match(comment.content)) {
					info.current.title = match->groups[0].Trim().Replace("\\:", ":");
					info.current.description.segments.Add(DragonfruitDesc(DragonfruitDescType::Paragraph, match->groups[1].Trim()));
				}
				else if (prioTitle) {
					info.current.title = comment.content.Trim();
				}
				else {
					info.current.title = "";
					info.current.description.segments.Add(DragonfruitDesc(DragonfruitDescType::Paragraph, comment.content.Trim()));
				}
			}
			
			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseMulti(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		if (comment.commentType == "M" || comment.commentType == "Multi" || comment.commentType == "Multiline") {
			info.multi = true;
			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseCode(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		if (comment.commentType == "Code") {
			info.EndCategory();
			info.ignoreCode = true;
			info.current.code.Add(comment.content.Trim());
			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseImport(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		if (comment.commentType == "I" || comment.commentType == "Import" || comment.commentType == "Include") {
			Optional<Match> match = Regex::Match("^%s*(%w+){%s*%:%s*(%w+)}?%n*$", comment.content);

			if (match) {
				String label = match->groups.Size() > 1 ? match->groups[1] : String("");
				List<DocComment> comments;

				if (!info.imported.Contains(match->groups[0], comments)) {
					comments = Import(info.directory + match->groups[0] + ".dragonfruit", docInfo);
					info.imported.Add(match->groups[0], comments);
				}

				bool foundLabel = label.Size() == 0;

				for (const DocComment& comment : comments) {
					if (foundLabel) {
						if (comment.commentType == "L" || comment.commentType == "Label") break;
						ParseComment(comment, info, docInfo);
					}
					else if (comment.commentType == "L" || comment.commentType == "Label") {
						if (comment.content.Trim() == label) {
							foundLabel = true;
						}
					}
				}
			}

			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseSettings(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		const Regex settingsPattern = Regex("^%s*(%w+)%s*%:%s*(%w+){%s*%,%s*(%w+)%s*%:%s*(%w+)}*%n*$");

		if (comment.commentType == "S" || comment.commentType == "Settings") {
			if (Optional<Match> match = settingsPattern.Match(comment.content)) {
				for (UInt i = 0; i < match->groups.Size(); i += 2) {
					if (match->groups[i] == "block") {
						if (match->groups[i + 1] == "manual") {
							docInfo.blockMode = DragonfruitBlockMode::Manual;
						}
						else if (match->groups[i + 1] == "indent") {
							docInfo.blockMode = DragonfruitBlockMode::Indent;
						}
					}
					else if (match->groups[i] == "tab") {
						if (Regex::Match("^%d+$", match->groups[i + 1])) {
							docInfo.tabSize = match->groups[i + 1].ToUInt();
						}
					}
				}
			}

			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParsePara(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo, const UInt items) {
		if (comment.commentType == "p" || comment.commentType == "para" || comment.commentType == "paragraph") {
			if (info.current.type == DragonfruitType::None) {
				info.current.type  = DragonfruitType::Summary;
				info.current.file  = info.file;
				info.currentIndent = comment.indent;
			}

			info.current.description.AddInItem(items, DragonfruitDesc(DragonfruitDescType::Paragraph, comment.content));
			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseItem(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo, const UInt items) {
		if (items > 0) {
			if (info.current.type == DragonfruitType::None) {
				info.current.type  = DragonfruitType::Summary;
				info.current.file  = info.file;
				info.currentIndent = comment.indent;
			}

			info.current.description.AddItem(items, comment.content);
			return true;
		}

		return false;
	}

	inline bool Dragonfruit::ParseDescription(const DocComment& comment, ParseInfo& info, DragonfruitInfo& docInfo) {
		if (info.current.type == DragonfruitType::None) {
			info.current.type  = DragonfruitType::Summary;
			info.current.file  = info.file;
			info.currentIndent = comment.indent;
		}

		info.current.description.AddText(comment.content);
		return true;
	}
}
