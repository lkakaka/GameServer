#include "MyBuffer.h"
#include <stdexcept>

#define THROW_EXCEPTION(msg) {std::logic_error ex(msg); throw std::exception(ex);}


MyBuffer::MyBuffer() {

}

MyBuffer::MyBuffer(const char* data, int len) {
	std::copy(data, data + len, std::back_inserter(buffer));
}


int MyBuffer::readInt(bool isRemove) {
	if (buffer.size() < 4) THROW_EXCEPTION("mybuffer read int error");
	int c1 = buffer[0];
	int c2 = buffer[1];
	int c3 = buffer[2];
	int c4 = buffer[3];
	if (isRemove) buffer.erase(buffer.begin(), buffer.begin() + 4);
	return c1 << 24 | c2 << 16 | c3 << 8 | c4;
}

void MyBuffer::writeInt(int val) {
	for (int i = 0; i < 4; i++) {
		unsigned char ch = (unsigned char)(val >> (24 - 8 * i));
		buffer.push_back(ch);
	}
}

void MyBuffer::writeByte(unsigned char val) {
	buffer.push_back(val);
}

unsigned char MyBuffer::readByte(bool isRemove) {
	if (buffer.size() < 1) THROW_EXCEPTION("mybuffer read int error");
	unsigned char val = (unsigned char)buffer[0];
	if (isRemove) buffer.erase(buffer.begin(), buffer.begin() + 1);
	return val;
}

void MyBuffer::writeString(const char* dat, int len) {
	std::copy(dat, dat + len, std::back_inserter(buffer));
}

void MyBuffer::append(std::vector<char>& dat) {
	std::copy(dat.begin(), dat.end(), std::back_inserter(buffer));
}

void MyBuffer::append(std::vector<char>& dat, int len) {
	std::copy(dat.begin(), dat.begin() + len, std::back_inserter(buffer));
}
