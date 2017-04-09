#pragma once
#include "Stream.h"
#include <climits>

namespace IO {

	class BinaryReader {

	public:
		BinaryReader(Stream& stream);
		
		bool ReadInteger(signed int& value, signed int min = INT_MIN, signed int max = INT_MAX);

	private:
		Stream* __stream;
		Byte* __buffer;
		size_t __buffer_size;
		size_t __byte_offset;
		Byte __bit_offset;

		void FlushRead();

		void AllocateBuffer(size_t bytes);

		void ReadBits(uint32_t value, int bits);

	};

}