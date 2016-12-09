#pragma once
#include "Stream.h"

namespace IO {

	Stream& Stream::operator << (Byte byte) {

		WriteByte(byte);

		return *this;

	}
	Stream& Stream::operator >> (Byte& byte) {

		ReadByte(byte);

		return *this;

	}

}