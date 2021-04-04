#pragma once

#include "Boxx/String.h"
#include "Boxx/Set.h"

#include "Symbols/NameList.h"

///N Melon
namespace Melon {
	/// Contains info about a location in a file.
	struct FileInfo {
		/// The name of the file.
		Boxx::String filename;

		/// The line number of the file.
		Boxx::UInt line;

		/// The current namespace.
		Symbols::NameList currentNamespace;

		/// The include name for the file.
		Symbols::Name includeName;

		/// All included namespaces.
		Boxx::Set<Symbols::NameList> includedNamespaces;

		/// The statement number of this file location.
		Boxx::UInt statement;

		/// Empty constructor.
		FileInfo() {
			filename = "";
			line = 0;
			statement = 0;
		}

		/// Constructor.
		FileInfo(const Boxx::String& filename, const Boxx::UInt line, const Boxx::UInt statementNumber) {
			this->filename = filename;
			this->line = line;
			this->statement = statementNumber;
		}

		/// Constructor with namespace information.
		FileInfo(const Boxx::String& filename, const Boxx::UInt line, const Boxx::UInt statementNumber, const Symbols::NameList& currentNamespace, const Boxx::Set<Symbols::NameList>& includedNamespaces) {
			this->filename = filename;
			this->line = line;
			this->statement = statementNumber;
			this->currentNamespace = currentNamespace;
			this->includedNamespaces = includedNamespaces;
		}
	};
}