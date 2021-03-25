#pragma once

#include <fstream>
#include <iostream>
#include <sstream>

#include "Types.h"
#include "Error.h"
#include "String.h"
#include "Buffer.h"
#include "Pointer.h"

///[Settings] block: indent 

///[Namespace] Boxx
namespace Boxx {
	///[Heading] Files

	///[Title] FileMode
	/// Bit flags for the different modes of opening a file.
	///[Block] FileMode
	enum class FileMode : UByte {
		/// No mode.
		None = 0,

		/// Opens a file in binary mode.
		Binary = 1
	};

	BOXX_ENUM_FLAGS(FileMode);

	///[Title] FileReader
	/// Used to read the contents of a file.
	///[Block] FileReader
	class FileReader {
	public:
		FileReader();

		/// Opens a file for reading.
		///[Error] FileNotFoundError: Thrown if the file was not found.
		explicit FileReader(const char* const filename, const FileMode mode = FileMode::None);

		FileReader(const FileReader& file);
		FileReader(FileReader&& file) noexcept;
		~FileReader();

		///[Heading] Methods

		/// Reads a line from the file.
		///[Error] FileClosedError: Thrown if the file is closed.
		///[Error] EndOfFileError: Thrown if the end of file has been reached.
		String ReadLine();

		/// Reads the remaining contents of the file.
		///[Error] FileClosedError: Thrown if the file is closed.
		///[Error] EndOfFileError: Thrown if the end of file has been reached.
		String ReadAll();

		/// Reads the remaining contents of the file to a buffer.
		///[Error] FileClosedError: Thrown if the file is closed.
		///[Error] EndOfFileError: Thrown if the end of file has been reached.
		Buffer ReadToBuffer();

		/// Close the file.
		void Close();

		/// Checks if the file is open.
		bool IsOpen();

		///[Heading ]Iterators

		class LineIterator {
		public:
			LineIterator();
			LineIterator(FileReader* const file);
			~LineIterator() {}

			String operator*();
			LineIterator operator++();
			bool operator!=(const LineIterator& iterator);

		private:
			FileReader* file;
			String next;
		};

		/// Iterates over all lines of the file.
		///[Code] for (String line : file)
		LineIterator begin();
		LineIterator end();

		void operator=(const FileReader& file);
		void operator=(FileReader&& file) noexcept;

	private:
		Pointer<std::ifstream> file;
		bool done = false;
	};

	///[Title] FileWriter
	/// Used to write to a file.
	///[Block] FileWriter
	class FileWriter {
	public:
		FileWriter();

		/// Opens a file for writing.
		///[Error] FileOpenError: Thrown if the file can not be opened.
		explicit FileWriter(const char* const filename, const FileMode mode = FileMode::None);

		FileWriter(const FileWriter& file);
		FileWriter(FileWriter&& file) noexcept;
		~FileWriter();

		///[Heading] Methods

		/// Writes a string to the file.
		///[Error] FileClosedError: Thrown if the file is closed.
		void Write(const String& text);

		/// Writes the contents of a buffer to the file.
		///[Error] FileClosedError: Thrown if the file is closed.
		void Write(const Buffer& data);

		/// Close the file.
		///[Error] FileClosedError: Thrown if the file is already closed.
		void Close();

		/// Checks if the file is open.
		bool IsOpen() const;

		void operator=(const FileWriter& file);
		void operator=(FileWriter&& file) noexcept;

	private:
		Pointer<std::ofstream> file;
	};

	///[Title] FileError
	/// Base class for all file errors.
	///[Block] FileError: Error
	class FileError : public Error {
	public:
		FileError():Error(){}
		FileError(const char* const msg):Error(msg){}

		virtual String Name() const override {
			return "FileError";
		}
	};

	///[Title] FileNotFoundError
	/// Used if a file is not found.
	///[Block] FileNotFoundError: FileError
	class FileNotFoundError : public FileError {
	public:
		FileNotFoundError():FileError(){}
		FileNotFoundError(const char* const msg):FileError(msg){}

		virtual String Name() const override {
			return "FileNotFoundError";
		}
	};

	///[Title] FileOpenError
	/// Used if a file can not be opened.
	///[Block] FileOpenError: FileError
	class FileOpenError : public FileError {
		public:
		FileOpenError():FileError(){}
		FileOpenError(const char* const msg):FileError(msg){}

		virtual String Name() const override {
			return "FileOpenError";
		}
	};

	///[Title] FileClosedError
	/// Used if a file is closed.
	///[Block] FileClosedError: FileError
	class FileClosedError : public FileError {
	public:
		FileClosedError():FileError(){}
		FileClosedError(const char* const msg):FileError(msg){}

		virtual String Name() const override {
			return "FileClosedError";
		}
	};


	///[Title] EndOfFileError
	/// Used if the end of file has been reached.
	///[Block] EndOfFileError: FileError
	class EndOfFileError : public FileError {
	public:
		EndOfFileError():FileError(){}
		EndOfFileError(const char* const msg):FileError(msg){}

		virtual String Name() const override {
			return "EndOfFileError";
		}
	};

	inline FileReader::FileReader() {

	}

	inline FileReader::FileReader(const char* const filename, const FileMode mode) {
		if ((mode & FileMode::Binary) != FileMode::None)
			file = new std::ifstream(filename, std::fstream::binary);
		else
			file = new std::ifstream(filename);

		if (!file->is_open()) {
			throw FileNotFoundError("Could not find file: " + String(filename));
		}
	}

	inline FileReader::FileReader(const FileReader& file) {
		done = file.done;
		this->file = file.file;
	}

	inline FileReader::FileReader(FileReader&& file) noexcept {
		done = file.done;
		this->file = std::move(file.file);
	}

	inline FileReader::~FileReader() {
		
	}

	inline String FileReader::ReadLine() {
		if (!IsOpen())
			throw FileClosedError("File is closed");
		else if (done)
			throw EndOfFileError("End of file reached");

		std::string str;

		if (!std::getline(*file, str))
			done = true;

		return str;
	}

	inline String FileReader::ReadAll() {
		if (!IsOpen())
			throw FileClosedError("File is closed");
		else if (done)
			throw EndOfFileError("End of file reached");

		std::stringstream ss;
		ss << file->rdbuf();
		done = true;
		return String(ss.str().c_str(), (unsigned int)ss.str().size());
	}

	inline Buffer FileReader::ReadToBuffer() {
		if (!IsOpen())
			throw FileClosedError("File is closed");
		else if (done)
			throw EndOfFileError("End of file reached");

		std::stringstream ss;
		ss << file->rdbuf();
		done = true;

		String data = String(ss.str().c_str(), (unsigned int)ss.str().size());
		Buffer buf = Buffer(data.Size());
		buf.Write(data);
		buf.SetPos(0);
		return buf;
	}

	inline void FileReader::Close() {
		if (IsOpen()) file->close();
	
	}

	inline bool FileReader::IsOpen() {
		return file != nullptr && file->is_open();
	}

	inline FileReader::LineIterator FileReader::begin() {
		return LineIterator(this);
	}

	inline FileReader::LineIterator FileReader::end() {
		return LineIterator();
	}

	inline void FileReader::operator=(const FileReader& file) {
		done = file.done;
		this->file = file.file;
	}

	inline void FileReader::operator=(FileReader&& file) noexcept {
		done = file.done;
		this->file = std::move(file.file);
	}

	inline FileReader::LineIterator::LineIterator() {
		file = nullptr;
	}

	inline FileReader::LineIterator::LineIterator(FileReader* const file) : file(file) {
		next = file->ReadLine();
	}

	inline FileReader::LineIterator FileReader::LineIterator::operator++() {
		return *this;
	}

	inline String FileReader::LineIterator::operator*() {
		try {
			const String line = next;
			next = file->ReadLine();
			return line;
		}
		catch (EndOfFileError) {
			file->done = true;
			return "";
		}
	}

	inline bool FileReader::LineIterator::operator!=(const LineIterator& iterator) {
		return !file->done;
	}

	inline FileWriter::FileWriter() {

	}

	inline FileWriter::FileWriter(const char* const filename, const FileMode mode) {
		if ((mode & FileMode::Binary) != FileMode::None)
			file = new std::ofstream(filename, std::fstream::binary);
		else
			file = new std::ofstream(filename);

		if (!file->is_open()) {
			throw FileOpenError("Could not open file: " + String(filename));
		}
	}

	inline FileWriter::FileWriter(const FileWriter& file) {
		this->file = file.file;
	}

	inline FileWriter::FileWriter(FileWriter&& file) noexcept {
		this->file = std::move(file.file);
	}

	inline FileWriter::~FileWriter() {
		
	}

	inline void FileWriter::Write(const String& text) {
		if (!IsOpen())
			throw FileClosedError("File is closed");

		file->write(text, text.Size());
	}

	inline void FileWriter::Write(const Buffer& data) {
		if (!IsOpen())
			throw FileClosedError("File is closed");

		String text = data.ToString();
		file->write(text, text.Size());
	}

	inline void FileWriter::Close() {
		if (IsOpen()) file->close();
	}

	inline bool FileWriter::IsOpen() const {
		return file != nullptr && file->is_open();
	}

	inline void FileWriter::operator=(const FileWriter& file) {
		this->file = file.file;
	}

	inline void FileWriter::operator=(FileWriter&& file) noexcept {
		this->file = std::move(file.file);
	}
}
