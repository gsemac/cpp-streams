#include "FileStream.h"
#include <cassert>
#include <algorithm>
#include <sys/stat.h>

namespace IO {

	FileStream::FileStream(const char* path, FileMode mode) : FileStream(path, mode, FileAccess::ReadWrite) {}
	FileStream::FileStream(const char* path, FileMode mode, FileAccess access) {

		// Initialize member variables.
		__position = 0;
		__path = path;
		__last_read = false;

		// Initialize flags.
		InitFlags(mode, access);

		// Open the file.
		__stream.open(path, __flags);
	
		// If opened in "Append" mode, seek to the end of the file.
		if (__flags & std::fstream::app) {
			__stream.seekg(0, std::ios_base::end);
			__position = __stream.tellg();
		}

	}
	FileStream::~FileStream() {

		Close();

	}

	size_t FileStream::Length() {

		// If the stream is in "Append" mode, just return the current position (guaranteed to be at the end of the stream).
		if (__flags & std::fstream::app)
			return __position;

		// If the stream does not support reading, throw an error.
		if (!CanRead())
			throw NotSupportedException();

		// Preserve the current seek position.
		std::streampos pos = __stream.tellg();

		// Seek to the end of the stream, and record the size.
		__stream.seekg(0, std::ios::end);
		std::streampos size = __stream.tellg();

		// Seek back to the previous position.
		__stream.seekg(pos, std::ios::beg);

		// Return the file size.
		return size < 0 ? 0 : (size_t)size;

	}
	size_t FileStream::Position() const {

		return __position;

	}
	void FileStream::Flush() {

		__stream.flush();

	}
	void FileStream::SetLength(size_t length) {

		// Throw error if the stream does not support writing or seeking.
		if (!CanWrite() || !CanSeek())
			throw NotSupportedException();

		// Get the current length.
		size_t clength = Length();

		if (length < clength) {

			// Create a buffer, and copy into it the first length bytes of the file.
			Byte* buf = new Byte[length];
			size_t prev_pos = __position;
			Seek(0, IO::SeekOrigin::Begin);
			Read(buf, 0, length);

			// Close the file stream.
			__stream.flush();
			__stream.close();

			// Create a new file stream with the truncation flag.
			__stream.open(__path, std::fstream::out | std::fstream::trunc);

			// Write the buffer back to the file, and free it.
			__position = 0;
			Write(buf, 0, length);
			delete[] buf;

			// Close the file.
			__stream.flush();
			__stream.close();

			// Re-open the file with previous flags, and seek the former or truncated position.
			__stream.open(__path, __flags);
			__last_read = false;
			__stream.seekg((std::min)(prev_pos, length));

		}
		else if (length > clength) {

			// Seek to the desired length, write a null byte, and then seek back.
			size_t prev_pos = __position;
			__stream.seekg(-1 + length);
			__stream.write("\0", 1);
			__stream.seekg(__position);

		}

	}
	bool FileStream::ReadByte(Byte& byte) {

		// Throw an exception of the stream is not readable.
		if (!CanRead())
			throw NotSupportedException();

		// If the last action was not a read, seek to refresh the stream.
		if (!__last_read) __stream.seekg(__position);
		__last_read = true;

		// Attempt to read a byte from the Stream.
		char b;
		__stream.read(&b, 1);

		// If we've reached the end of the Stream, a valid character was not read.
		if (__stream.eof() || b == -1)
			return false;

		// Set the value read, and increment the seek position.
		byte = b;
		++__position;

		// Return true, since a byte was successfully read.
		return true;

	}
	void FileStream::WriteByte(Byte byte) {

		// Throw an exception of the stream is not writeable.
		if (!CanWrite())
			throw NotSupportedException();

		// If the last action was not a write, seek to refresh the stream.
		if (__last_read) __stream.seekg(__position);
		__last_read = false;

		__stream.write((const char*)&byte, 1);
		++__position;

	}
	size_t FileStream::Read(void* buffer, size_t offset, size_t length) {

		// Throw an exception of the stream is not readable.
		if (!CanRead())
			throw NotSupportedException();

		// If the last action was not a read, seek to refresh the stream.
		if (!__last_read) __stream.seekg(__position);
		__last_read = true;

		// Write bytes from the stream to the buffer.
		__stream.read((char*)buffer + offset * sizeof(Byte), length);

		// Update the seek position.
		size_t bytes_read = __stream.gcount();
		__position += bytes_read;

		return bytes_read;

	}
	void FileStream::Write(const void* buffer, size_t offset, size_t length) {

		// Throw an exception of the stream is not writeable.
		if (!CanWrite())
			throw NotSupportedException();

		// If the last action was not a write, seek to refresh the stream.
		if (__last_read) __stream.seekg(__position);
		__last_read = false;

		// Write bytes to the stream from the buffer.
		__stream.write((const char*)buffer, length);

		// Update the sek position.
		__position += length;

	}
	void FileStream::Close() {

		// Flush and close the stream if it is open.
		if (__stream.is_open()) {
			__stream.flush();
			__stream.close();
		}

		// Reset the seek position and flags.
		__position = 0;
		__flags = 0;
		__access_flags = 0;

	}
	size_t FileStream::Seek(long long offset, SeekOrigin origin) {

		// Throw an exception of the stream is not seekable.
		if (!CanSeek()) throw
			IO::IOException();

		// For fstream, seekg and seekp refer to the same pointer, and do not need to be considered separately.

		// Store the current stream position (to compute difference later).
		std::streampos pos = __stream.tellg();

		// Determine the seek origin.
		std::ios_base::seekdir seek_origin = std::ios_base::beg;
		switch (origin) {
		case SeekOrigin::Current:
			seek_origin = std::ios_base::cur;
			break;
		case SeekOrigin::End:
			seek_origin = std::ios_base::end;
			break;
		}

		// Move the seek pointer.
		__stream.seekg(offset, seek_origin);

		// Adjust the seek position according to the difference.
		__position += (__stream.tellg() - pos);

		// Return the new position.
		return __position;

	}
	size_t FileStream::Seek(long long position) {

		return Seek(position, SeekOrigin::Begin);

	}
	bool FileStream::CanRead() const {

		return (__access_flags & std::fstream::in) && !(__flags & std::fstream::app);

	}
	bool FileStream::CanSeek() const {

		return !(__flags & std::fstream::app);

	}
	bool FileStream::CanWrite() const {

		return __access_flags & std::fstream::out;

	}

	bool FileStream::FileExists(const char* path) {

		struct stat buf;
		return (stat(path, &buf) != -1);

	}
	void FileStream::InitFlags(FileMode mode, FileAccess access) {

		// Initialize variables.
		__flags = std::fstream::binary;
		__access_flags = 0;

		// Set access flags.
		switch (access) {
		case FileAccess::Read:
			__access_flags |= std::fstream::in;
			break;
		case FileAccess::ReadWrite:
			__access_flags |= std::fstream::in;
		case FileAccess::Write:
			__access_flags |= std::fstream::out;
			break;
		}

		// Set mode flags.
		switch (mode) {

		case FileMode::Append:
			__flags |= std::fstream::app | std::fstream::out;
			break;

		case FileMode::Open:
			if (!FileExists(__path.c_str()))
				throw FileNotFoundException();
			__flags |= __access_flags;
			break;

		case FileMode::OpenOrCreate:
			if (FileExists(__path.c_str()))
				__flags |= __access_flags;
			else
				__flags |= std::fstream::out;
			break;

		case FileMode::CreateNew:
			if (FileExists(__path.c_str()))
				throw IO::IOException();

		case FileMode::Create:

		case FileMode::Truncate:
			__flags |= std::fstream::trunc | __access_flags;
			break;

		}

	}

}