#include "Buffer.h"
#include <cstdlib>
#include <cstring>
#include <algorithm>

namespace IO {

	Buffer::Buffer(size_t bytes, bool zero) {

		_size = bytes;

		if (bytes)
			if (zero)
				_buffer = (Byte*)calloc(bytes, sizeof(Byte));
			else
				_buffer = (Byte*)malloc(bytes);
		else
			_buffer = nullptr;

	}
	Buffer::~Buffer() {

		if (_size > 0)
			delete _buffer;

		_buffer = nullptr;
		_size = 0;

	}

	void Buffer::Shift(int amount) {

		// We'll shift without wrapping. Memory "pulled in" from the edges will be zeroed-out.

		if ((std::abs)(amount) >= _size)

			// If the shift amount is greater than the size of the buffer, it's easier just to clear it.
			Clear();

		else if (amount > 0) {

			// Shift right.
			Copy(*this, amount, 0, _size - amount);

			// Zero-out the "new" memory.
			Clear(0, amount);

		}
		else if (amount < 0) {

			// Shift left.
			Copy(*this, 0, -amount, _size + amount);

			// Zero-out the "new" memory.
			Clear(_size + amount, -amount);

		}

	}
	void Buffer::Rotate(int amount) {

		if (amount < 0)
			amount += _size;
		amount %= _size;

		Reverse(0, _size - amount);
		Reverse(_size - amount, amount);
		Reverse(0, _size);

	}
	void Buffer::Fill(Byte value) {

		Fill(0, Size(), value);

	}
	void Buffer::Fill(size_t index, size_t length, Byte value) {

		if (_buffer)
			memset(_buffer + index, value, length);

	}
	void Buffer::Copy(Buffer& buffer, size_t offset, size_t index, size_t length) {

		if (buffer._buffer == _buffer)
			memmove(buffer._buffer + offset, _buffer + index, length);
		else
			memcpy(buffer._buffer + offset, _buffer + index, length);

	}
	void Buffer::Clear() {

		Clear(0, _size);

	}
	void Buffer::Clear(size_t index, size_t length) {

		if (_buffer)
			memset(_buffer + index, 0, length);

	}
	void Buffer::Reverse() {

		Reverse(0, Size());

	}
	void Buffer::Reverse(size_t index, size_t length) {

		if (!_buffer || length <= 0)
			return;

		for (size_t i = index, j = index + length - 1; i < j; ++i, --j) 
			std::swap(*(_buffer + i), *(_buffer + j));

	}
	void Buffer::Resize(size_t bytes, bool zero) {

		// Create a new buffer, and zero-out the memory if it's larger than the current buffer.
		Buffer newBuffer(bytes, zero);

		// Copy the contents of this buffer to the new one.
		Copy(newBuffer, 0, 0, (std::min)(_size, bytes));

		// Swap our old memory address with that of the old buffer so it frees the old memory.
		std::swap(_buffer, newBuffer._buffer);
		
		// Update this buffer's size.
		_size = bytes;

	}
	size_t Buffer::Size() const {

		return _size;

	}
	Buffer::Byte* Buffer::Address() const {

		return _buffer;

	}

	Buffer::Buffer::Byte& Buffer::operator[](size_t index) {

		return *(_buffer + index);

	}

}