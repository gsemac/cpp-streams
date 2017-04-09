#include "BinaryWriter.h"
#include "Exception.h"
#include <cstdlib>
#include <algorithm>
#include <cassert>

namespace IO {

	BinaryWriter::BinaryWriter() {

		__stream = nullptr;
		__owns_stream = false;
		__buffer = nullptr;
		__byte_offset = 0;
		__bit_offset = 0;

		// Set buffer to 64-bits initially, which should be enough to contain most primitive types.
		__buffer_size = 8;
		AllocateBuffer(__buffer_size);

	}
	BinaryWriter::BinaryWriter(Stream& stream) : BinaryWriter() {

		__stream = &stream;

	}
	BinaryWriter::~BinaryWriter() {

		// Free the write buffer if one has been allocated.
		if (__buffer)
			free(__buffer);

	}

	Stream& BinaryWriter::BaseStream() {

		return *__stream;

	}

	void BinaryWriter::Close() {

		// Close the underlying stream.
		if (__stream)
			__stream->Close();
		__stream = nullptr;

	}
	void BinaryWriter::Flush() {

		// If there is no stream, throw error.
		if (!__stream)
			throw IO::IOException();

		// Flush the write buffer.
		FlushWrite();

		// Flush the underlying stream.
		__stream->Flush();

	}
	void BinaryWriter::Seek(long long position, SeekOrigin offset) {

		// If there is no stream or the stream does not support seeking, throw error.
		if (!__stream || !__stream->CanSeek())
			throw NotSupportedException();

		// Seek the underlying stream.
		__stream->Seek(position, offset);

	}
	void BinaryWriter::Seek(long long position) {

		Seek(position, SeekOrigin::Begin);

	}

	void BinaryWriter::WriteBool(bool value) {

		WriteBits(value, 1);

	}
	void BinaryWriter::WriteByte(Byte value, Byte min, Byte max) {

		WriteBits(value, BitsRequired(min, max));

	}
	void BinaryWriter::WriteBytes(Byte* value, size_t length) {

		WriteBytes(value, 0, length);

	}
	void BinaryWriter::WriteBytes(Byte* value, size_t offset, size_t length) {

		// Increment the pointer by the offset value.
		value += offset;

		// Write bytes from the array.
		for (size_t i = 0; i < length; ++i)
			WriteByte(*(value++));

	}
	void BinaryWriter::WriteChar(signed char value, signed char min, signed char max) {

		unsigned char shift = (min < 0) ? -min : 0;
		WriteBits(value + shift, BitsRequired(min + shift, max + shift));

	}
	void BinaryWriter::WriteString(const char* value) {

		size_t length = std::strlen(value) + 1;
		WriteString(value, length);

	}
	void BinaryWriter::WriteString(const char* value, size_t length) {

		WriteString(value, 0, length);

	}
	void BinaryWriter::WriteString(const char* value, size_t offset, size_t length) {

		value += offset;
		for (size_t i = 0; i < length; ++i)
			WriteByte(*(value++));

	}
	void BinaryWriter::WriteFloat(float value) {

		union FloatInt {
			unsigned int i;
			float f;
		} float_int;

		float_int.f = value;
		WriteInteger(float_int.i);

	}
	void BinaryWriter::WriteInteger(unsigned int value, unsigned int min, unsigned int max) {

		WriteBits(value, BitsRequired(min, max));

	}
	void BinaryWriter::WriteInteger(signed int value, signed int min, signed int max) {

		unsigned int shift = (min < 0) ? -min : 0;
		WriteBits(value + shift, BitsRequired(min + shift, max + shift));

	}
	void BinaryWriter::WriteShort(unsigned short value, unsigned short min, unsigned short max) {

		WriteBits(value, BitsRequired(min, max));

	}
	void BinaryWriter::WriteShort(signed short value, signed short min, signed short max) {

		unsigned short shift = (min < 0) ? -min : 0;
		WriteBits(value + shift, BitsRequired(min + shift, max + shift));

	}

	void BinaryWriter::FlushWrite() {

		// Write the buffer to the underlying stream. If we've written any bits to the current byte, flush it.
		size_t length = __byte_offset + (__bit_offset > 0);
		if (__buffer && length > 0)
			__stream->Write(__buffer, 0, length);

		// Reset the buffer.
		ClearBuffer();

	}

	void BinaryWriter::AllocateBuffer(size_t bytes) {

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
	void BinaryWriter::ClearBuffer() {

		// Zero-out the portion of the buffer we've written to.
		size_t length = __byte_offset + (__bit_offset > 0);
		memset(__buffer, 0, length);

		// Reset buffer offsets.
		__byte_offset = 0;
		__bit_offset = 0;

	}

	Byte BinaryWriter::BitsRemaining() {

		return (__buffer_size * 8) - (__byte_offset * 8 + __bit_offset);

	}
	void BinaryWriter::IncrementBitOffset() {

		if (__bit_offset == 7) {

			// If we've reached the end of the byte, reset bit count, increment byte count.
			__bit_offset = 0;
			++__byte_offset;

			// If we've reached the end of the buffer, flush write.
			if (__byte_offset == __buffer_size)
				FlushWrite();

		}
		else
			++__bit_offset;

	}

	int BinaryWriter::BytesToBits(size_t bytes) {

		return bytes * 8;

	}
	int BinaryWriter::BitsToBytes(size_t bits) {

		return (int)(std::ceil)(bits / 8.0f);

	}
	int BinaryWriter::BitsRequired(uint32_t min, uint32_t max) {

		// Make sure that max and min values are valid.
		assert(max > min);
		assert(max >= 1);

		// 0x7FFFFFF is the maximum value of a (signed) 32-bit integer.
		// If the difference between the max/min is this high, we require a full 32 bits. 
		if (max - min >= 0x7FFFFFF)
			return 32;

		// Otherwise, calculate the bits required to represent n distinct values,
		// where n is the range bounded by min and max, plus 0.
		return BitsRequired(max - min + 1);

	}
	int BinaryWriter::BitsRequired(uint32_t distinct_values) {

		// (It doesn't make sense to have 0 distinct values.)
		assert(distinct_values > 1);

		// Calculate the maximum value that can be represented-- This means all bits are equal to 1.
		unsigned int max = distinct_values - 1;

		// Loop until we reach a 0 bit, or expend all 32 bits.
		for (int index = 0; index < 32; ++index) {

			if ((max & ~1) == 0)
				return index + 1;
			max >>= 1;

		}

		// If we haven't returned yet, we require the full 32 bits.
		return 32;

	}

	/*
	void BinaryWriter::WriteBits(void* value, size_t bytes) {

		WriteBits(value, bytes, BytesToBits(bytes));

	}
	void BinaryWriter::WriteBits(void* value, size_t bytes, int bits) {

		// If the bit offset is 0 and bits is the number of bits in the data, we can easily just copy the bytes directly.
		if (__bit_offset == 0 && bits == BytesToBits(bytes)) {

			Byte* ptr = (Byte*)value;
			for (size_t i = 0; i < bytes; ++i) {
				__buffer[__byte_offset++] = *ptr;
				ptr += sizeof(Byte);
				if (__byte_offset == __buffer_size)
					FlushWrite();
			}
			return;

		}

		// We want to copy the least-significant bits from the value.
		// Adjust the address so that it points to the starting byte we want to copy.
		int byte_index = (sizeof(Byte) * bytes) - BitsToBytes(bits);
		int bytes_left = bytes - byte_index;
		Byte* ptr = (Byte*)value + byte_index;
		int bit_index = (std::min)(7, bits - 1);

		for (int i = 0; i < bytes_left; ++i) {

			// Write the bits from the current byte to the buffer.
			for (int bit = bit_index; bit >= 0 && bits > 0; --bit, --bits) {
				__buffer[__byte_offset] |= ((*ptr >> bit) & 1) << (7 * sizeof(Byte) - __bit_offset);
				IncrementBitOffset();
			}

			// Increment the byte pointer.
			ptr += sizeof(Byte);

			// Reset bit index.
			bit_index = 7;

		}

	}
	*/
	void BinaryWriter::WriteBits(uint32_t value, int bits) {

		// If we're writing a single byte and the bit offset is 0, write it directly.
		if (bits == 8 && !__bit_offset) {
			__buffer[__byte_offset++] = value;
			if (__byte_offset == __buffer_size)
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