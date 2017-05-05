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
	TEST_METHOD(BitsToBytesWith8Bits) {

		Assert::AreEqual(1, IO::BitsToBytes(8));

	}
	// Tests that BitsToBytes correctly rounds up to 1 byte when given less than 8 bits.
	TEST_METHOD(BitsToBytesWithLessThan8Bits) {

		Assert::AreEqual(1, IO::BitsToBytes(5));

	}
	// Tests that BitsRequired returns the correct number of bits when given a 2-bit min and 3-bit max.
	TEST_METHOD(BitsRequiredFor2BitMinAnd3BitMax) {

		Assert::AreEqual(3, IO::BitsRequired(2, 7));

	}
	};

	TEST_CLASS(BitWriterTests) {
public:
	// Tests multidirectional bit-level seeking and writing in a stream containing multiple bytes.
	TEST_METHOD(BitSeekFromAllOriginsWithMultipleBytes) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);

		// Write two blank bytes to the stream.
		// The stream now looks like this:
		// 00000000 00000000
		for (int i = 0; i < 2; ++i)
			bw.WriteByte(0);

		// Write so that the stream looks like this:
		// 00100001 10000010
		bw.SeekBits(2, IO::SeekOrigin::Begin);
		bw.WriteBool(1);
		bw.SeekBits(4, IO::SeekOrigin::Current);
		bw.WriteBool(1);
		bw.WriteBool(1);
		bw.SeekBits(-2, IO::SeekOrigin::End);
		bw.WriteBool(1);
		bw.Flush();

		// Seek back to the beginning of the stream.
		ms.Seek(0);

		// Read the bytes back and ensure that the bits have been set.
		IO::Byte bytes[2];
		for (int i = 0; i < 2; ++i)
			ms.ReadByte(bytes[i]);

		Assert::AreEqual(IO::Byte(0b00100001), bytes[0]);
		Assert::AreEqual(IO::Byte(0b10000010), bytes[1]);

	}
	};

	TEST_CLASS(BitReaderTests) {
public:
	// Tests that a BitReader can accurately read multiple bounded unsigned integers written by a BitWriter.
	TEST_METHOD(ReadMultipleBoundedUnsignedIntegers) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		for (unsigned int i = 0; i <= 5; ++i)
			bw.WriteInteger(6U, i, 7U);
		bw.Flush();

		ms.Seek(0);

		unsigned int value;
		for (unsigned int i = 0; i <= 5; ++i) {
			br.ReadInteger(value, i, 7U);
			Assert::AreEqual(6U, value);
		}

	}
	// Tests that a BitReader can accurately read the largest possible unsigned integer written by a BitWriter.
	TEST_METHOD(ReadMaximumUnsignedInteger) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteInteger(UINT_MAX);
		bw.Flush();

		ms.Seek(0);

		unsigned int value = 0;
		br.ReadInteger(value);
		Assert::AreEqual(UINT_MAX, value);

	}
	// Tests that a BitReader can accurately read multiple positively-bounded signed integers written by a BitWriter.
	TEST_METHOD(ReadMultiplePositivelyBoundedSignedIntegers) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		for (signed int i = 0; i <= 5; ++i)
			bw.WriteInteger(6, i, 7);
		bw.Flush();

		ms.Seek(0);

		signed int value;
		for (signed int i = 0; i <= 5; ++i) {
			br.ReadInteger(value, i, 7);
			Assert::AreEqual(6, value);
		}

	}
	// Tests that a BitReader can accurately read multiple negatively-bounded signed integers written by a BitWriter.
	TEST_METHOD(ReadMultipleNegativelyBoundedSignedIntegers) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		for (signed int i = 0; i <= 5; ++i)
			bw.WriteInteger(-6, -7, -i);
		bw.Flush();

		ms.Seek(0);

		signed int value;
		for (signed int i = 0; i <= 5; ++i) {
			br.ReadInteger(value, -7, -i);
			Assert::AreEqual(-6, value);
		}

	}
	// Tests that a BitReader can accurately read a single unbounded signed integer written by a BitWriter.
	TEST_METHOD(ReadUnboundedSignedInteger) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteInteger(65);
		bw.Flush();

		ms.Seek(0);

		signed int value;
		br.ReadInteger(value);
		Assert::AreEqual(65, value);

	}
	// Tests that a BitReader can accurately read the largest possible signed integer written by a BitWriter.
	TEST_METHOD(ReadMaximumSignedInteger) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteInteger(INT_MAX);
		bw.Flush();

		ms.Seek(0);

		signed int value = 0;
		br.ReadInteger(value);
		Assert::AreEqual(INT_MAX, value);

	}
	// Tests that a BitReader can accurately read the smallest possible signed integer written by a BitWriter.
	TEST_METHOD(ReadMinimumSignedInteger) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteInteger(INT_MIN);
		bw.Flush();

		ms.Seek(0);

		signed int value = 0;
		br.ReadInteger(value);
		Assert::AreEqual(INT_MIN, value);

	}
	// Tests that bit-level seeking from the beginning of a stream with a single byte is accurate.
	TEST_METHOD(BitSeekFromBeginningOfStreamWithSingleByte) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteByte(0b11001100);
		bw.Flush();

		ms.Seek(0);

		bool value;

		br.SeekBits(1);
		br.ReadBool(value);
		Assert::AreEqual(true, value);

		br.SeekBits(2);
		br.ReadBool(value);
		Assert::AreEqual(false, value);

	}
	// Tests that a BitReader can accurately read bytes written by a BitWriter that are unaligned to byte boundaries.
	TEST_METHOD(ReadMultipleUnalignedBytes) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);
		IO::Byte input[] = { 0b11111111, 0b00000000, 0b00001010 };

		bw.WriteBool(true);
		bw.WriteBytes(input, sizeof(input));
		bw.Flush();

		ms.Seek(0);

		br.SeekBits(1);

		IO::Byte output[sizeof(input)];
		br.ReadBytes(output, sizeof(output));

		for (size_t i = 0; i < sizeof(output); ++i)
			Assert::AreEqual(input[i], output[i]);

	}
	// Tests that a BitReader can accurately read a single unbounded signed char written by a BitWriter.
	TEST_METHOD(ReadUnboundedSignedChar) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteChar(65);
		bw.Flush();

		ms.Seek(0);

		signed char value;
		br.ReadChar(value);

		Assert::AreEqual((signed char)65, value);

	}
	// Tests that a BitReader can accurately read a single bounded signed char written by a BitWriter.
	TEST_METHOD(ReadBoundedSignedChar) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteChar(3, 1, 7);
		bw.Flush();

		ms.Seek(0);

		signed char value;
		br.ReadChar(value, 1, 7);

		Assert::AreEqual((signed char)3, value);

	}
	// Tests that a BitReader can accurately read a float written by a BitWriter.
	TEST_METHOD(ReadFloat) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteFloat(0.75f);
		bw.Flush();

		ms.Seek(0);

		float value;
		br.ReadFloat(value);

		Assert::AreEqual(0.75f, value);

	}
	// Tests that a BitReader can accurately read an std::string written by a BitWriter.
	TEST_METHOD(ReadString) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		std::string str = "Hello, world!";
		bw.WriteString(str);
		bw.Flush();

		ms.Seek(0);

		std::string value;
		br.ReadString(value);

		Assert::AreEqual(str, value);

	}
	// Tests that a BitReader can accurately read a c-string written by a BitWriter.
	TEST_METHOD(ReadCString) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		const char str[] = "Hello, world!";
		bw.WriteString(str);
		bw.Flush();

		ms.Seek(0);

		char value[14];
		br.ReadString(value);

		Assert::IsTrue(std::strcmp(str, value) == 0);

	}
	// Tests that reads on the read buffer are properly flushed to the underlying stream.
	TEST_METHOD(FlushReadSeeksUnderlyingStream) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteByte(1);
		bw.WriteByte(2);
		bw.WriteByte(3);
		bw.Flush();

		ms.Seek(0);

		IO::Byte value;
		br.ReadByte(value);
		br.Flush();

		Assert::AreEqual(1U, ms.Position());

	}
	// Tests that the Peek() method properly returns the next byte in the stream.
	TEST_METHOD(PeekReturnsNextByte) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteByte(1);
		bw.WriteByte(2);
		bw.Flush();

		ms.Seek(1);

		IO::Byte value;
		value = br.Peek();

		Assert::AreEqual((IO::Byte)2, value);

	}
	// Tests that the Peek() method does not advance the read position.
	TEST_METHOD(PeekDoesNotAdvanceReadPosition) {

		IO::MemoryStream ms;
		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteByte(1);
		bw.WriteByte(2);
		bw.Flush();

		ms.Seek(0);		

		br.Peek();

		// If peeking didn't advance the read position, we should read the first byte.
		IO::Byte value;
		br.ReadByte(value);

		Assert::AreEqual((IO::Byte)1, value);

	}

	};
	
}