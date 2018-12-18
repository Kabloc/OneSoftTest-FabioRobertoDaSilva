#include <boost/asio.hpp>
#include <market_data.h>
#include <iostream>
#include <functional>

using boost::asio::ip::tcp;

class session : public std::enable_shared_from_this<session> {
public:
	session(boost::asio::io_service &ios) 
		: socket_(ios) 
		, timer_(ios)
	{}

	~session() {
		std::cout << "session closed" << std::endl;
	}

	void send_message(const std::string& data, std::function<void(const boost::system::error_code& err)> callback);
	void send_message(const std::string& data, const std::chrono::milliseconds& lapse, std::function<void(const boost::system::error_code& err)> callback);

	tcp::socket& get_socket() 
	{ return socket_; }

	std::chrono::time_point<std::chrono::steady_clock>& get_last_send()
	{ return last_send_; }

private:
	std::string data_;
	tcp::socket socket_;
	boost::asio::deadline_timer timer_;
	std::chrono::time_point<std::chrono::steady_clock> last_send_;
};

class neg_server : public std::enable_shared_from_this<neg_server> {
public:
	neg_server(boost::asio::io_service& ios, short port, const std::string& file_name);

private:
	void handle_accept(std::shared_ptr<session> ses, const boost::system::error_code& err);
	void start_transmition(std::shared_ptr<session> ses);
	void on_transmition(std::shared_ptr<session> ses, std::size_t order);

	tcp::acceptor acceptor_;
	boost::asio::io_service& ios_;
	market_data::loader data_loader_;
};

