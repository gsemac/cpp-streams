#pragma once
#include "IStream.h"
#include "Exception.h"
#include <cassert>

// The following definitions are defaults and should be overridden by the derived class to improve efficiency.

namespace IO {

	size_t IStream::Read(void* buffer, size_t offset, size_t length) {
		
		if (!CanRead())
			throw NotSupportedException();

		Byte* addr = (Byte*)buffer + offset * sizeof(Byte);
		
		Byte byte;
		size_t bytes_read;
		for (bytes_read = 0; bytes_read < length && ReadByte(byte); ++bytes_read) {
			*addr = byte;
			addr += sizeof(Byte);
		}
	
		return bytes_read;

	}
	void IStream::Write(const void* buffer, size_t offset, size_t length) {
	
		if (!CanWrite())
			throw NotSupportedException();

		Byte* addr = (Byte*)buffer + offset * sizeof(Byte);
		
		for (size_t i = 0; i < length; ++i)
			WriteByte(*(addr + i));

	}
	void IStream::Close() {}
	void IStream::CopyTo(IStream& stream) {
		
		if (!CanRead() || !stream.CanWrite())
			throw NotSupportedException();

		Byte byte;
		while (ReadByte(byte))
			stream.WriteByte(byte);

	}
	void IStream::CopyTo(IStream& stream, size_t buffer_size) {
		
		// Throw an exception of the stream is not readable, or the output stream is not writeable.
		if (!CanRead() || !stream.CanWrite())
			throw NotSupportedException();

		// Create a buffer of the required size.
		Byte* buf = new Byte[buffer_size];

		// Read/write until no more bytes can be read.
		size_t bytes_read;
		while (bytes_read = Read(buf, 0, buffer_size), bytes_read > 0) 
			stream.Write(buf, 0, bytes_read);

		// Dispose of the buffer.
		delete[] buf;

	}

	IStream& IStream::operator << (Byte byte) {

		WriteByte(byte);

		return *this;

	}
	IStream& IStream::operator >> (Byte& byte) {

		ReadByte(byte);

		return *this;

	}

}