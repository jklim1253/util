#pragma once

#define _WIN32_WINNT 0x0601
#include <string>
#include <vector>
#include <boost/asio.hpp>

namespace network {
	typedef typename boost::asio::ip::tcp::socket tcpsocket;

	// session class
	struct session {
		std::shared_ptr<tcpsocket> sock;
		std::string data;
		std::vector<char> buffer;
		std::string read_data;
		std::size_t length;
	};

	// client class
	class client {
	public :
		int connect(const std::string& ip_address, const unsigned short& port);
		int send(tcpsocket& sock, const std::string& str);
		int send_away(std::shared_ptr<tcpsocket>& sock, const std::string& str);

	protected :
		static void handler(boost::system::error_code e, std::size_t transferred, std::shared_ptr<session>& s);

		boost::asio::io_service ios;
	};

	// server class
	class server {
	public :
		server(const int& backlog = 30);

		int ready(const unsigned short& port);
		std::string receive(tcpsocket& sock);
		void receive_hidden(std::shared_ptr<tcpsocket>& sock);

	protected :
		static void sizehandler(boost::system::error_code e, std::size_t byte_read_size, std::shared_ptr<session>& s);
		static void handler(boost::system::error_code e, std::size_t byte_read_size, std::shared_ptr<session>& s);

	private :
		const int BACKLOG_SIZE;
	};
}