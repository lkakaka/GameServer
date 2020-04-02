#ifndef POWER_H
#define POWER_H

#include "../Common/ServerExports.h"

//#ifdef MATH_EXPORT
//#define MATH_API __declspec(dllexport)
//#else
//#define MATH_API
//#endif

#ifdef __cplusplus
extern "C" { 
#endif
	SERVER_EXPORT_API double power(double base, int exponent);
	SERVER_EXPORT_API int add(int a, int b);
#ifdef __cplusplus
}
#endif

#endif // end POWER_H