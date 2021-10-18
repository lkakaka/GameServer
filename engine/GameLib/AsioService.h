#pragma once
#include <vector>
#include <iostream>
#include <thread>
#include "boost/asio.hpp"
#include "Singleton.h"

#define MAIN_IO_PTR AsioServiceMgr::getSingleton()->getMainAsioService()->getIoService()
#define MAIN_IO (*AsioServiceMgr::getSingleton()->getMainAsioService()->getIoService())

#define MAIN_IO_SERVICE_PTR AsioServiceMgr::getSingleton()->getMainAsioService()

class AsioService
{
private:
	std::shared_ptr<boost::asio::io_service> m_io;

	void signalHandler(std::function<void()> task, const boost::system::error_code& error, int signal_number);
public:
	AsioService();
	inline boost::asio::io_service* getIoService() { return m_io.get(); }
	void run();
	void async_run_task(std::function<void()> task);
	inline void stop() { m_io->stop(); }
};

class AsioServiceMgr : public Singleton<AsioServiceMgr>{
private:
	std::vector<AsioService*> m_services;
public:
	AsioService* createAsioService();
	AsioService* getMainAsioService();

	void stopAll();
};

