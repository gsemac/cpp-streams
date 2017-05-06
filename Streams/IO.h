#pragma once
#include <stdint.h>

namespace IO {

	typedef uint8_t Byte;

	// Returns the number of bits corresponding to the given number of bytes.
	int BytesToBits(size_t bytes);
	// Returns the number of bytes corresponding to the given number of bits.
	int BitsToBytes(size_t bits);

	// Returns the number of bytes required to store an integer between the given minimum and maximum values.
	int BitsRequired(uint32_t min, uint32_t max);
	// Returns the number of bytes required to store the given number of distinct values.
	int BitsRequired(uint32_t distinct_values);

	// Returns the value of the nth bit from the given byte, where 0 is the most-significant bit and 7 is the least-significant bit.
	bool GetBit(Byte byte, Byte bit);
	// Sets the nth bit of the given byte to the given value, where 0 is the most-significant bit and 7 is the least-significant bit.
	void SetBit(Byte& byte, Byte bit, bool value);

}