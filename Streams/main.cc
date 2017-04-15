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
	IO::Byte byte = 0;
	
	ms.WriteByte(0);
	ms.WriteByte(0);
	ms.Seek(0);

	bw.BitSeek(2);
	bw.WriteBool(1);
	bw.BitSeek(12, IO::SeekOrigin::Current);
	std::cout << "Stream Position: " << ms.Position() << std::endl;
	bw.WriteBool(1);
	bw.WriteBool(1);
	bw.BitSeek(32);
	bw.WriteByte(255);
	bw.Flush();

	ms.Seek(0);
	while(ms.ReadByte(byte))
		std::cout << std::bitset<8>(byte) << ' ';
	std::cout << "\n";

	getchar();


	// Write some data to the memory stream.
	for (int i = 1; i <= 7; ++i)
		bw.WriteInteger(i, 0, 7);

	bw.Flush();

	// Seek the stream back to the start.
	ms.Seek(0, IO::SeekOrigin::Begin);

	// Print the resulting memory stream.
	std::cout << "MemoryStream Contents:" << std::endl;
	while (ms.ReadByte(byte))
		std::cout << std::bitset<8>(byte) << ' ';
	std::cout << std::endl;
	ms.Seek(0, IO::SeekOrigin::Begin);

	// Now lets try to read that data back out.
	std::cout << "\nRead Contents:" << std::endl;
	for (int i = 1; i <= 7; ++i) {
		int value;
		br.ReadInteger(value, 0, 7);
		std::cout << std::bitset<3>(value) << " (" << (unsigned int)value << ")" << std::endl;
	}

	// Print the read buffer.
	std::cout << "\nRead Buffer:\n";
	for (int i = 0; i < br._bytes_read; ++i) {
		std::cout << std::bitset<8>(br._buffer[i]);
	}
	std::cout << "\nRead Buffer Size: " << br._buffer.Size();
	std::cout << "\nRead Bytes: " << br._bytes_read;

	getchar();

	}
	catch (Exception ex) {
		std::cout << ex.Message();
		getchar();
	}

}