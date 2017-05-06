#include "IStream.h"

namespace IO {

	class BufferedSteam : public IStream {

	public:
		BufferedSteam(IStream& stream);
		BufferedSteam(IStream& stream, size_t buffer_size);
		~BufferedSteam();

		// Gets the length in bytes of the stream.
		size_t Length() override;
		// Gets the current position of this stream.
		size_t Position() const override;
		// Clears all buffers for this stream and causes any buffered data to be written to the underlying device.
		void Flush() override;
		// Sets the length of the buffered stream.
		void SetLength(size_t length) override;
		// Reads a byte from the underlying stream.
		bool ReadByte(Byte& byte) override;
		// Writes a byte to the current position in the buffered stream.
		void WriteByte(Byte byte) override;
		// Copies bytes from the current buffered stream to a given buffer.
		size_t Read(void* buffer, size_t offset, size_t length) override;
		// Copies bytes to the buffered stream and advances the current position within the buffered stream by the number of bytes written.
		void Write(const void* buffer, size_t offset, size_t length) override;
		// Closes the current stream and releases any resources associated with the current stream.
		void Close() override;
		// Sets the position within the current buffered stream.
		size_t Seek(long long offset, SeekOrigin origin) override;
		// Sets the position within the current buffered stream.
		size_t Seek(long long position) override;
		// Gets a value indicating whether the current stream supports reading.
		bool CanRead() const override;
		// Gets a value indicating whether the current stream supports seeking.
		bool CanSeek() const override;
		// Gets a value indicating whether the current stream supports writing.
		bool CanWrite() const override;

	protected:
		// Flushes reads performed on the buffer to the underlying stream.
		void FlushRead();
		// Flushes writes performed on the buffer to the underlying stream.
		void FlushWrite();

	private:
		// The underlying stream.
		IStream* _stream;
		// The buffer, shared for both read/write operations.
		Byte* _buffer;
		// The size of the buffer.
		size_t _buffer_size;
		// The read position in the buffer.
		long long _read_offset;
		// The number of bytes read into the buffer.
		long long _read_length;
		// The write position in the buffer.
		long long _write_offset;

	};

}