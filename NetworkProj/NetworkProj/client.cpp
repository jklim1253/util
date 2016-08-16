#include "network.h"
#include <iostream>
#include <boost/asio.hpp>
#include <list>

using namespace network;
using namespace boost;

int client::connect(const std::string& ip_address, const unsigned short& port) {
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip_address), port);

		asio::io_service ios;

		asio::ip::tcp::socket sock(ios, ep.protocol());

		sock.connect(ep);

		std::cout << sock.local_endpoint().address().to_string() << ":" << sock.local_endpoint().port();
		std::cout << " connected." << std::endl;

		send(sock, "hello, I'm client!");
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message : " << e.what();

		return e.code().value();
	}

	return 0;
}
int client::send(asio::ip::tcp::socket& sock, const std::string& str) {

	asio::const_buffers_1 buffer = asio::buffer(str);
	sock.send(buffer);

	return 0;
}