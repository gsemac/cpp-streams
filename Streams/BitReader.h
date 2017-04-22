#pragma once
#include "Stream.h"
#include "Buffer.h"
#include <climits>
#include <array>
#include <string>

namespace IO {

	class BitReader {

	public:
		BitReader(Stream& stream);
		~BitReader();

		// Gets the underlying stream of the BinaryWriter.
		Stream& BaseStream();

		// Closes the current BinaryWriter and the underlying stream.
		void Close();
		// Clears all buffers for the current writer and causes any buffered data to be written to the underlying device.
		void Flush();

		// Sets the position within the current stream.
		void Seek(long long position, SeekOrigin offset);
		// Sets the position within the current stream.
		void Seek(long long position);
		// Sets the bit position within the current stream.
		void BitSeek(long long bits, SeekOrigin offset);
		// Sets the bit position within the current stream.
		void BitSeek(long long bits);

		// Returns the next available byte, or -1 if no more bytes are available, and does not advance the read position.
		int Peek();
		
		bool ReadBool(bool& value);
		bool ReadByte(Byte& value, Byte min = 0, Byte max = UCHAR_MAX);
		size_t ReadBytes(Byte* value, size_t length);
		size_t ReadBytes(Byte* value, size_t offset, size_t length);
		bool ReadChar(signed char& value, signed char min = SCHAR_MIN, signed char max = SCHAR_MAX);
		size_t ReadString(char* value);
		size_t ReadString(char* value, size_t length);
		size_t ReadString(char* value, size_t offset, size_t length);
		size_t ReadString(std::string& value);
		size_t ReadString(std::string& value, size_t length);
		bool ReadFloat(float& value);
		bool ReadInteger(unsigned int& value, unsigned int min = 0, unsigned int max = UINT_MAX);
		bool ReadInteger(signed int& value, signed int min = INT_MIN, signed int max = INT_MAX);
		bool ReadShort(unsigned short& value, unsigned short min = 0, unsigned short max = USHRT_MAX);
		bool ReadShort(signed short& value, signed short min = SHRT_MIN, signed short max = SHRT_MAX);
	
	public:
		Stream* _stream;
		Buffer _buffer;
		size_t _byte_offset;
		size_t _bytes_read; // the number of bytes read into the buffer
		Byte _bit_offset;

		void FlushRead();

		void FillBuffer();
		void ClearBuffer();

		// Returns the number of free bits left in the read buffer.
		size_t UnreadBitsLeft() const;

		void IncrementBitOffset();

		bool ReadBits(uint32_t& value, int bits);

	};

}