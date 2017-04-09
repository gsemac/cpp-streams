#pragma once
#include "Stream.h"
#include <climits>
#include <stdint.h>

namespace IO {

	class BinaryWriter {

	public:
		BinaryWriter(Stream& stream);
		~BinaryWriter();

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

		void WriteBool(bool value);
		void WriteByte(Byte value, Byte min = 0, Byte max = UCHAR_MAX);
		void WriteBytes(Byte* value, size_t length);
		void WriteBytes(Byte* value, size_t offset, size_t length);
		void WriteChar(signed char value, signed char min = CHAR_MIN, signed char max = CHAR_MAX);
		void WriteString(const char* value);
		void WriteString(const char* value, size_t length);
		void WriteString(const char* value, size_t offset, size_t length);
		void WriteFloat(float value);
		//void WriteDouble(double value);
		void WriteInteger(unsigned int value, unsigned int min = 0, unsigned int max = UINT_MAX);
		void WriteInteger(signed int value, signed int min = INT_MIN, signed int max = INT_MAX);
		void WriteShort(unsigned short value, unsigned short min = 0, unsigned short max = USHRT_MAX);
		void WriteShort(signed short value, signed short min = SHRT_MIN, signed short max = SHRT_MAX);
		
	private:
		Stream* __stream;
		bool __owns_stream;
		Byte* __buffer;
		size_t __buffer_size;
		size_t __byte_offset;
		Byte __bit_offset;

		BinaryWriter();

		void FlushWrite();
		
		void AllocateBuffer(size_t bytes);
		void ClearBuffer();
		
		Byte BitsRemaining();
		void IncrementBitOffset();
		
		int BytesToBits(size_t bytes);
		int BitsToBytes(size_t bits);
		int BitsRequired(uint32_t min, uint32_t max);
		int BitsRequired(uint32_t distinct_values);

		//void WriteBits(void* value, size_t bytes);
		//void WriteBits(void* value, size_t bytes, int bits);
		void WriteBits(uint32_t value, int bits);

	};

}