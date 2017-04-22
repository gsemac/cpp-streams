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

		IO::Buffer buffer(100, true);
		IO::MemoryStream ms(buffer.Address(), buffer.Size());

		IO::BitWriter bw(ms);
		IO::BitReader br(ms);

		bw.WriteString("Hello, world!");
		bw.Flush();

		ms.Seek(0);

		buffer.Shift(-5);

		std::string value;
		std::cout << "Bytes read: " << br.ReadString(value) << std::endl;
		std::cout << value;

		//size_t bytes_read;
		//do {
		//	value.clear();
		//	bytes_read = br.ReadString(value);
		//	std::cout << "\nBytes Read: " << bytes_read << std::endl;
		//	std::cout << value;
		//} while (bytes_read);


	}
	catch (Exception& ex) {

		std::cout << ex.what();

	}

	getchar();
	return 0;

}