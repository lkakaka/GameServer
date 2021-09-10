#include <iostream>
#include "Test.h"

#define NEW new(__FILE__, __LINE__)

class Foo {
public:
	int a;
	int b;
	
	Foo() { std::cout << "Foo()" << std::endl; }
	~Foo() { std::cout << "~Foo()" << std::endl; }

	void* operator new(std::size_t size)
	{
		void* ptr = std::malloc(size);
		printf("operator new, size=%ld, ptr=%ld\n", size, (int64_t)ptr);
		return ptr;
	}

	void* operator new(std::size_t size, const char* file, int line)
	{
		void* ptr = std::malloc(size);
		printf("operator new, %s:%d, size=%ld, ptr=%ld\n", file, line, size, (int64_t)ptr);
		return ptr;
	}

	void* operator new (std::size_t size, void* p)
	{
		printf("placement new, size=%ld, ptr=%ld\n", size, (int64_t)p);
		return p;
	}

	void operator delete(void* ptr)
	{
		//std::cout << "operator delete, ptr=" << ptr << std::endl;
		printf("operator delete, ptr=%ld\n", (int64_t)ptr);
		std::free(ptr);
	}

};

class Foo1 {
public:
	int a;
};

void* operator new(size_t size, const char* file, int line) {
	std::cout << "my operator new \n";
	return malloc(size); 
}

void testOperateNew()
{
	Foo* m = NEW Foo;
	Foo* m2 = new(m) Foo;
	std::cout << sizeof(Foo) << std::endl;
	//delete m2;
	delete m;

	Foo1* f1 = NEW Foo1;
	delete f1;
}