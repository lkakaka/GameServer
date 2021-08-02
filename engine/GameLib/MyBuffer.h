#pragma once
#include <vector>
#include <string>
#include "../Common/ServerExports.h"

// 非线程安全

class MyBuffer
{
private:
	std::vector<unsigned char> buffer;  // must be unsigned char
	int cur_pos; // current read pos
private:
	unsigned char* getCurrentData();
	void moveCurrentPos(int offset);
public:

	MyBuffer();
	MyBuffer(const char* data, int len);

	inline size_t size() { return buffer.size() - cur_pos; }
	inline unsigned char* data() { return buffer.data() + cur_pos; }
	void remove(int len);

	unsigned char readByte();
	short readShort();
	int readInt();
	long long readLong();
	
	unsigned char getByte(int offset = 0);
	int getInt(int offset=0);
	short getShort(int offset=0);
	long long getLong(int offset = 0);
	
	void writeByte(unsigned char val);
	void writeShort(short val);
	void writeInt(int val);
	void writeLong(long long val);
	
	void writeString(const char* dat, int len);
	void append(std::vector<char>& dat);
	void append(std::vector<char>& dat, int len);

	void push_back(char val) { buffer.push_back(val); }
};

