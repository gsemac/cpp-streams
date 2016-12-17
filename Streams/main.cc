#include "Stream.h"
#include "MemoryStream.h"
#include "FileStream.h"
#include <iostream>
#include <fstream>
#include <string>

int main(int argv, char* argc[]) {

	IO::FileStream fs("TestFile.txt", IO::FileMode::Open);

	fs.SetLength(5);
	fs.Seek(0, IO::SeekOrigin::End);
	fs.WriteByte(66);

	//fs.Write("Hello, World!", 0, 13);

	fs.Close();

}