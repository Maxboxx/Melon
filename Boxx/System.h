#pragma once

#include "Types.h"
#include "String.h"
#include "Array.h"
#include "List.h"
#include "Error.h"

#include <filesystem>

///[Settings] block: indent

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Static Classes

	///[Title] System
	/// Static class for communicating with the operating system.
	///[Block] System
	class System final {
	public:
		System() = delete;

		/// Executes a system command.
		static void Execute(const String& command);

		/// Checks if a file exists.
		static bool FileExists(const String& file);

		/// Checks if a directory exists.
		static bool DirectoryExists(const String& directory);

		/// Creates a directory.
		///[Returns] bool success: {true} if the directory was created. {false} otherwise.
		static bool CreateDirectory(const String& directory);

		/// Deletes a directory.
		///[Returns] bool success: {true} if the directory was deleted. {false} otherwise.
		static bool DeleteDirectory(const String& directory);

		/// Deletes a file.
		///[Returns] bool success: {true} if the file was deleted. {false} otherwise.
		static bool DeleteFile(const String& directory);

		/// Gets a list of all files in the specified directory.
		static List<String> GetFilesInDirectory(const String& directory);

		/// Gets a list of all directories in the specified directory.
		static List<String> GetDirectoriesInDirectory(const String& directory);
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

	inline List<String> System::GetDirectoriesInDirectory(const String& directory) {
		List<String> dirs;

		for (const std::filesystem::directory_entry& entry : std::filesystem::directory_iterator((const char*)directory)) {
			if (entry.is_directory()) {
				dirs.Add(entry.path().filename().string());
			}
		}

		return dirs;
	}
}