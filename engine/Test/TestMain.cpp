#include <stdio.h>
#include <iostream>
#include <vector>
#include <string.h>
#include <map>
#include <memory>
#include "Test.h"
#include "Logger.h"

#define _TEST_MAIN


class Base {
public:
	virtual void test() { printf("Base::test\n");  };
	long long a;
	/*virtual void test1() {};
	virtual void test2() {};*/
	virtual ~Base() { printf("~Base\n"); };
};

class Base1 {
public:
	int a;
	void test1() { printf("Base1::test1\n"); };
	/*virtual void test() {};
	virtual void test1() {};
	virtual void test2() {};*/
};

class Drive : public Base {
public:
	~Drive() { printf("~Drive\n"); }
	void test() { printf("Drive::test\n"); };
	void test1() { printf("Drive::test1\n"); };
};

class Drive1 : public Drive {
public:
	~Drive1(){ printf("~Drive1\n"); }
};

class A {
private:
	
public:
	std::shared_ptr<Base> b;
	int m_val;
	A(int val) : m_val(val) { printf("A constructor,%d\n", m_val); }
	A(const A& a) : m_val(a.m_val){ printf("A assign constructor,%d\n", m_val); }
	/*A& operator=(const A& a) { 
		m_val = a.m_val;
		printf("A = operator,%d\n", m_val);
		return *this;
	}*/

	~A() { 
		printf("~A\n"); 
	}
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



class Blob {
public:
	Blob()
		: data_(nullptr), size_(0) {
		log("Blob's default constructor");
	}

	explicit Blob(size_t size)
		: data_(new char[size]), size_(size) {
		log("Blob's parameter constructor");
	}

	~Blob() {
		log("Blob's destructor");
		if (data_ != nullptr) delete[] data_;
	}

	Blob(const Blob& other) {
		log("Blob's copy constructor");
		data_ = new char[other.size_];
		memcpy(data_, other.data_, other.size_);
		size_ = other.size_;
	}

	Blob& operator=(const Blob& other) {
		log("Blob's copy assignment operator");
		if (this == &other) {
			return *this;
		}
		delete[] data_;
		data_ = new char[other.size_];
		memcpy(data_, other.data_, other.size_);
		size_ = other.size_;
		return *this;
	}

	/*Blob(Blob&& other) {
		log("Blob's move constructor");
		data_ = new char[other.size_];
		std::swap(data_, other.data_);
		std::swap(size_, other.size_);
	}*/

	/*Blob& operator=(Blob&& other) {
		log("Blob's move assignment operator");
		if (this == &other) {
			return *this;
		} 
		std::swap(data_, other.data_);
		std::swap(size_, other.size_);
	}*/

	/*void set(size_t offset, size_t len, const void* src) {
		len = std::min(len, size_ - offset);
		memcpy(data_ + offset, src, len);
	}*/

private:
	char* data_;
	size_t size_;

	void log(const char* msg) {
		std::cout << "[" << this << "] " << msg << std::endl;
	}
};


Blob createBlob(const char* str) {
	size_t len = strlen(str);
	Blob blob(len);
	//blob.set(0, len, str);
	return blob;
}

class Empty {
public:
	int a;
	std::vector<int> b;
};

struct _S {
public:
    std::string data;
};

#ifdef _TEST_MAIN

#define print_error(fmt, ...)	{	\
		printf("%s:%d : "#fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__); \
		LOG_ERROR("%s:%d : "#fmt"\n", __FILE__, __LINE__, ##__VA_ARGS__);	\
		}
int main() {

	Logger::initLog("test");

	Drive d;
	Base* b = &d;
	b->test();

	/*std::map<std::string, std::vector<std::string>> m;
	auto pair = m.try_emplace("a", std::vector<std::string>());
	pair.first->second.push_back("aa");*/

	/*std::shared_ptr<Base> p(new Base());
	A* a = new A(1);
	a->b = p;
	p.reset();
	delete a;*/

	/*int bs = sizeof(Base);
	int ds = sizeof(Drive);
	Base b;
	Drive d;
	int o1 = sizeof(b);
	int o2 = sizeof(d);
	
	Base* p = new Drive();
	(*p).test();


	std::vector<std::string> v = split("101.1.0", ".");
	int serverId = atoi(v[0].c_str());
	int serviceType = atoi(v[1].c_str());
	int serviceNo = atoi(v[2].c_str());


	int size = sizeof(long long);*/


	/*std::vector<A> v;
	v.emplace_back(1);*/

	//int i = 2;
	//int j = i > 1 ? 0 : 1;

	//unsigned char x = 128;
	//int y = x;
	//int z = y << 24;

	//A a(1);
	//A b(0);
	//b = a;
	//printf("val=%d\n", b.m_val);
	////test_func(v);

	////std::vector<A> v1 = v;

	/*Blob blob;

	std::cout << "Start assigning value..." << std::endl;
	blob = createBlob("A very very very long string representing serialized data");
	std::cout << "End assigning value" << std::endl;*/

	/*Drive1* d1 = new Drive1();
	delete d1;*/

	//testBoostMultiIndex();
	//testOperateNew();

	//testSOL();

	LOG_DEBUG("test");
	print_error("test,%d", 1);

	Empty e;
	printf("ptr1=%I64d\n", &e.b);
	e.b.assign(1000, 1);
	printf("ptr2=%I64d\n", &e.b);
	printf("size=%d", sizeof(e.b));
    
    printf("\n\n");
    _S* p = new _S();
    p->data = "aa";
    printf("data=%s\n", p->data.c_str());

	testBindFunction();

#ifndef WIN32
	test_signal();
#endif // WIN32
	
	//curl_multi_demo();
	//run_asio_curl();

	return 1;
}

#endif // _TEST_MAIN
