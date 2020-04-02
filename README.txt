
1、编译工程
   1) windows:
		需安装cmake(3.0+) vs2017+
		
		1、 protobuf(3.9.0)库编译（下载地址:https://github.com/protocolbuffers/protobuf/releases/download/v3.9.0/protobuf-all-3.9.0.zip）:
	   在工程同级目录下解压protobuffer
	   cd protobuf-3.9.0/cmake
	   mkdir build
	   cd build
	   cmake ..
	   打开工程protobuf.sln, 编译
	   
		2、生成vs工程
	   执行工程目录下的_cmake.bat
	
	2) linux：
		需安装cmake(3.0+) gcc gcc-c++
	   