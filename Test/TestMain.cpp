#include <stdio.h>
#include <iostream>
#include <vector>

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


int main() {
	
	/*std::vector<A> v;
	v.emplace_back(1);*/
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