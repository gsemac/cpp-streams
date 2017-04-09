#include "BinaryReader.h"
#include <cstdlib>
#include <cstring>

namespace IO {

	BinaryReader::BinaryReader(Stream& stream) {

		__stream = &stream;
		__buffer = nullptr;
		__buffer_size = 8;
		__byte_offset = 0;
		__bit_offset = 0;

		AllocateBuffer(__buffer_size);

	}

	bool BinaryReader::ReadInteger(signed int& value, signed int min, signed int max) {

		Byte byte;
		__stream->ReadByte(byte);

		unsigned int shift = (min < 0) ? -min : 0;
		signed int tmp = byte;
		tmp -= shift;
		
		value = tmp;

		return true;

		//unsigned int shift = (min < 0) ? -min : 0;
		//WriteBits(value + shift, BitsRequired(min + shift, max + shift));

	}

	void BinaryReader::FlushRead() {



	}

	void BinaryReader::AllocateBuffer(size_t bytes) {

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
	
	void BinaryReader::ReadBits(uint32_t value, int bits) {

		//// If we're writing a single byte and the bit offset is 0, write it directly.
		//if (bits == 8 && !__bit_offset) {
		//	__buffer[__byte_offset++] = value;
		//	if (__byte_offset == __buffer_size)
		//		FlushWrite();
		//	return;
		//}

		//// Write "bits" least-significant bits from the value.
		//for (int i = bits - 1; i >= 0; --i) {
		//	__buffer[__byte_offset] |= ((value >> i) & 1) << (7 - __bit_offset);
		//	IncrementBitOffset();
		//}

	}

}