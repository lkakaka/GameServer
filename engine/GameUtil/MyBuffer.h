#pragma once
#include <vector>
#include <string>
#include "../Common/ServerExports.h"

class MyBuffer
{
private:
	std::vector<unsigned char> buffer;  // must be unsigned char
public:

	MyBuffer();
	MyBuffer(const char* data, int len);

	inline int size() { return buffer.size(); }
	inline unsigned char* data() { return buffer.data(); }
	inline void remove(int len) { buffer.erase(buffer.begin(), buffer.begin() + len); }

	int readInt();
	unsigned char readByte();

	int getInt();
	unsigned char getByte();
	
	void writeInt(int val);
	void writeByte(unsigned char val);
	void writeString(const char* dat, int len);
	void append(std::vector<char>& dat);
	void append(std::vector<char>& dat, int len);

	void push_back(char val) { buffer.push_back(val); }
};

