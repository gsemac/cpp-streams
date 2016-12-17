#pragma once
#include "Stream.h"
#include "Exception.h"
#include <fstream>

namespace IO {

	// Specifies how the operating system should open a file.
	enum class FileMode {
		// Opens the file if it exists and seeks to the end of the file, or creates a new file.
		Append,
		// Specifies that the operating system should create a new file. If the file already exists, it will be overwritten.
		Create,
		// Specifies that the operating system should create a new file. 
		CreateNew,
		// Specifies that the operating system should open an existing file.
		Open,
		// Specifies that the operating system should open a file if it exists; otherwise, a new file should be created.
		OpenOrCreate,
		// Specifies that the operating system should open an existing file. When the file is opened, it should be truncated so that its size is zero bytes.
		Truncate
	};

	// Defines constants for read, write, or read/write access to a file.
	enum class FileAccess {
		// Read access to the file. Data can be read from the file.
		Read,
		// Read and write access to the file. Data can be written to and read from the file.
		ReadWrite,
		// Write access to the file. Data can be written to the file. 
		Write
	};

	// Provides a Stream for a file.
	class FileStream : public Stream {

	public:
		FileStream(const char* path, FileMode mode);
		FileStream(const char* path, FileMode mode, FileAccess access);
		~FileStream();

		size_t Length() override;
		size_t Position() const override;
		virtual void Flush() override;
		virtual void SetLength(size_t length) override;
		virtual bool ReadByte(Byte& byte) override;
		virtual void WriteByte(Byte byte) override;
		virtual size_t Read(void* buffer, size_t offset, size_t length) override;
		virtual void Write(const void* buffer, size_t offset, size_t length) override;
		virtual void Close() override;
		virtual void Seek(long offset, SeekOrigin origin) override;
		virtual bool CanRead() const override;
		virtual bool CanSeek() const override;
		virtual bool CanWrite() const override;

	protected:
		std::fstream __stream;
		std::string __path;
		std::size_t __position;
		unsigned int __flags;
		unsigned int __access_flags;
		bool __last_read;

	private:
		bool FileExists(const char* path);
		void InitFlags(FileMode mode, FileAccess access);

	};

}