#include "MemoryStream.h"
#include "Exception.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace IO {

	MemoryStream::MemoryStream() : MemoryStream(0) {}
	MemoryStream::MemoryStream(size_t capacity) {

		__length = 0;
		__capacity = capacity;
		__buffer = (Byte*)malloc(capacity);
		__position = 0;

	}
	MemoryStream::MemoryStream(const char* buffer, size_t size) : MemoryStream(size) {

		__buffer = (Byte*)memcpy(__buffer, buffer, size);
		__length = size;
		__capacity = size;

	}
	MemoryStream::~MemoryStream() {

		Close();

	}

	size_t MemoryStream::Length() {

		return __length;

	}
	size_t MemoryStream::Capacity() const {

		return __capacity;

	}
	size_t MemoryStream::Position() const {

		return __position;

	}
	bool MemoryStream::IsEmpty() const {

		return (__length == 0);

	}
	void MemoryStream::Flush() {}
	void MemoryStream::SetLength(size_t length) {

		// Throw error if the stream does not support writing or seeking.
		if (!CanWrite() || !CanSeek())
			throw NotSupportedException();

		// If the new length is larger than the current capacity, resize the buffer.
		if (length > __capacity)
			Reserve(length);

		// Set the new length.
		__length = length;

		// If the length is less than the seek position, move seek position to end.
		if (__position > __length)
			__position = __length;

	}
	bool MemoryStream::ReadByte(Byte& byte) {

		// Throw an exception of the stream is not readable.
		if (!CanRead())
			throw NotSupportedException();

		// Return false if the buffer does not contain enough bytes to read.
		if (__position + sizeof(Byte) > __length)
			return false;

		// Copy data from the buffer to the output memory.
		memcpy(&byte, __buffer + __position * sizeof(Byte), sizeof(Byte));

		// Move the seek position forward by the number of bytes written.
		__position += sizeof(Byte);

		// A byte was successfully read, so return true.
		return true;

	}
	void MemoryStream::WriteByte(Byte byte) {

		// Throw an exception of the stream is not writeable.
		if (!CanWrite())
			throw NotSupportedException();

		// Make enough room in the buffer for new data.
		AllocateBytes(sizeof(Byte));

		// Copy the data to the buffer.
		memcpy(__buffer + __position * sizeof(Byte), &byte, sizeof(Byte));

		// Move the seek position forward by the number of bytes written.
		__position += sizeof(Byte);

		// If the stream is now longer, increase the length.
		if (__position > __length)
			__length = __position;

	}
	size_t MemoryStream::Read(void* buffer, size_t offset, size_t length) {

		// Throw an exception of the stream is not readable.
		if (!CanRead())
			throw NotSupportedException();

		// Calculate new length if necessary (if length is greater than buffer size).
		size_t len = length;
		if (__position + length > __length)
			if (__position > __length)
				// The stream has been seeked beyond the end of the stream. Nothing to read.
				return 0;
			else
				len = __length - __position;
	
		// If length is 0, return.
		if (len == 0)
			return 0;

		// Copy memory from the buffer to the output buffer.
		memcpy((Byte*)buffer + offset * sizeof(Byte), __buffer + __position * sizeof(Byte), length);

		// Increase the seek position by bytes read.
		__position += len;

		// Return the number of bytes read.
		return len;

	}
	void MemoryStream::Write(const void* buffer, size_t offset, size_t length) {

		// Throw an exception of the stream is not writeable.
		if (!CanWrite())
			throw NotSupportedException();

		// Make enough room in the buffer for new data.
		AllocateBytes(length);

		// Copy memory from the input buffer to the internal buffer.
		memcpy(__buffer + __position * sizeof(Byte), (Byte*)buffer + offset * sizeof(Byte), length);

		// Move the seek position forward by the number of bytes written.
		__position += length;

		// If the stream is now longer, increase the length.
		if (__position > __length)
			__length = __position;

	}
	void MemoryStream::Close() {

		if (__buffer)
			free(__buffer);
		__buffer = nullptr;

	}
	void MemoryStream::CopyTo(Stream& stream) {

		// Throw an exception of the stream is not readable.
		if (!CanRead())
			throw NotSupportedException();

		// If the seek position is greater than or equal to the length of the stream, do nothing.
		if (__position >= __length)
			return;

		// Write remaining contents of the current stream to the other stream.
		stream.Write(__buffer, __position, __length - __position);
		__position = __length;

	}
	void MemoryStream::CopyTo(Stream& stream, size_t buffer_size) {

		Stream::CopyTo(stream, buffer_size);

	}
	size_t MemoryStream::Seek(long long offset, SeekOrigin origin) {

		// Throw an exception of the stream is not seekable.
		if (!CanSeek())
			throw NotSupportedException();

		// Get the origin position from the seek origin.
		size_t origin_position;
		switch (origin) {
		case SeekOrigin::Begin:
			origin_position = 0;
			break;
		case SeekOrigin::Current:
			origin_position = __position;
			break;
		case SeekOrigin::End:
			origin_position = __length;
			break;
		}

		// Throw an error if the new position is less than 0.
		if (offset + origin_position < 0)
			throw IOException("An attempt was made to move the position before the beginning of the stream.");

		// Apply the new position.
		__position = (size_t)(offset + origin_position);

		// Return the new position.
		return __position;

	}
	size_t MemoryStream::Seek(long long position) {

		return Seek(position, SeekOrigin::Begin);

	}
	bool MemoryStream::CanRead() const {

		return __buffer;

	}
	bool MemoryStream::CanSeek() const {

		return __buffer;

	}
	bool MemoryStream::CanWrite() const {

		return __buffer;

	}

	Byte* MemoryStream::Buffer() {

		return __buffer;

	}
	void MemoryStream::Reserve(size_t capacity) {

		// If the new capacity is less than the current capacity, do nothing.
		if (__capacity >= capacity)
			return;

		// Increase the capacity of the buffer and clear the new memory.
		__buffer = (Byte*)realloc(__buffer, capacity);
		memset(__buffer + __capacity, 0, capacity - __capacity);

		__capacity = capacity;

	}
	void MemoryStream::Resize(size_t size, Byte value) {

		// Adjust the size of the buffer.
		__buffer = (Byte*)realloc(__buffer, size);

		// If the buffer is larger, fill new memory with given value.
		if (size > __length)
			memset(__buffer + __length * sizeof(Byte), value, size - __length);

		// Update size and capcity counters.
		__capacity = size;
		__length = size;

		// If the seek position is now beyond the end of the buffer, move it to the end of the buffer.
		if (__position > __length)
			__position = __length;


	}

	void MemoryStream::AllocateBytes(size_t bytes) {

		// Calculate the required capacity. Note that the position may be greater than the length.
		size_t required_capacity = (std::max)(__length, __position) + bytes;

		// Increase buffer capacity if needed.
		if (required_capacity > __capacity) {
			size_t new_capacity = __capacity > 0 ? __capacity : 1;
			while (required_capacity > new_capacity)
				new_capacity *= 2;
			Reserve(new_capacity);
		}

	}

}