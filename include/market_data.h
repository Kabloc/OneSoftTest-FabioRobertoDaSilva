#pragma once
#include <vector>
#include <exception>
#include <chrono>
#include <string>

namespace market_data {

	struct trade_line {
		int trade_id_;
		std::chrono::milliseconds timestamp_;
		std::string line_;
	};

	class loader {
	public:
		loader(const std::string& file_name);
		trade_line get_line(std::size_t line_number) const;
		std::string get_header() const;
		std::string get_trailer() const;
		std::size_t size() const;

	private:
		void process_line(const std::string& line);
		std::chrono::milliseconds string_to_milliseconds(const std::string& time);

		std::string file_name_;
		std::vector<trade_line> data_;
	};
}