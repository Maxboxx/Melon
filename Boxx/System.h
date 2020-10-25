#pragma once

#include "Types.h"
#include "String.h"
#include "Array.h"
#include "List.h"
#include "Error.h"

#include <filesystem>

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

		///T Delete Directory
		/// Deletes a directory
		///R bool success: <code>true</code> if the directory was deleted. <code>false</code> otherwise.
		static bool DeleteDirectory(const String& directory);

		///T Delete File
		/// Deletes a file
		///R bool success: <code>true</code> if the file was deleted. <code>false</code> otherwise.
		static bool DeleteFile(const String& directory);

		///T Get Files in Directory
		/// Gets a list of all files in the specified directory
		///E SystemNotSupportedError: Thrown if the operating system is not Windows
		static List<String> GetFilesInDirectory(const String& directory);
	};

	inline void System::Execute(const String& command) {
		system(command);
	}

	inline bool System::FileExists(const String& file) {
		return std::filesystem::is_regular_file((const char*)file);
	}

	inline bool System::DirectoryExists(const String& directory) {
		return std::filesystem::is_directory((const char*)directory);
	}

	inline bool System::CreateDirectory(const String& directory) {
		return std::filesystem::create_directory((const char*)directory);
	}

	inline bool System::DeleteDirectory(const String& directory) {
		if (std::filesystem::is_directory((const char*)directory)) {
			return std::filesystem::remove_all((const char*)directory);
		}

		return false;
	}

	inline bool System::DeleteFile(const String& directory) {
		if (!std::filesystem::is_directory((const char*)directory)) {
			return std::filesystem::remove((const char*)directory);
		}

		return false;
	}

	inline List<String> System::GetFilesInDirectory(const String& directory) {
		List<String> files;

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator((const char*)directory)) {
			if (entry.is_regular_file()) {
				files.Add(entry.path().filename().string());
			}
		}

		return files;
	}
}