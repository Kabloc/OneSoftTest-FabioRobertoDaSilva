#include <boost/asio.hpp>
#include <market_data.h>
#include <iostream>
#include <functional>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
	session(boost::asio::io_service &ios) 
		: ios_(ios)
		, socket_(ios) 
	{}

	~session() {
		std::cout << "session closed" << std::endl;
	}

	void send_message(const std::string& data, std::function<void(const boost::system::error_code& err, std::size_t bytes_transferred)> callback);

	tcp::socket& get_socket() 
	{ return socket_; }

	std::chrono::time_point<std::chrono::system_clock>& get_last_send()
	{ return last_send_; }

private:
	tcp::socket socket_;
	boost::asio::io_service &ios_;
	boost::system::error_code err_;
	size_t bytes_transferred_;
	std::chrono::time_point<std::chrono::system_clock> last_send_;

};

class neg_server : public std::enable_shared_from_this<neg_server> {
public:
	neg_server(boost::asio::io_service& ios, short port, const std::string& file_name);

private:
	void handle_accept(std::shared_ptr<session> ses, const boost::system::error_code& err);
	void start_transmition(std::shared_ptr<session> ses);
	void on_transmition(std::shared_ptr<session> ses, std::size_t order);

	boost::asio::io_service& ios_;
	tcp::acceptor acceptor_;
	market_data::loader data_loader_;
	boost::asio::deadline_timer timer_;
};

