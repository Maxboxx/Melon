#pragma once

#include "Types.h"
#include "String.h"
#include "Set.h"
#include "File.h"
#include "List.h"
#include "Regex.h"
#include "Optional.h"

///N Documentation

namespace Boxx {
	struct DocInfo;
	struct DocSection;

	///B Documentation
	/// Class for generating and storing documentation for a code file
	class Documentation {
	public:
		///H Members
		
		///T Name
		/// The name of the document
		String name;

		///T Sections
		/// A list of all documentation sections
		List<DocSection> sections;

		Documentation();
		~Documentation();

		///H Static functions

		///T Generate documentation
		/// Generates a <code>Documentation</code> object from a code file
		///A const String& filename: The name of the file to generate documentation for
		///A const DocInfo& info: Information about the language used in the file
		///E FileNotFoundError: Thrown if the code file is not found
		static Documentation Generate(const String& filename, const DocInfo& info);

	private:
		struct DocComment {
			char commentType = ' ';
			String content;
			List<DocComment> subcomments;

			~DocComment() {}
		};

		static List<DocSection> ParseComments(const List<DocComment>& comments, UInt& index, const bool isBlock = false);
		static DocSection ParseBlock(const List<DocComment>& comments, UInt& index);
		static DocSection ParseTitle(const List<DocComment>& comments, UInt& index);
		static void ParseSectionInfo(DocSection& section, const List<DocComment>& comments, UInt& index);
		static DocSection ParseHeading(const List<DocComment>& comments, UInt& index);

		static bool HasCategory(DocSection& section, const String& name, DocSection& category);
	};

	///B DocSectionType
	/// The different types of documentation sections
	enum class DocSectionType : UByte {
		///T Values
		///M
		None,
		Title,
		Heading,
		Block,
		Category,
		Value
		///M
	};

	///B DocInfo
	/// Contains information about the language
	struct DocInfo {
		///T Documentation comment
		/// The start of a documentation comment
		String docComment = "///";

		~DocInfo() {}
	};

	///B DocSection
	/// A documentation section
	struct DocSection {
		///T Section type
		/// The type of documentation section
		DocSectionType sectionType = DocSectionType::None;

		///T Tile
		/// The title of the documentation section
		String title;

		///T Code
		/// Lines of code for the section
		List<String> code;

		///T Description
		/// A list of descriptions for the section
		List<String> description;

		///T Subsections
		/// A list of all subsections to the documentation section
		List<DocSection> sections;

		~DocSection() {}
	};
	
	inline Documentation::Documentation() {
	
	}

	inline Documentation::~Documentation() {

	}

	inline Documentation Documentation::Generate(const String& filename, const DocInfo& info) {
		FileReader file;
		
		try {
			file = FileReader(filename);
		}
		catch (FileNotFoundError e) {
			throw e;
		}

		Regex comment = Regex(Regex::Escape(info.docComment) + "(" + Regex::Escape(info.docComment[info.docComment.Size() - 1]) + "*)(%u)?{%s+(./)%n*|()%n*}$");

		List<DocComment> comments;
		bool multi = false;
		bool findCode = false;
		bool hasS = false;
		UByte commentLevel = 0;
		UByte currentLevel = 0;

		Documentation doc;
		doc.name = filename;

		for (const String& line : file) {
			Array<String> matches = comment.Match(line);
			DocComment c;
			bool add = true;

			if (matches.Size() == 3) {
				UByte level = matches[0].Size();

				if (commentLevel < level) {
					currentLevel++;
				}
				else if (commentLevel > level) {
					currentLevel--;
				}

				commentLevel = level;
				c.commentType = matches[1][0];
				c.content = matches[2];
			}
			else if (matches.Size() == 2) {
				UByte level = matches[0].Size();

				if (commentLevel < level) {
					currentLevel++;
				}
				else if (commentLevel > level) {
					currentLevel--;
				}

				commentLevel = level;
				c.content = matches[1];
			}
			else if (findCode) {
				if (!hasS) {
					c.commentType = 'S';
					c.content = line;
				}
				else {
					add = false;
				}

				multi = false;
				findCode = false;
				hasS = false;
			}
			else if (multi) {
				c.commentType = 'S';
				c.content = line;
			}
			else {
				add = false;
			}

			if (!matches.IsEmpty()) {
				if (c.commentType == 'T' || c.commentType == 'B') {
					findCode = true;

					multi = false;
					hasS = false;
				}
				else if (c.commentType == 'S') {
					hasS = true;
				}
				else if (c.commentType == 'M') {
					add = false;

					if (!multi) {
						multi = true;
						findCode = false;
					}
					else {
						multi = false;
						findCode = false;
						hasS = false;
					}
				}
				else if (c.commentType == 'N') {
					doc.name = c.content;
					add = false;
				}
			}

			if (add) {
				List<DocComment> comment = comments;

				for (UInt i = 0; i < currentLevel; i++) {
					comment = comment.Last().subcomments;
				}

				comment.Add(c);
			}
		}

		UInt index = 0;
		doc.sections = ParseComments(comments, index);

		return doc;
	}

	inline List<DocSection> Documentation::ParseComments(const List<DocComment>& comments, UInt& index, const bool isBlock) {
		List<DocSection> sections;

		while (index < comments.Size()) {
			bool sub = true;

			if (comments[index].commentType == 'B') {
				if (isBlock) break;
				sections.Add(ParseBlock(comments, index));
				sub = false;
			}
			else if (comments[index].commentType == 'H') {
				sections.Add(ParseHeading(comments, index));
			}
			else if (comments[index].commentType == 'T') {
				sections.Add(ParseTitle(comments, index));
			}
			else {
				index++;
			}

			if (sub && !comments[index - 1].subcomments.IsEmpty()) {
				UInt index2 = 0;

				for (const DocSection& s : ParseComments(comments[index - 1].subcomments, index2)) {
					sections.Add(s);
				}
			}
		}

		return sections;
	}

	inline DocSection Documentation::ParseBlock(const List<DocComment>& comments, UInt& index) {
		DocSection block = DocSection();
		block.sectionType = DocSectionType::Block;
		block.title = comments[index].content;

		index++;
		ParseSectionInfo(block, comments, index);

		if (!comments[index - 1].subcomments.IsEmpty()) {
			UInt index2 = 0;
			block.sections = ParseComments(comments[index - 1].subcomments, index2);
		}

		for (const DocSection& section : ParseComments(comments, index, true)) {
			block.sections.Add(section);
		}

		return block;
	}

	inline DocSection Documentation::ParseTitle(const List<DocComment>& comments, UInt& index) {
		DocSection title = DocSection();
		title.sectionType = DocSectionType::Title;
		title.title = comments[index].content;

		index++;
		ParseSectionInfo(title, comments, index);

		return title;
	}

	inline void Documentation::ParseSectionInfo(DocSection& section, const List<DocComment>& comments, UInt& index) {
		DocSection currentSection = section;
		Optional<String> categoryName = nullptr;

		Regex splitValue = Regex("^({~%:|%:%:}+){%:(.*)}?$");

		for (; index < comments.Size(); index++) {
			char commentType = comments[index].commentType;

			if (commentType == ' ') {
				currentSection.description.Add(comments[index].content);
			}
			else if (commentType == 'C') {
				DocSection categorySection;

				if (HasCategory(section, comments[index].content, categorySection)) {
					currentSection = categorySection;
				}
				else {
					currentSection = DocSection();
					currentSection.sectionType = DocSectionType::Category;
					currentSection.title = comments[index].content;

					section.sections.Add(currentSection);
				}

				categoryName = currentSection.title;
			}
			else if (
				commentType == 'A' ||
				commentType == 'R' ||
				commentType == 'E' ||
				commentType == 'W' ||
				commentType == 'V'
			) {
				String catName = categoryName ? categoryName.Get() : "Category";

				switch (commentType) {
					case 'A': catName = "Arguments"; break;
					case 'R': catName = "Returns";   break;
					case 'E': catName = "Errors";    break;
					case 'W': catName = "Warnings";  break;
				}

				DocSection categorySection;

				if (!HasCategory(section, catName, categorySection)) {
					categorySection.sectionType = DocSectionType::Category;
					categorySection.title = catName;
					section.sections.Add(categorySection);
				}

				currentSection = DocSection();
				currentSection.sectionType = DocSectionType::Value;
				currentSection.title = catName;

				if (commentType != 'W') {
					Array<String> m = splitValue.Match(comments[index].content);
					currentSection.code.Add(m[0].Replace("::", ":"));

					if (m.Size() > 1) {
						currentSection.description.Add(m[1]);
					}
				}
				else {
					currentSection.description.Add(comments[index].content);
				}

				categorySection.sections.Add(currentSection);
			}
			else if (commentType == 'S') {
				section.code.Add(comments[index].content.Trim());
			}
			else {
				break;
			}
		}
	}

	inline DocSection Documentation::ParseHeading(const List<DocComment>& comments, UInt& index) {
		DocSection heading = DocSection();
		heading.sectionType = DocSectionType::Heading;
		heading.title = comments[index].content;

		for (index++; index < comments.Size(); index++) {
			if (comments[index].commentType == ' ') {
				heading.description.Add(comments[index].content);
			}
			else {
				break;
			}
		}

		return heading;
	}

	inline bool Documentation::HasCategory(DocSection& section, const String& name, DocSection& category) {
		for (DocSection& s : section.sections) {
			if (s.sectionType == DocSectionType::Category) {
				if (s.title == name) {
					category = s;
					return true;
				}
			}
		}

		return false;
	}
}
