#pragma once

#ifdef HTTP_SERVER_EXPORT
#define HTTP_SERVER_API __declspec(dllexport)
#else
#define HTTP_SERVER_API
#endif
