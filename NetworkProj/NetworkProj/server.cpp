#include "network.h"
#include <iostream>
#include <vector>
#include <boost/asio.hpp>

using namespace network;
using namespace boost;
server::server(const int& backlog/*=30*/) : BACKLOG_SIZE(backlog) {

}
int server::ready(const unsigned short& port) {
	asio::ip::tcp::endpoint ep(asio::ip::address_v4::any(), port);

	asio::io_service ios;

	try {
		asio::ip::tcp::acceptor acceptor(ios, ep.protocol());

		acceptor.bind(ep);

		acceptor.listen(BACKLOG_SIZE);

		asio::ip::tcp::socket sock(ios);

		acceptor.accept(sock);

		std::cout << "request from ";
		std::cout << sock.local_endpoint().address().to_string() << ":" << sock.local_endpoint().port();
		std::cout << " accepted." << std::endl;

		std::string message;
		do {

			message = receive(sock);

			std::cout << "> " << message << std::endl;

		} while (message != std::string("quit"));

		sock.close();
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message : " << e.what();
		return e.code().value();
	}

	return 0;
}
std::string server::receive(boost::asio::ip::tcp::socket& sock) {
	std::vector<char> message(100);
	asio::mutable_buffers_1 buffer = asio::buffer(message, 100);
	size_t readsize = sock.receive(buffer);

	return std::string(message.begin(), message.begin() + readsize);
}
