#pragma once

#define _WIN32_WINNT 0x0601
#include <string>
#include <boost/asio.hpp>

namespace network {
	struct session {
		std::shared_ptr<boost::asio::ip::tcp::socket> sock;
		std::string data;
	};
	class client {
	public :
		int connect(const std::string& ip_address, const unsigned short& port);
		int send(boost::asio::ip::tcp::socket& sock, const std::string& str);
		int send_away(std::shared_ptr<boost::asio::ip::tcp::socket>& sock, const std::string& str);
		static void handler(boost::system::error_code e, std::size_t transferred, std::shared_ptr<session>& s);
	};
	class server {
	public :
		server(const int& backlog = 30);

		int ready(const unsigned short& port);
		std::string receive(boost::asio::ip::tcp::socket& sock);
	private :
		const int BACKLOG_SIZE;
	};
}