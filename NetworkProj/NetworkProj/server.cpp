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

		std::cout << acceptor.local_endpoint().address().to_string() << ":"
			<< acceptor.local_endpoint().port() << " listen..." << std::endl;

		//do {
			std::shared_ptr<tcpsocket> sock(new tcpsocket(ios));

			acceptor.accept(*sock);

			std::cout << "request from ";
			std::cout << sock->remote_endpoint().address().to_string() << ":" << sock->remote_endpoint().port();
			std::cout << " accepted." << std::endl;

			receive_hidden(sock);

		//} while (true);

		//std::string message;
		//do {

		//	message = receive(sock);

		//	std::cout << "> " << message << std::endl;

		//} while (message != std::string("quit"));


		ios.run();
	}
	catch (system::system_error& e) {
		std::cout << "Error occured! Error code = " << e.code()
			<< ". Message : " << e.what();
		return e.code().value();
	}

	return 0;
}
std::string server::receive(tcpsocket& sock) {
	std::vector<char> message(100);
	asio::mutable_buffers_1 buffer = asio::buffer(message, 100);
	size_t readsize = sock.receive(buffer);

	return std::string(message.begin(), message.begin() + readsize);
}
void server::receive_hidden(std::shared_ptr<tcpsocket>& sock) {
	std::shared_ptr<session> s(new session);

	s->sock = sock;

	asio::async_read(*(s->sock), asio::buffer((char*)&s->length, sizeof(std::size_t)),
		std::bind(server::sizehandler, std::placeholders::_1, std::placeholders::_2, s));

	std::cout << "receive hidden" << std::endl;
}

void server::sizehandler(boost::system::error_code e, std::size_t byte_read_size, std::shared_ptr<session>& s) {
	std::cout << "size handler called." << std::endl;
	if (e.value() != 0) {
		std::cerr << "ERROR[" << e.value() << "] " << e.message() << std::endl;
		return;
	}

	std::cout << byte_read_size << "bytes read." << std::endl;
	std::cout << "data size is " << s->length << "bytes." << std::endl;

	s->buffer.resize(s->length);
	asio::async_read(*(s->sock), asio::buffer(s->buffer, s->length),
		std::bind(server::handler, std::placeholders::_1, std::placeholders::_2, s));
}
void server::handler(boost::system::error_code e, std::size_t byte_read_size, std::shared_ptr<session>& s) {
	std::cout << "data handler called." << std::endl;
	if (e.value() != 0) {
		std::cerr << "ERROR[" << e.value() << "] " << e.message() << std::endl;
		return;
	}

	std::cout << byte_read_size << "bytes read." << std::endl;

	std::string message(s->buffer.begin(), s->buffer.begin() + byte_read_size);

	std::cout << "[" << s->sock->remote_endpoint().address() << ":"
		<< s->sock->remote_endpoint().port() << "] "
		<< message << std::endl;

	s->read_data = message;

	if (message != std::string("quit")) {
		asio::async_read(*(s->sock), asio::buffer((char*)&s->length, sizeof(std::size_t)),
			std::bind(server::sizehandler, std::placeholders::_1, std::placeholders::_2, s));
	}
}