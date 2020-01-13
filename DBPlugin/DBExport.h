#pragma once

#ifdef DBPLUGIN_EXPORT
#define DBPLUNGIN_API __declspec(dllexport)
#else
#define DBPLUNGIN_API
#endif
