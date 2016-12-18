#include "Stream.h"
#include "MemoryStream.h"
#include "BufferedStream.h"
#include "FileStream.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argv, char* argc[]) {

	// Initialize streams.
	IO::MemoryStream ms(1024);
	IO::BufferedSteam bs(ms);

	const char* data_1 = "Hello, World!";

	bs.Write(data_1, 0, strlen(data_1));

	bs.Flush();

	std::cout << ms.Position() << std::endl;
	std::cout << bs.Position() << std::endl;
	std::cout << ms.Length() << std::endl;
	std::cout << bs.Length() << std::endl;

	getchar();

}