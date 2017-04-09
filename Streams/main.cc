#include "Stream.h"
#include "MemoryStream.h"
#include "BufferedStream.h"
#include "FileStream.h"
#include "StreamWriter.h"
#include "BinaryWriter.h"
#include "BinaryReader.h"
#include <iostream>
#include <fstream>
#include <string>
#include <bitset>

int main(int argv, char* argc[]) {

	IO::MemoryStream ms;
	IO::BinaryWriter sw(ms);
	IO::BinaryReader sr(ms);
	
	//std::cout << std::bitset<8>(3U) << std::endl;
	//getchar();

	sw.WriteInteger(-10, CHAR_MIN, CHAR_MAX);
	sw.Flush();

	IO::Byte byte = 0;
	ms.Seek(0);

	signed int value;
	sr.ReadInteger(value, CHAR_MIN, CHAR_MAX);

	std::cout << value;

	getchar();

	while (ms.ReadByte(byte))
		std::cout << std::bitset<8>(byte) << " ";

	byte = 0;
	ms.Seek(0);

	

	getchar();

}