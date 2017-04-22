#pragma once
#include "Stream.h"
#include "Buffer.h"
#include <climits>
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
		void SeekBits(long long bits, SeekOrigin offset);
		// Sets the bit position within the current stream.
		void SeekBits(long long bits);

		// Returns the next available byte, or -1 if no more bytes are available, and does not advance the read position.
		int Peek();

		// Reads a single bit from the stream.
		bool ReadBool(bool& value);
		// Reads a single byte from the stream between the given minimum and maximum values.
		bool ReadByte(Byte& value, Byte min = 0, Byte max = UCHAR_MAX);
		// Reads the specified number of bytes from the stream into the given array.
		size_t ReadBytes(Byte* value, size_t length);
		// Reads the specified number of bytes from the stream into the given array beginning at the given offset.
		size_t ReadBytes(Byte* value, size_t offset, size_t length);
		// Reads a single character from the stream between the given minimum and maximum values.
		bool ReadChar(signed char& value, signed char min = SCHAR_MIN, signed char max = SCHAR_MAX);
		// Reads bytes from the stream into the given array until a null byte or the end of the stream is encountered.
		size_t ReadString(char* value);
		// Reads length bytes from the stream into the given array.
		size_t ReadString(char* value, size_t length);
		// Reads length bytes from the stream into the given array at the given offset.
		size_t ReadString(char* value, size_t offset, size_t length);
		// Reads bytes from the stream into the given string until a null byte or the end of the stream is encountered.
		size_t ReadString(std::string& value);
		// Reads length bytes from the stream into the given string.
		size_t ReadString(std::string& value, size_t length);
		// Reads a single float from the stream.
		bool ReadFloat(float& value);
		// Reads a single unsigned integer from the stream between the given minimum and maximum values.
		bool ReadInteger(unsigned int& value, unsigned int min = 0, unsigned int max = UINT_MAX);
		// Reads a single signed integer from the stream between the given minimum and maximum values.
		bool ReadInteger(signed int& value, signed int min = INT_MIN, signed int max = INT_MAX);
		// Reads a single unsigned short from the stream between the given minimum and maximum values.
		bool ReadShort(unsigned short& value, unsigned short min = 0, unsigned short max = USHRT_MAX);
		// Reads a signed short from the stream between the given minimum and maximum values.
		bool ReadShort(signed short& value, signed short min = SHRT_MIN, signed short max = SHRT_MAX);

	protected:
		// Reads bytes from the underlying stream until the buffer is full.
		void FillBuffer();
		// Resets the buffer bit and byte positions to the beginning of the buffer.
		void ClearBuffer();
		// Returns the number of unread bits in the buffer.
		size_t UnreadBitsLeft() const;
		// Flushes reads to the underlying stream.
		void FlushRead();

	private:
		Stream* _stream; // Pointer to the underlying stream
		Buffer _buffer; // Read buffer
		size_t _bytes_read; // The number of bytes read from the stream into the read buffer
		size_t _byte_offset; // The current byte offset in the read buffer
		Byte _bit_offset; // The current bit offset in the read buffer

		// Increments the bit offset by 1. If the bit offset becomes 8, the bit offset is reset and the byte offset is incremented.
		void IncrementBitOffset();
		// Writes bits into the buffer, flushing to the underlying stream when the buffer is full.
		bool ReadBits(uint32_t& value, int bits);

	};

}