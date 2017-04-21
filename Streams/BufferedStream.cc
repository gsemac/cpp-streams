#include "BufferedStream.h"
#include "Exception.h"
#include <cstdlib>
#include <cassert>

namespace IO {

	BufferedSteam::BufferedSteam(Stream& stream) : BufferedSteam(stream, 4096) {}
	BufferedSteam::BufferedSteam(Stream& stream, size_t buffer_size) {

		__stream = &stream;
		__buffer = nullptr;
		__buffer_size = buffer_size;
		__rpos = 0;
		__rlen = 0;
		__wpos = 0;

	}
	BufferedSteam::~BufferedSteam() {

		// Close the stream, freeing resources.
		Close();

	}

	size_t BufferedSteam::Length() {

		// If the stream is null, return 0.
		if (!__stream)
			return 0;

		// If there are any pending writes, flush them now.
		if (__wpos > 0)
			FlushWrite();

		// Return the length of the underlying stream.
		return __stream->Length();

	}
	size_t BufferedSteam::Position() const {

		// If the stream is null, return 0.
		if (!__stream)
			return 0;

		// Calculate offset from the underlying stream position.
		return (size_t)((__rpos - __rlen + __wpos) + __stream->Position());

	}
	void BufferedSteam::Flush() {

		// Flush pending writes.
		if (__wpos > 0)
			FlushWrite();

		// Flush pending reads.
		else if (__rpos < __rlen && __stream->CanSeek())
			FlushRead();

		// Reset read values.
		__rpos = 0;
		__rlen = 0;

	}
	void BufferedSteam::SetLength(size_t length) {

		// If the stream is null, throw error.
		if (!__stream)
			throw InvalidOperationException();

		// If the stream doesn't support seeking and writing, throw error.
		if (!__stream->CanSeek() || !__stream->CanWrite())
			throw NotSupportedException();

		// If there is content in the write buffer, flush write.
		if (__wpos > 0)
			FlushWrite();

		// If reads have been performed, flush reads.
		else if (__rpos < __rlen)
			FlushRead();
		__rpos = 0;
		__rlen = 0;

		// Set the new length of the underlying stream.
		__stream->SetLength(length);

	}
	bool BufferedSteam::ReadByte(Byte& byte) {

		// If the stream is null, return false.
		if (__stream)
			return false;

		// If the read buffer is empty and the stream does not support reading, throw error.
		if (__rlen == 0 && !__stream->CanRead())
			throw NotSupportedException();

		// Read bytes into the buffer if we're at the end of it (or if the read buffer is empty).
		if (__rpos == __rlen) {

			// Flush any pending writes.
			if (__wpos > 0)
				FlushWrite();

			// If a buffer has not been allocated, allocate it now.
			if (!__buffer)
				__buffer = (Byte*)malloc(__buffer_size);

			// Read bytes into the buffer, and reset the read position.
			__rlen = __stream->Read(__buffer, 0, __buffer_size);
			__rpos = 0;

		}

		// If the read buffer is still empty, there's nothing to read-- return false.
		if (__rpos == __rlen)
			return false;

		// Read a byte from the buffer, and return true.
		byte = __buffer[__rpos++];
		return true;

	}
	void BufferedSteam::WriteByte(Byte byte) {

		// If the stream is null, throw error.
		if (!__stream)
			throw InvalidOperationException();

		if (__wpos == 0) {

			// If the stream does not support writing, throw error.
			if (!__stream->CanWrite())
				throw NotSupportedException();

			// Flush read.
			if (__rpos < __rlen)
				FlushRead();
			else {
				__rpos = 0;
				__rlen = 0;
			}

			// Create the buffer if it doesn't already exist.
			if (!__buffer)
				__buffer = (Byte*)malloc(__buffer_size);

		}

		// Flush writes if the buffer is full.
		if (__wpos == __buffer_size)
			FlushWrite();

		// Set the value of the next byte, and increment the write position.
		__buffer[__wpos++] = byte;

	}
	size_t BufferedSteam::Read(void* buffer, size_t offset, size_t length) {

		// If the stream is null, return 0.
		if (!__stream)
			return 0;

		// Calculate the number of unread bytes.
		long bytes_read = __rlen - __rpos;

		// If the buffer is empty, we have the choice of using it or reading directly into the user's buffer.
		if (bytes_read == 0) {

			// If the stream does not support reading, throw error.
			if (!__stream->CanRead())
				throw NotSupportedException();

			//  If there are any pending writes, flush them now.
			if (__wpos > 0)
				FlushWrite();

			// If the read length is longer than the buffer, skip the buffer altogether.
			if (length >= __buffer_size) {

				// Read directly into the user's buffer.
				bytes_read = __stream->Read(buffer, offset, length);

				// Clear the read buffer.
				__rpos = 0;
				__rlen = 0;

				// Return the number of bytes read.
				return bytes_read;

			}

			// Allocate the read buffer if it hasn't already been allocated.
			if (!__buffer)
				__buffer = (Byte*)malloc(__buffer_size);

			// Read from the stream into the buffer.
			bytes_read = __stream->Read(__buffer, 0, __buffer_size);

			// If no bytes were read from the stream, return 0.
			if (bytes_read == 0)
				return 0;

			__rpos = 0;
			__rlen = bytes_read;

		}

		// Copy the number of bytes read or the requested length, whichever is smaller.
		if (bytes_read > length)
			bytes_read = length;

		// Copy the bytes into the user's buffer.
		memcpy((Byte*)buffer + offset * sizeof(Byte), __buffer + __rpos * sizeof(Byte), bytes_read);
		__rpos += bytes_read;

		// If we reached the end of the buffer but didn't read enough bytes, read more from the stream.
		if (bytes_read < length) {
			bytes_read += __stream->Read(buffer, offset + bytes_read * sizeof(Byte), length - bytes_read * sizeof(Byte));
			__rpos = 0;
			__rlen = 0;
		}

		// Return the number of bytes read.
		return bytes_read;

	}
	void BufferedSteam::Write(const void* buffer, size_t offset, size_t length) {

		// If the stream is null, throw error.
		if (!__stream)
			throw InvalidOperationException();

		if (__wpos == 0) {

			// If the stream does not support writing, throw error.
			if (!__stream->CanWrite())
				throw NotSupportedException();

			// Flush pending reads, and clear the read buffer.
			if (__rpos < __rlen)
				FlushRead();
			else {
				__rpos = 0;
				__rlen = 0;
			}

		}
		else if (__wpos > 0) {

			// Get the number of bytes left in the buffer.
			long bytes_left = __buffer_size - __wpos;
			if (bytes_left > 0) {

				if (bytes_left > length)
					bytes_left = length;

				// Write directly into the buffer (as much as we can).
				memcpy((Byte*)buffer + offset * sizeof(Byte), __buffer + __wpos * sizeof(Byte), bytes_left);
				__wpos += bytes_left;

				// If we could fit all of the bytes into the buffer, return here.
				if (length == bytes_left)
					return;

				// Adjust the offset and byte length according to bytes written.
				offset += bytes_left;
				length -= bytes_left;

			}

			// Write the buffer to the underlying stream so we can reset the buffer.
			__stream->Write(__buffer, 0, __wpos);
			__wpos = 0;

		}

		// If the bytes to write is greater than the buffer size, just write directly to the underlying stream.
		if (length >= __buffer_size) {
			__stream->Write(buffer, offset, length);
			return;
		}

		// If there are zero bytes, don't bother doing anything.
		else if (length == 0)
			return;

		// Allocate the write buffer if it hasn't already been allocated.
		if (!__buffer)
			__buffer = (Byte*)malloc(__buffer_size);

		// Copy the remaining bytes into the buffer.
		memcpy(__buffer, (Byte*)buffer + offset * sizeof(Byte), length);
		__wpos = length;

	}
	void BufferedSteam::Close() {

		// Dispose of the buffer if it was allocated.
		if (__buffer)
			free(__buffer);
		__buffer = nullptr;

		// Set stream to null.
		__stream = nullptr;

	}
	size_t BufferedSteam::Seek(long long offset, SeekOrigin origin) {

		// Throw error if the underlying stream does not support seeking.
		if (!__stream->CanSeek())
			throw NotSupportedException();

		// Write pending bytes to the stream.
		if (__wpos > 0)
			FlushWrite();

		// Adjust relative seeking position if we've been reading bytes.
		else if (origin == SeekOrigin::Current)
			offset -= __rlen - __rpos;

		// Store the previous postition the stream, as well as the new position.
		long long old_pos = (__rpos - __rlen) + __stream->Position();
		long long pos = __stream->Seek(offset, origin);

		// If there is data in the read buffer, we may need to adjust its contents.
		if (__rlen > 0) {

			if (old_pos == pos) {

				if (__rpos > 0) {

					// Move the buffer back by the read position.
					memcpy(__buffer, __buffer + __rpos * sizeof(Byte), __rlen - __rpos);
					__rlen -= __rpos;
					__rpos = 0;

				}

				// If there is still data in the buffer, updating stream's position.
				if (__rlen > 0)
					__stream->Seek(__rlen, SeekOrigin::Current);

			}
			else if (old_pos - __rpos < pos && pos < old_pos + __rlen - __rpos) {

				long diff = pos - old_pos;
				memcpy(__buffer, __buffer + (__rpos + diff) * sizeof(Byte), __rlen - (__rpos + diff));
				__rlen -= (__rpos + diff);
				__rpos = 0;
				if (__rlen > 0)
					__stream->Seek(__rlen, SeekOrigin::Current);

			}
			else {

				// Get rid of the read buffer.
				__rpos = 0;
				__rlen = 0;

			}

		}

		// Return the new position.
		return pos;

	}
	size_t BufferedSteam::Seek(long long position) {

		return Seek(position, SeekOrigin::Begin);

	}
	bool BufferedSteam::CanRead() const {

		return __stream && __stream->CanRead();

	}
	bool BufferedSteam::CanSeek() const {

		return __stream && __stream->CanSeek();

	}
	bool BufferedSteam::CanWrite() const {

		return __stream && __stream->CanWrite();

	}

	void BufferedSteam::FlushRead() {

		// Adjust the seek position in the underlying stream to match the seeks performed on the read buffer.
		// e.g., if we've seeked 1 byte into a 32-bit buffer, we need to seek the stream back 31 bytes.
		if (__rpos - __rlen != 0)
			__stream->Seek(__rpos - __rlen, IO::SeekOrigin::Current);

		__rpos = 0;
		__rlen = 0;

	}
	void BufferedSteam::FlushWrite() {

		// Write the contents of the buffer to the stream, and reset the write position.
		__stream->Write(__buffer, 0, __wpos);
		__wpos = 0;

		// Flush the underlying stream.
		__stream->Flush();

	}

}