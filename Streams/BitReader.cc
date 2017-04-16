#include "BitReader.h"
#include "Exception.h"
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <bitset>

namespace IO {

	BitReader::BitReader(Stream& stream) :
		_buffer(8, true) {

		_stream = &stream;
		_byte_offset = 0;
		_bit_offset = 0;
		_bytes_read = 0;

	}
	BitReader::~BitReader() {

		// Flush reads to the underlying stream.
		FlushRead();

	}

	Stream& BitReader::BaseStream() {

		return *_stream;

	}

	void BitReader::Close() {

		// If there is no stream, throw error.
		if (!_stream)
			throw IO::IOException();

		// Close the underlying stream.
		if (_stream)
			_stream->Close();
		_stream = nullptr;

	}
	void BitReader::Flush() {

		// If there is no stream, throw error.
		if (!_stream)
			throw IO::IOException();

		// Flush the read buffer.
		FlushRead();

		// Flush the underlying stream.
		_stream->Flush();

	}
	void BitReader::Seek(long long position, SeekOrigin offset) {

		// Flush reads performed on the buffer.
		FlushRead();

		// Seek the underlying stream.
		Seek(position, offset);

	}
	void BitReader::Seek(long long position) {

		Seek(position, IO::SeekOrigin::Begin);

	}
	void BitReader::BitSeek(long long bits, SeekOrigin offset) {

		// Flush the read buffer.
		FlushRead();

		// Convert the offset into one relative to the start of the stream.
		switch (offset) {
		case SeekOrigin::Current:
			bits += _stream->Position() * 8;
			bits += _bit_offset;
			break;
		case SeekOrigin::End:
			bits += _stream->Length() * 8;
			break;
		}

		// Get the number of bytes that we'll need to advance into the stream.
		long long bytes = bits / 8;

		// Calculate the remaining bit offset.
		bits -= bytes * 8;

		// Seek by the number of bytes.
		_stream->Seek(bytes);

		// Set the bit and byte offsets.
		_bit_offset = bits;
		_byte_offset = 0;

	}
	void BitReader::BitSeek(long long position) {

		BitSeek(position, IO::SeekOrigin::Begin);

	}

	int BitReader::Peek() {

		Byte value;
		if (ReadByte(value))
			return value;
		
		return -1;

	}

	bool BitReader::ReadBool(bool& value) {

		uint32_t valueRead;

		if (!ReadBits(valueRead, 1))
			return false;

		value = (bool)valueRead;

		return true;

	}
	bool BitReader::ReadByte(Byte& value, Byte min, Byte max) {

		uint32_t result = value - min;

		if (!ReadBits(result, BitsRequired(min, max)))
			return false;
		
		value = (Byte)result + min;

		return true;

	}
	size_t BitReader::ReadBytes(Byte* value, size_t length) {

		return ReadBytes(value, 0, length);

	}
	size_t BitReader::ReadBytes(Byte* value, size_t offset, size_t length) {

		Byte currentByte;
		size_t bytesRead = 0;

		while (bytesRead < length && ReadByte(currentByte))
			*(value + offset + bytesRead++) = currentByte;

		return bytesRead;

	}
	bool BitReader::ReadChar(signed char& value, signed char min, signed char max) {

		return false;

	}
	bool BitReader::ReadString(const char* value) {

		return false;

	}
	bool BitReader::ReadString(const char* value, size_t length) {

		return false;

	}
	bool BitReader::ReadString(const char* value, size_t offset, size_t length) {

		return false;

	}
	bool BitReader::ReadFloat(float& value) {

		return false;

	}
	bool BitReader::ReadInteger(unsigned int& value, unsigned int min, unsigned int max) {

		unsigned int result = value - min;

		if (!ReadBits(result, BitsRequired(min, max)))
			return false;

		value = result + min;

		return true;

	}
	bool BitReader::ReadInteger(signed int& value, signed int min, signed int max) {

		unsigned int temp = (unsigned int)(value + (INT_MAX - 1));

		if (!ReadInteger(temp, (unsigned int)(min + (INT_MAX - 1)), max + (INT_MAX - 1)))
			return false;

		value = (signed int)temp - (INT_MAX - 1);

		return true;

	}

	void BitReader::FlushRead() {

		// Adjust the seek position in the underlying stream to match the seeks performed on the read buffer.
		// e.g., if we've seeked 1 byte into a 32-bit buffer, we need to seek the stream back 31 bytes.

		// Clear the byte/bit positions and reset the read buffer.
		ClearBuffer();

	}

	void BitReader::FillBuffer() {

		// Read as much data from the stream as possible into 
		_bytes_read += _stream->Read(_buffer.Address(), _bytes_read, _buffer.Size() - _bytes_read);

	}
	void BitReader::ClearBuffer() {

		// The buffer isn't actually cleared, we just reset the offsets.
		// As long as they're respected, we shouldn't be reading beyond the valid portion of the buffer anyway.
		_bytes_read = 0;

		// Reset buffer offsets.
		_byte_offset = 0;
		_bit_offset = 0;

	}
	size_t BitReader::UnreadBitsLeft() const {

		return BytesToBits(_bytes_read - _byte_offset) - _bit_offset;

	}
	void BitReader::IncrementBitOffset() {

		if (_bit_offset == 7) {

			// If we've reached the end of the current byte, increment the byte offset and reset the bit offset.
			++_byte_offset;
			_bit_offset = 0;

		}
		else
			// Otherwise, just increment the bit offset.
			++_bit_offset;

	}

	bool BitReader::ReadBits(uint32_t& value, int bits) {

		// If we've read to the end of the read buffer, reset the offset pointers.
		if (_byte_offset >= _buffer.Size())
			ClearBuffer();

		// If the buffer is empty (e.g., not initialized or we just cleared it), fill it with data from the stream.
		if (_bytes_read == 0)
			FillBuffer();

		// Do we have enough unread data in the read buffer to return the requested number of bits?
		// If not, shift so that the byte in the current read position is the first item in the buffer. 
		// Only do this if we haven't already read all of the bytes in the buffer.
		if (_byte_offset > 0 && _byte_offset < _bytes_read && UnreadBitsLeft() < bits) {

			_buffer.Shift(-(int)_byte_offset);
			_byte_offset = 0;
			_bytes_read = 1;

			// Try to fill the new buffer space with new data from the stream.
			FillBuffer();

		}

		// If we still don't have enough to read the requested number of bits, return false.
		if (UnreadBitsLeft() < bits)
			return false;

		// Zero-out the output value to clear any existing bits.
		value = 0;

		// Read the desired number of bits into the output variable.
		for (int i = bits - 1; i >= 0; --i) {

			// Read from the reader buffer from right-to-left, but write bytes into the output variable right-to-left.
			value |= GetBit(_buffer[_byte_offset], _bit_offset) << i;

			// Increment the bit offset.
			IncrementBitOffset();

		}

		// Return true, since the read was successful.
		return true;

	}

}