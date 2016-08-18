#include "network.h"
#include <iostream>
#include <thread>
#include <boost/asio.hpp>
#include <list>

using namespace network;
using namespace boost;

int client::connect(const std::string& ip_address, const unsigned short& port) {
	try {
		asio::ip::tcp::endpoint ep(asio::ip::address::from_string(ip_address), port);

		std::shared_ptr<tcpsocket> sock(new tcpsocket(ios, ep.protocol()));

		sock->connect(ep);

		std::cout << sock->remote_endpoint().address().to_string() << ":" << sock->remote_endpoint().port();
		std::cout << " connected." << std::endl;

		std::thread t([this, &sock]() {
			std::string message("Hello");
			do {
				std::getline(std::cin, message);
				if (message.length() != 0) {
					send_away(sock, message);

					std::cout << "send away" << std::endl;
				}
			} while (message != std::string("quit"));
		});

		t.join();

		sock->close();
		ios.run();
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message : " << e.what();

		return e.code().value();
	}

	return 0;
}
int client::send(tcpsocket& sock, const std::string& str) {

	sock.send(asio::buffer(str));

	return 0;
}
int client::send_away(std::shared_ptr<tcpsocket>& sock, const std::string& str) {

	std::shared_ptr<session> s(new session());
	s->sock = sock;
	s->data.append(str);
	s->length = s->data.length();

	s->sock->async_send(asio::buffer((char*)&s->length, sizeof(std::size_t)),
		std::bind(client::handler, std::placeholders::_1, std::placeholders::_2, s));
	s->sock->async_send(asio::buffer(s->data.c_str(), s->length), 
		std::bind(client::handler, std::placeholders::_1, std::placeholders::_2, s));

	std::cout << "send away end" << std::endl;
	return 0;
}
void client::handler(system::error_code e, std::size_t transferred, std::shared_ptr<session>& s) {
	std::cout << "handler called." << std::endl;
	if (e.value() != 0) {
		std::cerr << "ERROR[" << e.value() << "] " << e.message() << std::endl;
		return;
	}

	std::cout << "[" << transferred << "bytes sent]" << std::endl;
}