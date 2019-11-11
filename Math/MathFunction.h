#ifndef POWER_H
#define POWER_H

#ifdef MATH_EXPORT
#define MATH_API __declspec(dllexport)
#else
#define MATH_API
#endif

#ifdef __cplusplus
extern "C" { 
#endif
	MATH_API double power(double base, int exponent);
	MATH_API int add(int a, int b);
#ifdef __cplusplus
}
#endif

#endif // end POWER_H