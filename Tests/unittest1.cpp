#include "stdafx.h"
#include "CppUnitTest.h"
#include "IO.h"
using namespace Microsoft::VisualStudio::CppUnitTestFramework;

namespace Tests {

	TEST_CLASS(UnitTest1) {

public:

	TEST_METHOD(TestMethod1) {
		// TODO: Your test code here

		

		Assert::AreEqual(2, IO::BitsToBytes(16));
	}


	};
}