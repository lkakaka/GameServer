#include "MyBuffer.h"
#include "../Common/ServerMacros.h"
#include <iterator>
//#include <stdexcept>
//
//#define THROW_EXCEPTION(msg) {std::logic_error ex(msg); throw std::exception(ex);}

#define REMOVE_DATA_SIZE 256


MyBuffer::MyBuffer() : cur_pos(0) {

}

MyBuffer::MyBuffer(const char* data, int len) : cur_pos(0) {
	std::copy(data, data + len, std::back_inserter(buffer));
}

unsigned char* MyBuffer::getCurrentData() {
	return &(buffer[cur_pos]);
}

void MyBuffer::moveCurrentPos(int offset) {
	cur_pos += offset;
	if (cur_pos > REMOVE_DATA_SIZE) {
		buffer.erase(buffer.begin(), buffer.begin() + cur_pos);
		cur_pos = 0;
	}
}

unsigned char MyBuffer::getByte(int offset) {
	if (buffer.size() < cur_pos + offset + 1) THROW_EXCEPTION("mybuffer read int error");
	unsigned char val = (unsigned char)buffer[cur_pos];
	return val;
}

unsigned char MyBuffer::readByte() {
	unsigned char val = getByte();
	moveCurrentPos(1);
	return val;
}

short MyBuffer::getShort(int offset) {
	if (buffer.size() < cur_pos + offset + 2) THROW_EXCEPTION("mybuffer get short error");
	unsigned char* data = getCurrentData();
	short c1 = data[offset];
	short c2 = data[offset + 1];
	return c1 << 8 | c2;
}

short MyBuffer::readShort() {
	short val = getShort();
	moveCurrentPos(2);
	return val;
}

int MyBuffer::getInt(int offset) {
	if (buffer.size() < cur_pos + offset + 4) THROW_EXCEPTION("mybuffer get int error");
	unsigned char* data = getCurrentData();
	int val = 0;
	for (int i = 0; i < 4; i++) {
		val |= data[i + offset] << (3 - i) * 8;
	}
	return val;
}

int MyBuffer::readInt() {
	int val = getInt();
	moveCurrentPos(4);
	return val;
}

long long MyBuffer::getLong(int offset) {
	if (buffer.size() < cur_pos + offset + 8) THROW_EXCEPTION("mybuffer get long error");
	unsigned char* data = getCurrentData();
	long long val = 0;
	for (int i = 0; i < 8; i++) {
		val |= ((long long)data[i + offset]) << (7 - i) * 8;
	}
	return val;
}

long long MyBuffer::readLong() {
	long long val = getLong();
	moveCurrentPos(8);
	return val;
}

void MyBuffer::writeByte(unsigned char val) {
	buffer.push_back(val);
}

void MyBuffer::writeShort(short val) {
	for (int i = 0; i < 2; i++) {
		unsigned char ch = (unsigned char)(val >> (8 - 8 * i));
		buffer.push_back(ch);
	}
}

void MyBuffer::writeInt(int val) {
	for (int i = 0; i < 4; i++) {
		unsigned char ch = (unsigned char)(val >> (24 - 8 * i));
		buffer.push_back(ch);
	}
}

void MyBuffer::changeInt(int offset, int val) {
	for (int i = 0; i < 4; i++) {
		unsigned char ch = (unsigned char)(val >> (24 - 8 * i));
		buffer[offset + i] = ch;
	}
}

void MyBuffer::writeLong(long long val) {
	for (int i = 0; i < 8; i++) {
		unsigned char ch = (unsigned char)(val >> (56 - 8 * i));
		buffer.push_back(ch);
	}
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

void MyBuffer::remove(int len) {
	buffer.erase(buffer.begin(), buffer.begin() + cur_pos + len);
	cur_pos = 0;
}
