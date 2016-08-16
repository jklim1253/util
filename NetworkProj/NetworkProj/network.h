#pragma once

#define _WIN32_WINNT 0x0601
#include <string>
#include <boost/asio.hpp>

namespace network {
	class client {
	public :
		int connect(const std::string& ip_address, const unsigned short& port);
		int send(boost::asio::ip::tcp::socket& sock, const std::string& str);
	};
	class server {
	public :
		server(const int& backlog = 30);

		int ready(const unsigned short& port);
		int receive(boost::asio::ip::tcp::socket& sock);
	private :
		const int BACKLOG_SIZE;
	};
}