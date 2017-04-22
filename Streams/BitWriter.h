#pragma once
#include "Stream.h"
#include <climits>
#include <stdint.h>
#include <string>

namespace IO {

	class BitWriter {

	public:
		BitWriter(Stream& stream);
		~BitWriter();

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
		void SeekBits(long long bits, SeekOrigin offset);
		// Sets the bit position within the current stream.
		void SeekBits(long long bits);

		void WriteBool(bool value);
		void WriteByte(Byte value, Byte min = 0, Byte max = UCHAR_MAX);
		void WriteBytes(Byte* value, size_t length);
		void WriteBytes(Byte* value, size_t offset, size_t length);
		void WriteChar(signed char value, signed char min = SCHAR_MIN, signed char max = SCHAR_MAX);
		void WriteString(const char* value);
		void WriteString(const char* value, size_t length);
		void WriteString(const char* value, size_t offset, size_t length);
		void WriteString(const std::string& value);
		void WriteFloat(float value);
		void WriteInteger(unsigned int value, unsigned int min = 0, unsigned int max = UINT_MAX);
		void WriteInteger(signed int value, signed int min = INT_MIN, signed int max = INT_MAX);
		void WriteShort(unsigned short value, unsigned short min = 0, unsigned short max = USHRT_MAX);
		void WriteShort(signed short value, signed short min = SHRT_MIN, signed short max = SHRT_MAX);
		
	private:
		Stream* _stream;
		bool _owns_stream;
		Byte* _buffer;
		size_t _buffer_size;
		size_t _byte_offset;
		Byte _bit_offset;

		BitWriter();

		void FlushWrite();
		
		void AllocateBuffer(size_t bytes);
		void ClearBuffer();
		
		size_t BitsRemaining() const;
		void IncrementBitOffset();
		
		void WriteBits(uint32_t value, int bits);

	};

}