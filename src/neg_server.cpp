#include <neg_server.h>
#include <functional>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>

void session::send_message(const std::string& data, std::function<void(const boost::system::error_code& err)> callback)
{ send_message(data, std::chrono::milliseconds(0), callback); }

void session::send_message(const std::string& data, const std::chrono::milliseconds& lapse,
	std::function<void(const boost::system::error_code& err)> callback) 
{
	using boost::asio::buffer;
	auto time_ago = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now() - last_send_);
	auto new_lapse = lapse - time_ago;

	auto self = shared_from_this();
	if (new_lapse.count() > 0) {
		timer_.expires_from_now(boost::posix_time::milliseconds(new_lapse.count()));
		timer_.async_wait(
			[self, data, callback](const boost::system::error_code& err) {
				self->send_message(data, callback);
			}
		);
	}
	else {
		data_ = data + "\r\n";
		boost::asio::async_write(socket_, buffer(data_, data_.size()),
			[self, callback](const boost::system::error_code& err, std::size_t bytes_transferred) {
				if (!err) {
					self->last_send_ = std::chrono::steady_clock::now();
					callback(err);
				}
				else {
					callback(err);
				}
			}
		);
	}
}

neg_server::neg_server(boost::asio::io_service& ios, short port, const std::string& file_name)
	: ios_(ios)
	, acceptor_(ios, tcp::endpoint(tcp::v4(), port))
	, data_loader_(file_name)
{
	auto new_ses = std::make_shared<session>(ios);
	acceptor_.async_accept(new_ses->get_socket(),
		[new_ses, this](const boost::system::error_code& err) {
			handle_accept(new_ses, err);
		}
	);
}

void neg_server::handle_accept(std::shared_ptr<session> ses, const boost::system::error_code& err) {
	if (!err) {
		std::cout << "client connected" << std::endl;
		start_transmition(ses);

		auto new_ses = std::make_shared<session>(ios_);
		auto self = shared_from_this();
		acceptor_.async_accept(new_ses->get_socket(),
			[new_ses, self](const boost::system::error_code& err) {
				self->handle_accept(new_ses, err);
			}
		);
	}
	else {
		std::cerr << "err: " + err.message() << std::endl;
	}
}

void neg_server::start_transmition(std::shared_ptr<session> ses) {
	using boost::asio::buffer;

	if (!data_loader_.size()) {
		return;
	}

	auto self = shared_from_this();
	ses->send_message(data_loader_.get_header(),
		[ses, self](const boost::system::error_code& err) {
			if (!err) {
				std::cout << "transmition started" << std::endl;
				self->on_transmition(ses, 0);
			}
			else {
				std::cerr << "err: " + err.message() << std::endl;
			}
		}
	);
}

void neg_server::on_transmition(std::shared_ptr<session> ses, std::size_t order) {
	using boost::asio::buffer;

	if (order > data_loader_.size()) {
		std::cout << "transmition ended" << std::endl;
		return;
	}

	std::string data;
	std::chrono::milliseconds lapse(0);
	if (order < data_loader_.size()) {
		data = data_loader_.get_line(order).line_;
		if (order) {
			lapse = data_loader_.get_line(order).timestamp_ - data_loader_.get_line(order - 1).timestamp_;
		}
	}
	else {
		data = data_loader_.get_trailer();
	}

	auto self = shared_from_this();
	ses->send_message(data, lapse,
		[ses, order, self](const boost::system::error_code& err) {
			if (!err) {
				self->on_transmition(ses, order + 1);
			}
			else {
				std::cerr << "err: " + err.message() << std::endl;
			}
		}
	);
}
