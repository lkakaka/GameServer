
#include <iostream>
#include "boost/asio.hpp"

#include "Network.h"
#include "Log.h"

using namespace std;

int main()
{
	Log::initLog();

	boost::asio::io_service io;
	Network* network = new Network(&io);
	network->startListen();


	/*boost::asio::io_service io;
	boost::asio::deadline_timer t(io, boost::posix_time::seconds(5));
	t.wait();*/

	
	Log::logInfo("%MyServer Start!!!");

	return 0;
}