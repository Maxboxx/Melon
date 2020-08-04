#pragma once

#include "String.h"

#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>

#ifdef BOXX_WINDOWS
	#include <direct.h>
#endif

///N System

namespace Boxx {
	///B System
	/// Static class for communicating with the operating system
	class System {
	public:
		///T Execute
		/// Executes a system command
		static void Execute(const String& command);

		///T File Exists
		/// Checks if a file exists
		static bool FileExists(const String& file);

		///T Directory Exists
		/// Checks if a directory exists
		static bool DirectoryExists(const String& directory);

		///T Create Directory
		/// Creates a directory
		///R bool success: <code>true</code> if the directory was created. <code>false</code> otherwise.
		static bool CreateDirectory(const String& directory);
	};

	inline void System::Execute(const String& command) {
		system(command);
	}

	inline bool System::FileExists(const String& file) {
		struct stat info;
		return stat(file, &info) == 0;
	}

	inline bool System::DirectoryExists(const String& directory) {
		struct stat info;

		if (stat(directory, &info) != 0)
			return false;
		else if (info.st_mode & S_IFDIR)
			return true;
		else
			return false;
	}

	inline bool System::CreateDirectory(const String& directory) {
		#ifdef BOXX_WINDOWS
			return _mkdir(directory) != -1;
		#else
			return mkdir(directory, 0777) != -1;
		#endif
	}
}