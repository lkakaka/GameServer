if(WIN32)
	# 拷贝Depends目录下依赖库
	file(GLOB_RECURSE DLL_FILES ${DEPENDS_ROOT}/*.dll)
	foreach(DLL_FILE ${DLL_FILES})
		string(REGEX MATCH "Debug" debug_ver ${DLL_FILE})
		string(REGEX MATCH "Release" release_ver ${DLL_FILE})
		# 不在release目录下的dll拷贝到bin/Debug目录下
		if(NOT release_ver)
		   # message(STATUS ${DLL_FILE})
		   INSTALL(FILES ${DLL_FILE} CONFIGURATIONS Debug DESTINATION ${PROJECT_SOURCE_DIR}/bin/Debug)
		endif()
		
		# 不在Debug目录下的dll拷贝到bin/Release目录下
		if(NOT debug_ver)
		  # message(STATUS ${DLL_FILE}) 
		  INSTALL(FILES ${DLL_FILE} CONFIGURATIONS Release DESTINATION ${PROJECT_SOURCE_DIR}/bin/Release)
		endif()
	endforeach(DLL_FILE)
endif(WIN32)
