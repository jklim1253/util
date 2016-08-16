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

		std::shared_ptr<asio::ip::tcp::socket> sock(new asio::ip::tcp::socket(ios, ep.protocol()));

		sock->connect(ep);

		std::cout << sock->local_endpoint().address().to_string() << ":" << sock->local_endpoint().port();
		std::cout << " connected." << std::endl;

		std::string message("Hello");
		do {
			send_away(sock, message);
			std::getline(std::cin, message);
		} while (message != std::string("quit"));

		ios.run();
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message : " << e.what();

		return e.code().value();
	}

	return 0;
}
int client::send(asio::ip::tcp::socket& sock, const std::string& str) {

	sock.send(asio::buffer(str));

	return 0;
}
int client::send_away(std::shared_ptr<asio::ip::tcp::socket>& sock, const std::string& str) {

	std::shared_ptr<session> s(new session());
	s->sock = sock;
	s->data = str;

	sock->async_send(asio::buffer(s->data), std::bind(client::handler, std::placeholders::_1, std::placeholders::_2, s));

	return 0;
}
void client::handler(system::error_code e, std::size_t transferred, std::shared_ptr<session>& s) {

	std::cout << s->data << "[";
	std::cout << transferred << "bytes sent]" << std::endl;
}