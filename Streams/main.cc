#include "Stream.h"
#include "MemoryStream.h"
#include <iostream>

int main(int argv, char* argc[]) {

	IO::MemoryStream ms;
	ms << 255 << 254 << 253;
	ms.Seek(0, IO::SeekOrigin::Begin);

	IO::Byte b;
	while (ms.ReadByte(b))
		std::cout << (int)b << std::endl;

	getchar();

}