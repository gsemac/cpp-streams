#pragma warning(disable: 4514 4710 4711)
#include "Stream.h"
#include "MemoryStream.h"
#include "BufferedStream.h"
#include "FileStream.h"
#include "StreamWriter.h"
#include "BitWriter.h"
#include "BitReader.h"
#include "Buffer.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

int main(int argv, char* argc[]) {

	try {

		IO::MemoryStream ms(10);

		IO::BitReader br(ms);

		// Attempt to read something so that the read buffer is filled.
		bool value;
		br.ReadBool(value);

		std::cout << ms.Position() << std::endl;

		// Flush reads to the stream. Since we have read a single bit, the stream should be seeked one byte forward.
		br.Flush();

		std::cout << ms.Position() << std::endl;

	}
	catch (Exception& ex) {

		std::cout << ex.what();

	}

	getchar();
	return 0;

}