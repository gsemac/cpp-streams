#include "Stream.h"
#include "MemoryStream.h"
#include "BufferedStream.h"
#include "FileStream.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argv, char* argc[]) {

	// Initialize streams.
	IO::MemoryStream ms1;
	IO::MemoryStream ms2;
	IO::BufferedSteam bs(ms1);
	
	ms1.Seek(99);

	std::cout << ms1.Length() << std::endl;
	std::cout << ms1.Position() << std::endl;
	
	IO::Byte byte = 65;
	ms1.WriteByte(byte);
	ms1.Seek(0);
	ms1.CopyTo(ms2, 5);

	std::cout << "Length of MemoryStream   #2: " << ms2.Length() << std::endl;
	std::cout << "Position in MemoryStream #1: " << ms1.Position() << std::endl;
	std::cout << "Capacity of MemoryStream #1: " << ms1.Capacity() << std::endl;

	getchar();
	
	//// Write some data to the stream.
	////bs.WriteByte('A');
	////bs.Flush();
	//bs.Seek(25, IO::SeekOrigin::Begin);
	//std::cout << "BufferedStream is at: " << bs.Position() << std::endl;
	//std::cout << "MemoryStream is at: " << ms.Position() << std::endl;

	//// Read out the contents of the memory stream.
	//IO::Byte b;
	//ms.Seek(0, IO::SeekOrigin::Begin);
	//while (ms.ReadByte(b))
	//	std::cout << (char)b;

	getchar();

}