#include "MemoryStream.h"

namespace IO {

	class BufferedSteam : public MemoryStream {

	public:
		BufferedSteam(Stream& stream);
		BufferedSteam(Stream& stream, size_t buffer_size);
		
		size_t Seek(long long offset, SeekOrigin origin) override;
		size_t Seek(long long position) override;

	private:
		Stream& __stream;

	};

}