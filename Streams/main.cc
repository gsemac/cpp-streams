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

	IO::MemoryStream ms;
	IO::BitWriter bw(ms);
	IO::BitReader br(ms);

	//for (signed int i = 0; i <= 5; ++i)
	//	bw.WriteInteger(-6, -7, -i);
	//bw.WriteInteger(INT_MAX);
	bw.WriteInteger(5, 1, 7);
	bw.Flush();

	std::cout << "Write OK\n";

	ms.Seek(0);

	signed int value = 0;
	br.ReadInteger(value, -7, -5);

	std::cout << "Read: " << value;
	
	}
	catch (Exception& ex) {

		std::cout << ex.what();
		
	}

	getchar();
	return 0;

}