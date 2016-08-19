#define _WIN32_WINNT 0x0601
#include <list>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>

using namespace boost::asio::ip;

// Initiator
// Application code. visible to programmers.
struct Initiator { virtual void run() = 0; };
// Procator
class Procator;
// Asynchronous Operation Processor
class Async_Processor;
// Asynchronous Operation
struct Async_Operation { virtual void do_operation() = 0; };
// Completion Handler
struct Completion_Handler { virtual void do_handle() = 0; };
// Asynchronous Event Demultiplexer
class Async_Event_Demultiplexer;
// Completion Event Queue
class Completion_Event_Queue;

namespace ProActor {
	// TODO
	// 1. execute asynchronous operation using asynchronous operation processor.
	// 2. enqueue async-operation-handler to completion-event-queue.
	// 3. when async-operation complete, 
	//		proactor use async-event-demultiplexer to dequeue async-operation-handler from completion-event-queue.
	// 4. 
	class Service {

	};
	// TODO
	// 1. make async-operation for async-operation-processor.
	// 2. make completion-handler to process post-completion-of-async-operation.
	// 3. 
	struct User : public Initiator {
	public :
		virtual void run() {
			pre_run();
		}
		virtual void pre_run() = 0;
		virtual void make_async_operation() = 0;
	protected :
		std::list<std::shared_ptr<Async_Operation>> Opers_;
	};
};

struct Session {

	std::shared_ptr<tcp::socket>& get_socket() {
		return socket_;
	}
	const std::shared_ptr<tcp::socket>& get_socket() const {
		return socket_;
	}
private :
	std::shared_ptr<tcp::socket> socket_;
};
class Server : public ProActor::User {
	typedef Server Me;
public :
	Server() 
		: ios_()
		, acceptor_(ios_)
		, BACKLOG(5)
	{

	}
	virtual void pre_run() {
		std::shared_ptr<tcp::socket> sock_(new tcp::socket(ios_));

		acceptor_.listen(BACKLOG);
	}
	// ready code for server to do something asynchronously.
	virtual void make_async_opration() {
		//Opers_.push_back(std::make_shared<Async_Accept_Operation>(this));
		//Opers_.push_back(std::make_shared<Async_Read_Operation>(this));
		//Opers_.push_back(std::make_shared<Async_Write_Operation>(this));
	}

	boost::asio::io_service& get_service() { return ios_; }
	const boost::asio::io_service& get_service() const { return ios_; }
	tcp::acceptor& get_acceptor() { return acceptor_; }
	const tcp::acceptor& get_acceptor() const { return acceptor_; }
	std::list<std::shared_ptr<Session>>& get_sessionlist() { return sessionlist; }
	const std::list<std::shared_ptr<Session>>& get_sessionlist() const { return sessionlist; }
protected :
private :
	boost::asio::io_service ios_;
	tcp::acceptor acceptor_;
	std::list<std::shared_ptr<Session>> sessionlist;
	int BACKLOG;
};
struct Async_Accept_Operation : public Async_Operation {
	Async_Accept_Operation(Server* server) : server_(server) {}
	virtual void do_operation() {
		// required code for do_operation.
		std::shared_ptr<tcp::socket> socket_(std::make_shared<tcp::socket>(server_->get_service()));

		std::shared_ptr<Session> s(std::make_shared<Session>());
		s->get_socket() = socket_;

		server_->get_sessionlist().push_back(s);

		typedef void(*acceptHandler)(const boost::system::error_code& e);
		std::shared_ptr<acceptHandler> handler;
		// required code end.

		server_->get_acceptor().async_accept(*socket_, *handler);
	}
	Server* server_;
};
struct Async_Read_Operation : public Async_Operation {
	Async_Read_Operation(Server* server) : server_(server) {}
	virtual void do_operation() {

	}
	Server* server_;
};
struct Async_Write_Operation : public Async_Operation {
	Async_Write_Operation(Server* server) : server_(server) {}
	virtual void do_operation() {

	}
	Server* server_;
};
struct Accept_Completion_Handler : public Completion_Handler {
	virtual void do_handle() {

	}
};
struct Read_Completion_Handler : public Completion_Handler {
	virtual void do_handle() {

	}
};
struct Write_Completion_Handler : public Completion_Handler {
	virtual void do_handle() {

	}
};
int main(void) {

	return 0;
}