#include "AsioService.h"
#include <signal.h>
#include "Logger.h"

#ifdef _WIN32
#define SIGUSR1 11
#endif

INIT_SINGLETON_CLASS(AsioServiceMgr)

AsioService::AsioService() : m_io(new boost::asio::io_service()) {

}

void AsioService::run() {
	boost::asio::io_service::work work(*m_io);
	m_io->run();
}

void AsioService::async_run_task(std::function<void()> task) {
	//m_io->post(std::bind(&AsioService::taskHandler, this, task));
	boost::asio::post(*m_io, std::bind(&AsioService::taskHandler, this, task));
	//boost::asio::post(*m_io, task);
	/*boost::asio::signal_set signals(*m_io, SIGUSR1);
	signals.async_wait(std::bind(&AsioService::signalHandler, this, task, std::placeholders::_1, std::placeholders::_2));
	raise(SIGUSR1);*/
}

void AsioService::signalHandler(std::function<void()> task, const boost::system::error_code& error, int signal_number) {
	if (error) {
		LOG_ERROR("asio service async task error!!!");
	}
	LOG_DEBUG("signalHandler, %d", signal_number);
	task();
}

void AsioService::taskHandler(std::function<void()> task) {
	task();
}

AsioService* AsioServiceMgr::createAsioService() {
	AsioService* service = new AsioService();
	m_services.push_back(service);
	return service;
}

AsioService* AsioServiceMgr::getMainAsioService() {
	return *m_services.begin();
}

void AsioServiceMgr::stopAll() {
	for (auto iter = m_services.begin(); iter != m_services.end(); iter++) {
		(*iter)->stop();
	}
}
