#pragma once

#ifdef SERVER_EXPORT
	#ifndef SERVER_EXPORT_API
		#ifdef WIN32
			#define SERVER_EXPORT_API __declspec(dllexport)
		#else
			#define SERVER_EXPORT_API __attribute__((visibility("default")))
		#endif
	#endif
#else
#ifdef WIN32
#define SERVER_EXPORT_API __declspec(dllimport)
#else
#define SERVER_EXPORT_API 
#endif
#endif //(SERVER_EXPORT)
