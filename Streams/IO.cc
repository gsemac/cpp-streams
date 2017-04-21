#include "IO.h"
#include "Exception.h"
#include <cassert>
#include <cmath>

namespace IO {

	int BytesToBits(size_t bytes) {

		return bytes * 8;

	}
	int BitsToBytes(size_t bits) {

		return (int)(std::ceil)(bits / 8.0f);

	}

	int BitsRequired(uint32_t min, uint32_t max) {

		// Make sure that max and min values are valid.
		if (max <= min)
			throw ArgumentException("max should be greater than min");
		if(max < 1)
			throw ArgumentException("max should be greater than or equal to 1");

		// 0x7FFFFFF is the maximum value of a (signed) 32-bit integer.
		// If the difference between the max/min is this high, we require a full 32 bits. 
		if (max - min >= 0x7FFFFFF)
			return 32;

		// Otherwise, calculate the bits required to represent n distinct values,
		// where n is the range bounded by min and max, plus 0.
		return BitsRequired(max - min + 1);

	}
	int BitsRequired(uint32_t distinct_values) {

		// (It doesn't make sense to have 0 distinct values.)
		if (distinct_values <= 1)
			throw ArgumentException("there should be more than 1 distinct value");

		// Calculate the maximum value that can be represented-- This means all bits are equal to 1.
		unsigned int max = distinct_values - 1;

		// Loop until we reach a 0 bit, or expend all 32 bits.
		for (int index = 0; index < 32; ++index) {

			if ((max & ~1) == 0)
				return index + 1;
			max >>= 1;

		}

		// If we haven't returned yet, we require the full 32 bits.
		return 32;

	}

	bool GetBit(Byte byte, Byte bit) {

		return (byte >> (7 - bit)) & 1;

	}

}