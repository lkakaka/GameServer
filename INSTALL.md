
# 编译工程
  ## windows:
  1. 需安装cmake(3.0+) vs2017+ python3.8、mysql
		
  2. 相关依赖库编译
  * boost(1_70_0)库编译[download](https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.zip)
		在工程同级目录下解压boost,  
		执行bootstrap.bat, 生成b2.exe  
		执行b2.exe  
		
  * protobuf(3.9.0)库编译[download](https://github.com/protocolbuffers/protobuf/releases/download/v3.9.0/protobuf-all-3.9.0.zip):  
	   在工程同级目录下解压protobuf  
	   cd protobuf-3.9.0/cmake  
	   mkdir build  
	   cd build  
	   cmake -DCMAKE_INSTALL_PREFIX=../install ..  
	   打开工程protobuf.sln, 编译  
	   编译install工程, 安装头文件库文件到../install目录下  
	   拷贝头文件库文件到Depends/protobuf-3.9.0  
	   
	   (不需要这步、编译好的python版已放在script/python/google下)  
	   python版安装（先生成protoc.exe，放到protobuf-3.9.0/src/目录下）  
		cd protobuf-3.9.0/python  
		python3 setup.py build  
		python setup.py install (不知为什么需要安装,否则release会崩)
	    
  * ZMQ(4.3.3) [download](https://github.com/zeromq/libzmq)
		cd libzmq  
		mkdir build  
		cmake ..  
		打开工程ZeroMQ.sln, 编译
		
  * mysql Connector/C++ 8.0.19 （mysql-connector-c++-8.0.19-winx64.zip）[download](https://dev.mysql.com/downloads/file/?id=492171)
		已包含头文件和库文件
		
  * hiredis [download](https://github.com/redis/hiredis.git)
		cd hiredis  
		mkdir build  
		cd build  
		cmake -DCMAKE_INSTALL_PREFIX=./usr ..  
		打开工程, 编译  
		编译INSTALL工程,把头文件和库文件安装到./usr目录下  
		拷贝 ./usr目录下头文件库文件到工程里(Depends/hiredis/include/win, Depends/hiredis/libs/Debug[Release])
		
  3. 生成项目vs工程
	   执行engine目录下的_cmake.bat
	
  ## linux：
  1. 需安装cmake(3.0+) gcc gcc-c++、python3.8、mysql
		
  2. 相关依赖库编译
  * boost(1_70_0)库编译[download](https://dl.bintray.com/boostorg/release/1.70.0/source/boost_1_70_0.tar.gz)
		./bootstrap.sh  
		./b2  
	   
  * protobuf(3.9.0)库编译[download](https://github.com/protocolbuffers/protobuf/releases/download/v3.9.0/protobuf-all-3.9.0.tar.gz)
    ./configure  
		make  
		make install  
		
    python版安装 (不需要这步、编译好的python版已放在script/python/google下)  
		cd protobuf-3.9.0/python  
		python3 setup.py build  
		python setup.py install (不知为什么需要安装,否则会崩)  
		
  * ZMQ(4.3.3) [download](https://github.com/zeromq/libzmq)  
		cd libzmq  
		mkdir build  
		cd build  
		cmake ..  
		make  
		
  * mysql Connector/C++ 8.0.19  （mysql-connector-c++-8.0.19-linux-glibc2.12-x86-64bit.tar.gz）[download](https://dev.mysql.com/downloads/file/?id=492171)
		已包含头文件和库文件
		
  * hiredis [download](https://github.com/redis/hiredis.git)
		cd hiredis
		mkdir build
		cd build
		cmake -DCMAKE_INSTALL_PREFIX=./usr ..
		make
		make install
		拷贝 ./usr目录下头文件库文件到工程里(Depends/hiredis/include/linux, Depends/hiredis/libs)
		
  3. 生成工程Makefile
	   cd engine  
	   mkdir build  
	   ./_cmake.sh --script=[LUA|PYTHON] --build_type=[Debug|Release]
	   ./build.sh  

# 启动
  ## windows:
  1. 启动redis
  2. 启动center  
    shell/start_center.bat
  3. 启动服务器   
  shell/start_all.bat
  4. 关闭服务器  
  shell/stop_all.bat
	
  ## linux:
  1. 在bin目下ln -s pb.so libpb.so
  2. cd shell
  3. 启动center  
	./start.sh center all
  4. 启动101服 
	./start.sh 101 all
  5. 关闭服务器  
	./stop.sh all
	
	
