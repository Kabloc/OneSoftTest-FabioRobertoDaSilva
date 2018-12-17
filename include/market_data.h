#pragma once

#include <vector>
#include <exception>
#include <chrono>
#include <string>

namespace market_data {

	typedef struct {
		std::string line_;
		std::chrono::milliseconds timestamp_;
	} market_line;

	class loader {
	public:
		loader(const std::string& file_name);
		market_line get_line(std::size_t line_number) const;
		std::string get_header() const;
		std::string get_trailer() const;
		std::size_t size() const;

	private:
		void process_line(const std::string& line);
		std::chrono::milliseconds string_to_milliseconds(const std::string& time);

		std::string file_name_;
		std::vector<market_line> data_;
	};
}