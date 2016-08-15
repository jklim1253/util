#include <iostream>
#include <string>
#include "network.h"

int main(void) {

	std::string raw_ip_address = "127.0.0.1";
	unsigned short port_num = 3333;

	int command;
	do {
		std::cout << "0. exit\n1. client\n2. server\n:>";
		std::cin >> command;
	} while (command != 0 && command != 1 && command != 2);
	
	if (command == 0) {
		return 0;
	}
	else if (command == 1) {
		std::string ip_address;
		unsigned short port;

		std::cout << "server ip : ";
		std::cin >> ip_address;
		std::cout << "server port : ";
		std::cin >> port;

		network::client cli;

		cli.connect(ip_address, port);
	}
	else if (command == 2) {
		unsigned short port;

		std::cout << "port to open : ";
		std::cin >> port;

		network::server svr;

		svr.ready(port);
	}

	return 0;
}