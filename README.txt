
1、编译工程
   1) windows:
		需安装cmake(3.0+) vs2017+ python3.8、mysql
		
		相关依赖库编译
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
	   
	   (不需要这步、编译好的python版已放在script/python/google下)
	   python版安装（先生成protoc.exe，放到protobuf-3.9.0/src/目录下）
		cd protobuf-3.9.0/python
		python3 setup.py build
		python setup.py install (不知为什么需要安装,否则release会崩)
	    
		3、ZMQ(4.3.3) https://github.com/zeromq/libzmq
		cd libzmq
		mkdir build
		cmake ..
		打开工程ZeroMQ.sln, 编译
		
		4、mysql Connector/C++ 8.0.19 （mysql-connector-c++-8.0.19-winx64.zip）(https://dev.mysql.com/downloads/file/?id=492171)
		已包含头文件和库文件
		
		
	   
		生成项目vs工程
	   执行工程目录下的_cmake.bat
	
	2) linux：
		需安装cmake(3.0+) gcc gcc-c++、python3.8、mysql
		
		相关依赖库编译
		1、boost(1_70_0)库编译(下载地址https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz)
		./bootstrap.sh
		./b2
	   
	   2、 protobuf(3.9.0)库编译(下载地址：https://github.com/protocolbuffers/protobuf/releases/download/v3.9.0/protobuf-all-3.9.0.tar.gz)
	    ./configure
		make
		make install
		
		python版安装 (不需要这步、编译好的python版已放在script/python/google下)
		cd protobuf-3.9.0/python
		python3 setup.py build
		python setup.py install (不知为什么需要安装,否则会崩)
		
		3、ZMQ(4.3.3) https://github.com/zeromq/libzmq
		cd libzmq
		mkdir build
		cd build
		cmake ..
		make
		
		
		4、mysql Connector/C++ 8.0.19  （mysql-connector-c++-8.0.19-linux-glibc2.12-x86-64bit.tar.gz）(https://dev.mysql.com/downloads/file/?id=492171)
		已包含头文件和库文件
		
		
		5、hiredis (https://github.com/redis/hiredis.git)
		cd hiredis
		mkdir build
		cd build
		cmake ..
		make
		
	   
	   3、生成工程Makefile
	   执行工程目录下的_cmake.sh
	   cd build
	   make

2、启动
	windows:
	shell/start_all.bat
	
	linux:
	运行shell/start_all.sh