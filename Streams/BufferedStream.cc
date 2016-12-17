#include "BufferedStream.h"

namespace IO {

	BufferedSteam::BufferedSteam(Stream& stream) : BufferedSteam(stream, 4096) {}
	BufferedSteam::BufferedSteam(Stream& stream, size_t buffer_size) : MemoryStream(buffer_size), __stream(stream) {}

	size_t BufferedSteam::Seek(long long offset, SeekOrigin origin) {

		// Seek the underlying stream.
		__stream.Seek(offset, origin);

		// Seek self.
		return MemoryStream::Seek(offset, origin);

	}
	size_t BufferedSteam::Seek(long long position) {

		return Seek(position, SeekOrigin::Begin);

	}

}

