#include "network.h"
#include <iostream>
#include <boost/asio.hpp>

using namespace network;

int client::connect(const std::string& ip_address, const unsigned short& port) {
	using namespace boost;
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip_address), port);

		asio::io_service ios;

		asio::ip::tcp::socket sock(ios, ep.protocol());

		sock.connect(ep);

		std::cout << sock.local_endpoint().address().to_string() << ":" << sock.local_endpoint().port();
		std::cout << " connected." << std::endl;
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message : " << e.what();

		return e.code().value();
	}

	return 0;
}