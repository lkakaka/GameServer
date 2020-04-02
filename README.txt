
1、编译工程
   1) windows:
		需安装cmake(3.0+) vs2017+
		
		1、boost(1_70_0)库编译(下载地址https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.zip)
		在工程同级目录下解压boost
		执行bootstrap.bat, 生成b2.exe
		执行b2.exe
		
		2、 protobuf(3.9.0)库编译（下载地址:https://github.com/protocolbuffers/protobuf/releases/download/v3.9.0/protobuf-all-3.9.0.zip）:
	   在工程同级目录下解压protobuf
	   cd protobuf-3.9.0/cmake
	   mkdir build
	   cd build
	   cmake ..
	   打开工程protobuf.sln, 编译
	   
		3、生成vs工程
	   执行工程目录下的_cmake.bat
	
	2) linux：
		需安装cmake(3.0+) gcc gcc-c++
		1、boost(1_70_0)库编译(下载地址https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz)
	   
	   2、 protobuf(3.9.0)库编译(下载地址：https://github.com/protocolbuffers/protobuf/releases/download/v3.9.0/protobuf-all-3.9.0.tar.gz)
		
	   
	   3、生成vs工程
	   执行工程目录下的_cmake.sh