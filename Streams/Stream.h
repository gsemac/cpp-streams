#pragma once
#include <stddef.h>
#include <cstdint>

namespace IO {

	typedef uint8_t Byte;

	// Specifies the position in a stream to use for seeking.
	enum class SeekOrigin {
		// Specifies the beginning of a stream.
		Begin,
		// Specifies the current position within a stream.
		Current,
		// Specifies the end of a stream.
		End
	};

	class Stream {
		
	public:
		// When overridden in a derived class, gets the length in bytes of the stream.
		virtual size_t Length() = 0;
		// When overridden in a derived class, gets or sets the position within the current stream.
		virtual size_t Position() const = 0;
		// When overridden in a derived class, clears all buffers for this stream and causes any buffered data to be written to the underlying device.
		virtual void Flush() = 0;
		// When overridden in a derived class, sets the length of the current stream.
		virtual void SetLength(size_t length) = 0;
		// Reads a byte from the stream and advances the position within the stream by one byte, or returns false if at the end of the stream.
		virtual bool ReadByte(Byte& byte) = 0;
		// Writes a byte to the current position in the stream and advances the position within the stream by one byte.
		virtual void WriteByte(Byte byte) = 0;
		// Reads a sequence of bytes from the current stream and advances the position within the stream by the number of bytes read.
		virtual size_t Read(void* buffer, size_t offset, size_t length);
		// Writes a sequence of bytes to the current stream and advances the current position within this stream by the number of bytes written.
		virtual void Write(const void* buffer, size_t offset, size_t length);
		// Closes the current stream and releases any resources (such as sockets and file handles) associated with the current stream.
		virtual void Close();
		// Reads the bytes from the current stream and writes them to another stream.
		virtual void CopyTo(Stream& stream);
		// Reads the bytes from the current stream and writes them to another stream, using a specified buffer size.
		virtual void CopyTo(Stream& stream, size_t size);
		// When overridden in a derived class, sets the position within the current stream.
		virtual void Seek(long offset, SeekOrigin origin) = 0;
		// When overridden in a derived class, gets a value indicating whether the current stream supports reading.
		virtual bool CanRead() const = 0;
		// When overridden in a derived class, gets a value indicating whether the current stream supports seeking.
		virtual bool CanSeek() const = 0;
		// When overridden in a derived class, gets a value indicating whether the current stream supports writing.
		virtual bool CanWrite() const = 0;

		// Writes a byte to the current position in the stream and advances the position within the stream by one byte.
		virtual Stream& operator << (Byte byte);
		// Reads a byte from the stream and advances the position within the stream by one byte, or returns fals if at the end of the stream.
		virtual Stream& operator >> (Byte& byte);

	};

}