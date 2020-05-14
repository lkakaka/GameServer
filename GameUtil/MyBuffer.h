#pragma once
#include <vector>
#include <string>
#include "../Common/ServerExports.h"

class SERVER_EXPORT_API MyBuffer
{
private:
	std::vector<char> buffer;
public:

	MyBuffer();
	MyBuffer(const char* data, int len);

	inline int size() { return buffer.size(); }
	inline char* data() { return buffer.data(); }
	inline void remove(int len) { buffer.erase(buffer.begin(), buffer.begin() + len); }

	int readInt(bool isRemove);
	unsigned char readByte(bool isRemove);
	
	void writeInt(int val);
	void writeByte(unsigned char val);
	void writeString(const char* dat, int len);
	void append(std::vector<char>& dat);
	void append(std::vector<char>& dat, int len);

	void push_back(char val) { buffer.push_back(val); }
};

