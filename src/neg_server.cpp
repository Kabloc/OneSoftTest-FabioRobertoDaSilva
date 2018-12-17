#include <neg_server.h>
#include <functional>
#include <memory>
#include <iostream>
#include <boost/asio.hpp>

void session::send_message(const std::string& data, std::function<void(const boost::system::error_code& err, std::size_t bytes_transferred)> callback) {
	using boost::asio::buffer;

	bytes_transferred_ = boost::asio::write(socket_, buffer(data, data.size()), err_);
	last_send_ = std::chrono::system_clock::now();
	auto self = shared_from_this();
	ios_.post([self, callback]() { callback(self->err_, self->bytes_transferred_); });
}

neg_server::neg_server(boost::asio::io_service& ios, short port, const std::string& file_name)
	: ios_(ios)
	, acceptor_(ios, tcp::endpoint(tcp::v4(), port))
	, data_loader_(file_name)
	, timer_(ios)
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
		[ses, self](const boost::system::error_code& err, std::size_t bytes_transferred) {
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
		if (order + 1 <= data_loader_.size()) {
			lapse = data_loader_.get_line(order + 1).timestamp_ - data_loader_.get_line(order).timestamp_;
		}
	}
	else {
		data = data_loader_.get_trailer();
	}

	auto self = shared_from_this();
	ses->send_message(data,
		[ses, order, self, lapse](const boost::system::error_code& err, std::size_t bytes_transferred) {
			if (!err) {
				if (lapse.count() > 0) {
					auto lapse_now = std::chrono::system_clock::now() - ses->get_last_send() + lapse;
					boost::posix_time::milliseconds wait_for(lapse_now.count());
					self->timer_.expires_from_now(boost::posix_time::seconds(1));
					self->timer_.async_wait(
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
				else {
					self->on_transmition(ses, order + 1);
				}
			}
			else {
				std::cerr << "err: " + err.message() << std::endl;
			}
		}
	);
}
