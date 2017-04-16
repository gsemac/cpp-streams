#include "BitWriter.h"
#include "Exception.h"
#include <cstdlib>
#include <algorithm>
#include <cassert>
#include <iostream>

namespace IO {

	BitWriter::BitWriter() {

		__stream = nullptr;
		__owns_stream = false;
		__buffer = nullptr;
		__byte_offset = 0;
		__bit_offset = 0;

		// Set buffer to 64-bits initially, which should be enough to contain most primitive types.
		_buffer_size = 8;
		AllocateBuffer(_buffer_size);

	}
	BitWriter::BitWriter(Stream& stream) : BitWriter() {

		__stream = &stream;

	}
	BitWriter::~BitWriter() {

		// Free the write buffer if one has been allocated.
		if (__buffer)
			free(__buffer);

	}

	Stream& BitWriter::BaseStream() {

		// If there is no stream, throw error.
		if (!__stream)
			throw IO::IOException();

		return *__stream;

	}

	void BitWriter::Close() {

		// Close the underlying stream.
		if (__stream)
			__stream->Close();
		__stream = nullptr;

	}
	void BitWriter::Flush() {

		// If there is no stream, throw error.
		if (!__stream)
			throw IO::IOException();

		// Flush the write buffer.
		FlushWrite();

		// Flush the underlying stream.
		__stream->Flush();

	}
	void BitWriter::Seek(long long position, SeekOrigin offset) {

		// If there is no stream or the stream does not support seeking, throw error.
		if (!__stream || !__stream->CanSeek())
			throw NotSupportedException();

		// Flush any data in the write buffer to the stream before seeking to a new position.
		FlushWrite();

		// Seek the underlying stream.
		__stream->Seek(position, offset);

	}
	void BitWriter::Seek(long long position) {

		Seek(position, SeekOrigin::Begin);

	}
	void BitWriter::BitSeek(long long bits, SeekOrigin offset) {

		// Throw an exception of the underlying stream does not support reading.
		// We will need to read from the stream into the write buffer in order to modify individual bits.
		if (!__stream || !__stream->CanRead())
			throw NotSupportedException();

		// Convert the offset into one relative to the start of the stream.
		switch (offset) {
		case SeekOrigin::Current:
			bits += __stream->Position() * 8;
			bits += __bit_offset;
			break;
		case SeekOrigin::End:
			bits += __stream->Length() * 8;
			break;
		}

		// Get the number of bytes that we'll need to advance into the stream.
		long long bytes = bits / 8;

		// Calculate the remaining bit offset.
		bits -= bytes * 8;

		// Seek by the number of bytes.
		Seek(bytes);

		// Fill the write buffer with data from the stream.
		long long bytes_read = __stream->Read(__buffer, 0, _buffer_size);
	
		// Seek back to the original position.
		Seek(-bytes_read, SeekOrigin::Current);

		// Set the bit and byte offsets.
		__bit_offset = bits;
		__byte_offset = 0;

	}
	void BitWriter::BitSeek(long long bits) {

		BitSeek(bits, SeekOrigin::Begin);

	}

	void BitWriter::WriteBool(bool value) {

		WriteBits(value, 1);

	}
	void BitWriter::WriteByte(Byte value, Byte min, Byte max) {

		WriteBits(value, BitsRequired(min, max));

	}
	void BitWriter::WriteBytes(Byte* value, size_t length) {

		WriteBytes(value, 0, length);

	}
	void BitWriter::WriteBytes(Byte* value, size_t offset, size_t length) {

		// Increment the pointer by the offset value.
		value += offset;

		// Write bytes from the array.
		for (size_t i = 0; i < length; ++i)
			WriteByte(*(value++));

	}
	void BitWriter::WriteChar(signed char value, signed char min, signed char max) {

		unsigned char shift = (min < 0) ? -min : 0;
		WriteBits(value + shift, BitsRequired(min + shift, max + shift));

	}
	void BitWriter::WriteString(const char* value) {

		size_t length = std::strlen(value) + 1;
		WriteString(value, length);

	}
	void BitWriter::WriteString(const char* value, size_t length) {

		WriteString(value, 0, length);

	}
	void BitWriter::WriteString(const char* value, size_t offset, size_t length) {

		value += offset;
		for (size_t i = 0; i < length; ++i)
			WriteByte(*(value++));

	}
	void BitWriter::WriteFloat(float value) {

		union FloatInt {
			unsigned int i;
			float f;
		} float_int;

		float_int.f = value;
		WriteInteger(float_int.i);

	}
	void BitWriter::WriteInteger(unsigned int value, unsigned int min, unsigned int max) {

		WriteBits(value - min, BitsRequired(min, max));

	}
	void BitWriter::WriteInteger(signed int value, signed int min, signed int max) {

		unsigned int shift = (min < 0) ? -min : 0;
		WriteBits(value + shift, BitsRequired(min + shift, max + shift));

	}
	void BitWriter::WriteShort(unsigned short value, unsigned short min, unsigned short max) {

		WriteBits(value, BitsRequired(min, max));

	}
	void BitWriter::WriteShort(signed short value, signed short min, signed short max) {

		unsigned short shift = (min < 0) ? -min : 0;
		WriteBits(value + shift, BitsRequired(min + shift, max + shift));

	}

	void BitWriter::FlushWrite() {

		// Write the buffer to the underlying stream. If we've written any bits to the current byte, flush it.
		size_t length = __byte_offset + (__bit_offset > 0);
		if (__buffer && length > 0)
			__stream->Write(__buffer, 0, length);

		// Reset the buffer.
		ClearBuffer();

	}

	void BitWriter::AllocateBuffer(size_t bytes) {

		// Create a new buffer.
		Byte* new_buffer = (Byte*)calloc(bytes, sizeof(Byte));

		// If the buffer isn't empty, copy the contents of the old buffer into the new buffer.
		if (__buffer && (__byte_offset > 0 || __bit_offset > 0))
			memcpy(new_buffer, __buffer, __byte_offset > 0 ? __byte_offset : 1);

		// Free the old buffer (if it exists).
		if (__buffer)
			free(__buffer);

		// Apply the new buffer.
		__buffer = new_buffer;

	}
	void BitWriter::ClearBuffer() {

		// Zero-out the portion of the buffer we've written to.
		size_t length = __byte_offset + (__bit_offset > 0);
		memset(__buffer, 0, length);

		// Reset buffer offsets.
		__byte_offset = 0;
		__bit_offset = 0;

	}

	Byte BitWriter::BitsRemaining() const {

		return (_buffer_size * 8) - (__byte_offset * 8 + __bit_offset);

	}
	void BitWriter::IncrementBitOffset() {

		if (__bit_offset == 7) {

			// If we've reached the end of the byte, reset bit count, increment byte count.
			__bit_offset = 0;
			++__byte_offset;

			// If we've reached the end of the buffer, flush write.
			if (__byte_offset == _buffer_size)
				FlushWrite();

		}
		else
			++__bit_offset;

	}

	void BitWriter::WriteBits(uint32_t value, int bits) {

		// If we're writing a single byte and the bit offset is 0, write it directly.
		if (bits == 8 && !__bit_offset) {
			__buffer[__byte_offset++] = value;
			if (__byte_offset == _buffer_size)
				FlushWrite();
			return;
		}

		// Write "bits" least-significant bits from the value.
		for (int i = bits - 1; i >= 0; --i) {
			__buffer[__byte_offset] |= ((value >> i) & 1) << (7 - __bit_offset);
			IncrementBitOffset();
		}

	}

}