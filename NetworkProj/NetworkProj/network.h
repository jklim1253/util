#pragma once

#define _WIN32_WINNT 0x0601
#include <string>

namespace network {
	class client {
	public :
		int connect(const std::string& ip_address, const unsigned short& port);
	};
	class server {
	public :
		server(const int& backlog = 30);

		int ready(const unsigned short& port);
	private :
		const int BACKLOG_SIZE;
	};
}