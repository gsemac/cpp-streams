#pragma once
#include "Stream.h"
#include <cassert>

// The following definitions are defaults and should be overridden by the derived class to improve efficiency.

namespace IO {

	size_t Stream::Read(void* buffer, size_t offset, size_t length) {
		assert(CanRead());

		Byte* addr = (Byte*)buffer + offset * sizeof(Byte);
		
		Byte byte;
		size_t bytes_read;
		for (bytes_read = 0; bytes_read < length && ReadByte(byte); ++bytes_read) {
			*addr = byte;
			addr += sizeof(Byte);
		}

		return bytes_read;

	}
	void Stream::Write(const void* buffer, size_t offset, size_t length) {
		assert(CanWrite());

		Byte* addr = (Byte*)buffer + offset * sizeof(Byte);
		
		for (size_t i = 0; i < length; ++i)
			WriteByte(*(addr + i));

	}
	void Stream::Close() {}
	void Stream::CopyTo(Stream& stream) {
		assert(CanRead());

		Byte byte;
		while (ReadByte(byte))
			stream.WriteByte(byte);

	}
	void Stream::CopyTo(Stream& stream, size_t size) {
		assert(CanRead());

		Byte byte;
		for (size_t i = 0; i < size && ReadByte(byte); ++i)
			stream.WriteByte(byte);

	}

	Stream& Stream::operator << (Byte byte) {

		WriteByte(byte);

		return *this;

	}
	Stream& Stream::operator >> (Byte& byte) {

		ReadByte(byte);

		return *this;

	}

}