#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>

class A {
private:
	
public:
	int m_val;
	A(int val) : m_val(val) { printf("A constructor,%d\n", m_val); }
	A(const A& a) : m_val(a.m_val){ printf("A assign constructor,%d\n", m_val); }
	/*A& operator=(const A& a) { 
		m_val = a.m_val;
		printf("A = operator,%d\n", m_val);
		return *this;
	}*/
};


void test_func(std::vector<A> v) {
	printf("test func\n");
	std::vector<A> v1;
	v1 = v;
}


std::vector<std::string> split(char* str, const char* delimiters) {
	std::vector<std::string> vec;
	char* strc = new char[strlen(str) + 1];
	strcpy(strc, str);
	char* pch = strtok(strc, delimiters);
	while (pch != NULL)
	{
		printf("%s\n", pch);
		vec.push_back(std::string(pch));
		pch = strtok(NULL, delimiters);
	}
	delete[] strc;
	return vec;
}


int main() {

	std::vector<std::string> v = split("101.1.0", ".");
	int serverId = atoi(v[0].c_str());
	int serviceType = atoi(v[1].c_str());
	int serviceNo = atoi(v[2].c_str());

	/*std::vector<A> v;
	v.emplace_back(1);*/

	int i = 2;
	int j = i > 1 ? 0 : 1;

	unsigned char x = 128;
	int y = x;
	int z = y << 24;

	A a(1);
	A b(0);
	b = a;
	printf("val=%d\n", b.m_val);
	//test_func(v);

	//std::vector<A> v1 = v;

	return 1;
}
