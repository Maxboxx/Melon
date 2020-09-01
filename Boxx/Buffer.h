#pragma once

#include "Types.h"
#include "String.h"
#include "List.h"
#include "Array.h"
#include "Error.h"

///N Buffer

namespace Boxx {
	///B Endian
	/// Used to tell the buffer the endian of a value
	enum class Endian : UByte {
		///H Values

		///T Little endian
		Little,

		///T Big endian
		Big,

		///T System endian
		/// The endian used by the system
		System
	};

	///B Buffer
	/// A data structure that contains binary data
	/// The buffer grows if the capacity is reached
	class Buffer {
	public:
		///H Constructors

		///T Empty Buffer
		Buffer();

		///T Buffer with specific capacity
		explicit Buffer(const UInt capacity);

		Buffer(const Buffer& buffer);
		Buffer(Buffer&& buffer) noexcept;
		~Buffer();

		///H Methods

		///T Write
		/// Writes data to the current position of the buffer and advances the position to the next byte after the written data
		/// The data is converted to binary and is added to the buffer
		/// This function can handle writing a <code>String</code> to the buffer
		///A const Endian e: The endian to use for writing to the buffer
		/// Non scalar types might not be encoded properly if this value is not <code>Endian::System</code>
		/// Each component of a non scalar type have to be written to the buffer separately if an explicit endian is required
		///M
		template <class T>
		void Write(const T& data, const Endian endian = Endian::System);
		///M

		///T Read
		/// Reads data from the current position of the buffer and advances the position to the next byte after the read data
		/// A portion of the buffer data is converted from binary and returned
		///A const Endian e: The endian to use for writing to the buffer
		/// Non scalar types might not be encoded properly if this value is not <code>Endian::System</code>
		/// Each component of a non scalar type have to be written to the buffer separately if an explicit endian is required
		///E BufferReadError: Thrown if the buffer does not contain enough bytes to read
		///M
		template <class T>
		T Read(const Endian endian = Endian::System);
		///M

		///T Read string
		/// Reads a string from the current position of the buffer and advances the position to the next byte after the string
		///A const UInt bytes: The number of bytes to read from the buffer
		///E BufferReadError: Thrown if the buffer does not contain enough bytes to read
		String ReadString(const UInt bytes);

		///T To String
		/// Converts the entire binary buffer data to a string
		String ToString() const;

		///T Buffer size
		/// Returns the size of the buffer in bytes
		UInt Size() const;

		///T Buffer capacity
		/// Returns the current capacity of the buffer in bytes
		UInt Capacity() const;

		///T Set position
		/// Sets the current position of the buffer
		///E BufferPosError: Thrown if the position is invalid
		void SetPos(const UInt pos);

		///T Get position
		/// Gets the current position of the buffer
		UInt GetPos() const;

		void operator=(const Buffer& buffer);
		void operator=(Buffer&& buffer) noexcept;

	private:
		Array<UByte> data;
		UInt size = 0;
		UInt capacity = 16;
		UInt currentPos = 0;

		void Grow();
		static bool NeedsByteSwap(Endian source, Endian target);
	};

	///B BufferError
	/// Base class for all buffer errors
	class BufferError : public Error {
	public:
		BufferError() : Error() {}
		BufferError(const char* const msg) : Error(msg) {}

		virtual String Name() const override {
			return "BufferError";
		}
	};

	///B BufferReadError
	/// Thrown if a read error occurs
	class BufferReadError : public BufferError {
	public:
		BufferReadError() : BufferError() {}
		BufferReadError(const char* const msg) : BufferError(msg) {}

		virtual String Name() const override {
			return "BufferReadError";
		}
	};

	///B BufferPosError
	/// Thrown if the position of a buffer is invalid
	class BufferPosError : public BufferError {
		public:
		BufferPosError() : BufferError() {}
		BufferPosError(const char* const msg) : BufferError(msg) {}

		virtual String Name() const override {
			return "BufferPosError";
		}
	};

	inline Buffer::Buffer() {
		data = Array<UByte>(capacity);
	}

	inline Buffer::Buffer(const UInt capacity) {
		this->capacity = capacity;
		data = Array<UByte>(capacity);
	}

	inline Buffer::Buffer(const Buffer& buffer) {
		size = buffer.size;
		capacity = buffer.capacity;
		currentPos = buffer.currentPos;
		data = Array<UByte>(capacity);
		std::memcpy((UByte*)data, (const UByte*)buffer.data, sizeof(UByte) * capacity);
	}

	inline Buffer::Buffer(Buffer&& buffer) noexcept {
		size = buffer.size;
		capacity = buffer.capacity;
		currentPos = buffer.currentPos;
		data = std::move(buffer.data);
	}

	inline Buffer::~Buffer() {
	
	}

	template <class T>
	inline void Buffer::Write(const T& data, const Endian endian) {
		const UByte* d = (const UByte*)&data;

		while (sizeof(T) + currentPos > capacity) {
			Grow();
		}

		if (NeedsByteSwap(Endian::System, endian)) {
			for (UInt i = 0; i < sizeof(T); i++) {
				this->data[currentPos + i] = d[sizeof(T) - i - 1];
			}
		}
		else {
			std::memcpy(&this->data[currentPos], d, sizeof(T));
		}

		currentPos += sizeof(T);
		if (currentPos > size) size = currentPos;
	}

	template <>
	inline void Buffer::Write(const String& data, const Endian endian) {
		while (data.Size() * sizeof(char) + currentPos > capacity) {
			Grow();
		}

		std::memcpy(&this->data[currentPos], (const char*)data, data.Size() * sizeof(char));
		currentPos += data.Size() * sizeof(char);
		if (currentPos > size) size = currentPos;
	}

	template <class T>
	inline T Buffer::Read(const Endian endian) {
		if (currentPos + sizeof(T) > size)
			throw BufferReadError("Not enough bytes to read");

		T data;

		if (NeedsByteSwap(endian, Endian::System)) {
			UByte d[sizeof(T)];

			for (UInt i = 0; i < sizeof(T); i++) {
				d[i] = this->data[currentPos + sizeof(T) - i - 1];
			}

			data = *(T*)d;
		}
		else {
			data = *(T*)&this->data[currentPos];
		}

		currentPos += sizeof(T);
		return data;
	}

	inline String Buffer::ReadString(const UInt bytes) {
		if (currentPos + bytes > size)
			throw BufferReadError("Not enough bytes to read");

		String s = String(&(((const char*)(const UByte*)data)[currentPos]), bytes);
		currentPos += bytes;
		return s;
	}

	inline String Buffer::ToString() const {
		return String((const char*)(const UByte*)data, size);
	}

	inline UInt Buffer::Size() const {
		return size;
	}

	inline UInt Buffer::Capacity() const {
		return capacity;
	}

	inline void Buffer::SetPos(const UInt pos) {
		if (pos > Size()) throw BufferPosError("Invalid buffer position");
		currentPos = pos;
	}

	inline UInt Buffer::GetPos() const {
		return currentPos;
	}

	inline void Buffer::operator=(const Buffer& buffer) {
		size = buffer.size;
		capacity = buffer.capacity;
		currentPos = buffer.currentPos;
		data = Array<UByte>(capacity);
		std::memcpy((UByte*)data, (const UByte*)buffer.data, sizeof(UByte) * capacity);
	}

	inline void Buffer::operator=(Buffer&& buffer) noexcept {
		size = buffer.size;
		capacity = buffer.capacity;
		currentPos = buffer.currentPos;
		data = std::move(buffer.data);
	}

	inline void Buffer::Grow() {
		capacity *= 2;
		Array<UByte> newData = Array<UByte>(capacity);
		std::memcpy((UByte*)newData, (const UByte*)data, sizeof(char) * size);
		data = newData;
	}

	inline bool Buffer::NeedsByteSwap(Endian source, Endian target) {
		UInt i = 3;
		const Endian systemEndian = ((UByte*)&i)[0] == 3 ? Endian::Little : Endian::Big;
		
		if (source == Endian::System) source = systemEndian;
		if (target == Endian::System) target = systemEndian;

		return source != target;
	}
}
