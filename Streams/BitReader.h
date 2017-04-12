#pragma once
#include "Stream.h"
#include "Buffer.h"
#include <climits>
#include <array>

namespace IO {

	class BitReader {

	public:
		BitReader(Stream& stream);
		~BitReader();

		bool ReadBool(bool& value);
		bool ReadByte(Byte& value, Byte min = 0, Byte max = UCHAR_MAX);
		bool ReadBytes(Byte* value, size_t length);
		bool ReadBytes(Byte* value, size_t offset, size_t length);
		bool ReadChar(signed char& value, signed char min = CHAR_MIN, signed char max = CHAR_MAX);
		bool ReadString(const char* value);
		bool ReadString(const char* value, size_t length);
		bool ReadString(const char* value, size_t offset, size_t length);
		bool ReadFloat(float& value);
		bool ReadInteger(unsigned int& value, unsigned int min = 0, unsigned int max = UINT_MAX);
		bool ReadInteger(signed int& value, signed int min = INT_MIN, signed int max = INT_MAX);
		bool ReadShort(unsigned short& value, unsigned short min = 0, unsigned short max = USHRT_MAX);
		bool ReadShort(signed short& value, signed short min = SHRT_MIN, signed short max = SHRT_MAX);
	
	public:
		Stream* _stream;
		Buffer _buffer;
		size_t _byte_offset;
		Byte _bit_offset;
		size_t _bytes_read; // the number of bytes read into the buffer

		void FlushRead();

		void FillReadBuffer();
		void ClearReadBuffer();

		// Returns the number of free bits left in the read buffer.
		size_t UnreadBitsLeft() const;

		void IncrementBitOffset();

		bool ReadBits(uint32_t& value, int bits);

	};

}