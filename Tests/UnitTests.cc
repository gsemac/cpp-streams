#include "CppUnitTest.h"
#include "IO.h"
#include "BitReader.h"
#include "BitWriter.h"
#include "MemoryStream.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {

	TEST_CLASS(IOTests) {
public:
	// Tests that BitsToBytes correctly returns 1 byte when given 8 bits.
	TEST_METHOD(BitsToBytesOneByte) {

		Assert::AreEqual(1, IO::BitsToBytes(8));

	}
	// Tests that BitsToBytes correctly rounds up to 1 byte when given less than 8 bits.
	TEST_METHOD(BitsToBytesPartialByte) {

		Assert::AreEqual(1, IO::BitsToBytes(5));

	}
	// Tests that BitsRequired returns the correct number of bits when given a 2-bit min and 3-bit max.
	TEST_METHOD(BitsRequired2Bit3Bit) {

		Assert::AreEqual(3, IO::BitsRequired(2, 7));

	}
	};

	TEST_CLASS(BitWriterTests) {
public:
	// Tests seeking to specific bits and writing at those positions.
	TEST_METHOD(BitSeek) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);

		// Write four blank bytes to the stream.
		for (int i = 0; i < 4; ++i)
			bw.WriteByte(0);

		// Seek to the 8th bit and write a 1.
		bw.BitSeek(7, IO::SeekOrigin::Begin);
		bw.WriteBool(1);
		bw.BitSeek(-1, IO::SeekOrigin::Current);
		bw.Flush();

		// Seek back to the beginning of the stream.
		ms.Seek(0);

		// Read that byte back and check that the bit has been set.
		IO::Byte value;
		ms.ReadByte(value);

		Assert::AreEqual(IO::Byte(3), value);

	}
	};

	TEST_CLASS(BitReaderTests) {
public:
	// Tests that a BitReader can accurately read a 3-bit unsigned integer written by a BitWriter.
	TEST_METHOD(Read3BitUInteger) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteInteger(5U, 2U, 7U);
		bw.Flush();

		ms.Seek(0);

		unsigned int value;
		br.ReadInteger(value, 2U, 7U);

		Assert::AreEqual(5U, value);

	}
	// Tests that a BitReader can accurately read bytes written by a BitWriter that are unaligned to byte boundaries.
	TEST_METHOD(ReadBytesUnaligned) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);
		IO::Byte input[] = { 0xFF, 0x00, 0x0A };

		bw.WriteBool(true);
		bw.WriteBytes(input, sizeof(input));
		bw.Flush();

		ms.Seek(0);

		IO::Byte output[sizeof(input)];
		br.ReadBytes(output, sizeof(output));

		for (size_t i = 0; i < sizeof(output); ++i)
			Assert::AreEqual(input[i], output[i]);

	}
	};

}