
def test():
    from cffi import FFI
    ffi = FFI()
    ffi.cdef("""
        int printf(const char *format, ...); // 可以从man page或者头文件里面复制
    """)
    C = ffi.dlopen(None) # 加载整个C命名空间
    arg = ffi.new("char[]", b"world") # 等于C代码: char arg[] = "world";
    print(C.printf(b"hi there, %s.\n", arg)) # 调用printf

def test1():
    from cffi import FFI
    ffibuilder = FFI()

    # cdef() expects a single string declaring the C types, functions and
    # globals needed to use the shared object. It must be in valid C syntax.
    ffibuilder.cdef("""
        double power(double base, int exponent);
    """)

    # set_source() gives the name of the python extension module to
    # produce, and some C source code as a string.  This C code needs
    # to make the declarated functions, types and globals available,
    # so it is often just the "#include".
    ffibuilder.set_source("_math_cffi",
    """
        #include "../Math/MathFunction.h"   // the C header of the library
    """,
        libraries=['../Bin/Release/MathFunction'])   # library name, for the linker
    
    ffibuilder.compile(verbose=True)

def test_use():
    # use function
    # import _math_cffi
    from py_cffi._math_cffi import ffi,lib
    print("math.power(2,3)=", lib.power(2, 3))

