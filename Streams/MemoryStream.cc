#include "MemoryStream.h"
#include "Exception.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <cassert>

namespace IO {

	MemoryStream::MemoryStream() : MemoryStream(0) {}
	MemoryStream::MemoryStream(size_t capacity) {

		__length = 0;
		__capacity = capacity;
		__buffer = (Byte*)malloc(capacity);
		__position = 0;

	}
	MemoryStream::MemoryStream(const char* buffer, size_t size) : MemoryStream(size) {

		memcpy(__buffer, buffer, size);
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
	void MemoryStream::Flush() {

		__length = 0;
		__position = 0;

	}
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

		// Get the number of bytes to read from the buffer.
		size_t bytes = sizeof(Byte);

		// Return false if the buffer does not contain enough bytes to read.
		if (__position + bytes > __length)
			return false;

		// Copy data from the buffer to the output memory.
		memcpy(&byte, __buffer + __position * bytes, bytes);

		// Move the seek position forward by the number of bytes written.
		__position += bytes;

		// A byte was successfully read, so return true.
		return true;

	}
	void MemoryStream::WriteByte(Byte byte) {
	
		// Throw an exception of the stream is not writeable.
		if (!CanWrite())
			throw NotSupportedException();

		// Get the number of bytes required to store the data.
		size_t bytes = sizeof(Byte);

		// Make enough room in the buffer for new data.
		Allocate(bytes);

		// Copy the data to the buffer.
		memcpy(__buffer + __position * bytes, &byte, bytes);

		// Move the seek position forward by the number of bytes written.
		__position += bytes;

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
		if (__position + len > __length)
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
		Allocate(length);

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

		stream.Write(__buffer, Position(), Length() - Position());
		__position = __length;

	}
	void MemoryStream::CopyTo(Stream& stream, size_t size) {

		// Throw an exception of the stream is not readable.
		if (!CanRead())
			throw NotSupportedException();

		stream.Write(__buffer, Position(), size);
		__position += size;

	}
	void MemoryStream::Seek(long offset, SeekOrigin origin) {
	
		// Throw an exception of the stream is not seekable.
		if (!CanSeek())
			throw NotSupportedException();

		switch (origin) {
		case SeekOrigin::Begin:
			if (offset < 0)
				__position = 0;
			else if (offset > __length)
				__position = __length;
			else
				__position = (size_t)offset;
			break;
		case SeekOrigin::Current:
			if (offset + __position < 0)
				__position = 0;
			else if (offset + __position > __length)
				__position = __length;
			else
				__position = (size_t)(offset + __position);
			break;
		case SeekOrigin::End:
			if (offset >= 0)
				__position = __length;
			else 
				__position = (size_t)(offset + __length);
			break;
		}

	}
	bool MemoryStream::CanRead() const {

		return true;

	}
	bool MemoryStream::CanSeek() const {

		return true;

	}
	bool MemoryStream::CanWrite() const {

		return true;

	}

	Byte* MemoryStream::Buffer() {

		return __buffer;

	}
	void MemoryStream::Reserve(size_t capacity) {

		// If the new capacity is less than the current capacity, do nothing.
		if (__capacity >= capacity)
			return;

		// Increase the capacity of the buffer.
		__buffer = (Byte*)realloc(__buffer, capacity);
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

	void MemoryStream::Allocate(size_t bytes) {

		// Increase the size of the buffer if necessary.
		if (__length + bytes > __capacity) {
			size_t new_capacity = __capacity > 0 ? __capacity : 1;
			while (__length + bytes > new_capacity)
				new_capacity *= 2;
			Reserve(new_capacity);
		}
		
	}

}