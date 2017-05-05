#include "BufferedStream.h"
#include "Exception.h"
#include <cstdlib>
#include <cassert>

namespace IO {

	BufferedSteam::BufferedSteam(Stream& stream) : BufferedSteam(stream, 4096) {}
	BufferedSteam::BufferedSteam(Stream& stream, size_t buffer_size) {

		_stream = &stream;
		_buffer = nullptr;
		_buffer_size = buffer_size;
		_read_offset = 0;
		_read_length = 0;
		_write_offset = 0;

	}
	BufferedSteam::~BufferedSteam() {

		// Close the stream, freeing resources.
		Close();

	}

	size_t BufferedSteam::Length() {

		// If the stream is null, we obviously have a length of 0.
		if (!_stream)
			return 0;

		// Flush any writes performed on the buffer.
		if (_write_offset > 0)
			FlushWrite();

		// Return the length of the underlying stream.
		return _stream->Length();

	}
	size_t BufferedSteam::Position() const {

		// If the stream is null, we obviously have a position of 0.
		if (!_stream)
			return 0;

		// Calculate the position in the stream based on the positon of the underlying stream and any read/writes performed.
		return (size_t)((_read_offset - _read_length + _write_offset) + _stream->Position());

	}
	void BufferedSteam::Flush() {

		// Flush any writes performed on the buffer.
		if (_write_offset > 0)
			FlushWrite();

		// Flush any reads performed on the buffer.
		else if (_read_offset < _read_length && _stream->CanSeek())
			FlushRead();

		// Reset read position/length.
		_read_offset = 0;
		_read_length = 0;

	}
	void BufferedSteam::SetLength(size_t length) {

		// If the stream is null, throw an exception.
		if (!_stream)
			throw InvalidOperationException();

		// If the stream doesn't support seeking and writing, throw an exception.
		if (!_stream->CanSeek() || !_stream->CanWrite())
			throw NotSupportedException();

		// Flush any writes performed on the buffer.
		if (_write_offset > 0)
			FlushWrite();

		// Flush any reads performed on the buffer.
		else if (_read_offset < _read_length)
			FlushRead();
		_read_offset = 0;
		_read_length = 0;

		// Set the new length of the underlying stream.
		_stream->SetLength(length);

	}
	bool BufferedSteam::ReadByte(Byte& byte) {

		// If the stream is null, there are no bytes to read.
		if (_stream)
			return false;

		// If the read buffer is empty and the stream does not support reading, throw an exception.
		if (_read_length == 0 && !_stream->CanRead())
			throw NotSupportedException();

		// Read bytes into the buffer if we're at the end (or if the read buffer is empty).
		if (_read_offset == _read_length) {

			// Flush any pending writes.
			if (_write_offset > 0)
				FlushWrite();

			// If a buffer has not been allocated, allocate it now.
			if (!_buffer)
				_buffer = (Byte*)malloc(_buffer_size);

			// Read bytes into the buffer, and reset the read position.
			_read_length = _stream->Read(_buffer, 0, _buffer_size);
			_read_offset = 0;

		}

		// If the read buffer is still empty, there's nothing to read-- return false.
		if (_read_offset == _read_length)
			return false;

		// Read a byte from the buffer, and return true to indicate that a byte was read successfully.
		byte = _buffer[_read_offset++];
		return true;

	}
	void BufferedSteam::WriteByte(Byte byte) {

		// If the stream is null, throw an exception.
		if (!_stream)
			throw InvalidOperationException();

		if (_write_offset == 0) {

			// If the stream does not support writing, throw an exception.
			if (!_stream->CanWrite())
				throw NotSupportedException();

			// Flush reads performed on the buffer.
			if (_read_offset < _read_length)
				FlushRead();
			else {
				_read_offset = 0;
				_read_length = 0;
			}

			// Create the buffer if it doesn't already exist.
			if (!_buffer)
				_buffer = (Byte*)malloc(_buffer_size);

		}

		// Flush writes if the buffer is full.
		if (_write_offset == _buffer_size)
			FlushWrite();

		// Set the value of the next byte, and increment the write position.
		_buffer[_write_offset++] = byte;

	}
	size_t BufferedSteam::Read(void* buffer, size_t offset, size_t length) {

		// If the stream is null, there's nothing to read.
		if (!_stream)
			return 0;

		// Calculate the number of unread bytes.
		long long bytes_read = _read_length - _read_offset;

		// If the buffer is empty, we can either read from the stream into the buffer, or bypass it altogether.
		if (bytes_read == 0) {

			// If the stream does not support reading, throw an exception.
			if (!_stream->CanRead())
				throw NotSupportedException();

			// Flush any writes performed on the buffer.
			if (_write_offset > 0)
				FlushWrite();

			// If the read length is longer than the buffer, bypass the buffer altogether.
			if (length >= _buffer_size) {

				// Read directly into the output buffer.
				bytes_read = _stream->Read(buffer, offset, length);

				// Clear the read buffer.
				_read_offset = 0;
				_read_length = 0;

				// Return the number of bytes read.
				return bytes_read;

			}

			// Allocate the read buffer if it hasn't already been allocated.
			if (!_buffer)
				_buffer = (Byte*)malloc(_buffer_size);

			// Read from the stream into the buffer.
			bytes_read = _stream->Read(_buffer, 0, _buffer_size);

			// If no bytes were read from the stream, there's nothing to read.
			if (bytes_read == 0)
				return 0;

			_read_offset = 0;
			_read_length = bytes_read;

		}

		// Copy the number of bytes read or the requested length-- Whichever is fewer.
		if (bytes_read > length)
			bytes_read = length;

		// Copy the bytes into the output buffer.
		memcpy((Byte*)buffer + offset * sizeof(Byte), _buffer + _read_offset * sizeof(Byte), bytes_read);
		_read_offset += bytes_read;

		// If we've reached the end of the buffer but didn't read enough bytes, read the remaining bytes directly from the stream.
		if (bytes_read < length) {
			bytes_read += _stream->Read(buffer, offset + bytes_read * sizeof(Byte), length - bytes_read * sizeof(Byte));
			_read_offset = 0;
			_read_length = 0;
		}

		// Return the number of bytes read.
		return bytes_read;

	}
	void BufferedSteam::Write(const void* buffer, size_t offset, size_t length) {

		// If the stream is null, throw an exception.
		if (!_stream)
			throw InvalidOperationException();

		if (_write_offset == 0) {

			// If the stream does not support writing, throw an exception.
			if (!_stream->CanWrite())
				throw NotSupportedException();

			// Flush reads performed on the buffer.
			if (_read_offset < _read_length)
				FlushRead();
			else {
				_read_offset = 0;
				_read_length = 0;
			}

		}
		else if (_write_offset > 0) {

			// Get the number of bytes left in the buffer.
			long bytes_left = _buffer_size - _write_offset;
			if (bytes_left > 0) {

				if (bytes_left > length)
					bytes_left = length;

				// Fill up the write buffer (as much as we can).
				memcpy((Byte*)buffer + offset * sizeof(Byte), _buffer + _write_offset * sizeof(Byte), bytes_left);
				_write_offset += bytes_left;

				// If we could fit all of the bytes into the buffer, return here.
				if (length == bytes_left)
					return;

				// Adjust the offset and byte length according to bytes written.
				offset += bytes_left;
				length -= bytes_left;

			}

			// Write the buffer to the underlying stream so we can reset the buffer.
			_stream->Write(_buffer, 0, _write_offset);
			_write_offset = 0;

		}

		// If the bytes to write is greater than the buffer size, just write directly to the underlying stream.
		if (length >= _buffer_size) {
			_stream->Write(buffer, offset, length);
			return;
		}

		// If there are zero bytes, don't bother doing anything.
		else if (length == 0)
			return;

		// Allocate the write buffer if it hasn't already been allocated.
		if (!_buffer)
			_buffer = (Byte*)malloc(_buffer_size);

		// Copy the remaining bytes into the buffer.
		memcpy(_buffer, (Byte*)buffer + offset * sizeof(Byte), length);
		_write_offset = length;

	}
	void BufferedSteam::Close() {

		// Dispose of the buffer if it was allocated.
		if (_buffer)
			free(_buffer);
		_buffer = nullptr;

		// Set stream to null.
		_stream = nullptr;

	}
	size_t BufferedSteam::Seek(long long offset, SeekOrigin origin) {

		// Throw error if the underlying stream does not support seeking.
		if (!_stream->CanSeek())
			throw NotSupportedException();

		// Write pending bytes to the stream.
		if (_write_offset > 0)
			FlushWrite();

		// Adjust relative seeking position if we've been reading bytes.
		else if (origin == SeekOrigin::Current)
			offset -= _read_length - _read_offset;

		// Store the previous postition the stream, as well as the new position.
		long long old_pos = (_read_offset - _read_length) + _stream->Position();
		long long pos = _stream->Seek(offset, origin);

		// If there is data in the read buffer, we may need to adjust its contents.
		if (_read_length > 0) {

			if (old_pos == pos) {

				if (_read_offset > 0) {

					// Move the buffer back by the read position.
					memcpy(_buffer, _buffer + _read_offset * sizeof(Byte), _read_length - _read_offset);
					_read_length -= _read_offset;
					_read_offset = 0;

				}

				// If there is still data in the buffer, updating stream's position.
				if (_read_length > 0)
					_stream->Seek(_read_length, SeekOrigin::Current);

			}
			else if (old_pos - _read_offset < pos && pos < old_pos + _read_length - _read_offset) {

				long diff = pos - old_pos;
				memcpy(_buffer, _buffer + (_read_offset + diff) * sizeof(Byte), _read_length - (_read_offset + diff));
				_read_length -= (_read_offset + diff);
				_read_offset = 0;
				if (_read_length > 0)
					_stream->Seek(_read_length, SeekOrigin::Current);

			}
			else {

				// Get rid of the read buffer.
				_read_offset = 0;
				_read_length = 0;

			}

		}

		// Return the new position.
		return pos;

	}
	size_t BufferedSteam::Seek(long long position) {

		return Seek(position, SeekOrigin::Begin);

	}
	bool BufferedSteam::CanRead() const {

		return _stream && _stream->CanRead();

	}
	bool BufferedSteam::CanSeek() const {

		return _stream && _stream->CanSeek();

	}
	bool BufferedSteam::CanWrite() const {

		return _stream && _stream->CanWrite();

	}

	void BufferedSteam::FlushRead() {

		// Adjust the seek position in the underlying stream to match the seeks performed on the read buffer.
		// e.g., if we've seeked 1 byte into a 32-bit buffer, we need to seek the stream back 31 bytes.
		if (_read_offset - _read_length != 0)
			_stream->Seek(_read_offset - _read_length, IO::SeekOrigin::Current);

		_read_offset = 0;
		_read_length = 0;

	}
	void BufferedSteam::FlushWrite() {

		// Write the contents of the buffer to the stream, and reset the write position.
		_stream->Write(_buffer, 0, _write_offset);
		_write_offset = 0;

		// Flush the underlying stream.
		_stream->Flush();

	}

}