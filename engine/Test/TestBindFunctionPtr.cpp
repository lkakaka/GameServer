#include <boost/optional.hpp>
#include <boost/bind.hpp>
#include <iostream>
#include <ostream>
#include "Test.h"
using namespace std;

template<unsigned ID, typename Functor>
boost::optional<Functor>& get_local()
{
    static boost::optional<Functor> local;
    return local;
}

template<unsigned ID, typename Functor>
typename Functor::result_type wrapper()
{
    return get_local<ID, Functor>().get()();
}

template<typename ReturnType>
struct Func
{
    typedef ReturnType(*type)();
};

template<unsigned ID, typename Functor>
typename Func<typename Functor::result_type>::type get_wrapper(Functor f)
{
    (get_local<ID, Functor>()) = f;
    return wrapper<ID, Functor>;
}

// ----------------------------------------------------------------------

void test(void (*fptr)())
{
    fptr();
}

struct SomeStruct
{
    int data;
    void some_method()
    {
        cout << data << endl;
    }
    void another_method()
    {
        cout << -data << endl;
    }
};

void testBindFunction()
{
    SomeStruct local[] = { {11}, {22}, {33} };

   void (*fptr0)() = get_wrapper<0>(boost::bind(&SomeStruct::some_method, local[0]));
    /*get_wrapper<0>(boost::bind(&SomeStruct::some_method, local[1]));
    get_wrapper<1>(boost::bind(&SomeStruct::some_method, local[1]));

    test(fptr0); // 这里输出了22
    */
   
    test(get_wrapper<0>(boost::bind(&SomeStruct::some_method, local[0])));
    test(get_wrapper<1>(boost::bind(&SomeStruct::another_method, local[0])));

    test(get_wrapper<2>(boost::bind(&SomeStruct::some_method, local[1])));
    test(get_wrapper<3>(boost::bind(&SomeStruct::another_method, local[1])));

    test(get_wrapper<4>(boost::bind(&SomeStruct::some_method, local[2])));
    test(get_wrapper<5>(boost::bind(&SomeStruct::another_method, local[2])));
}
