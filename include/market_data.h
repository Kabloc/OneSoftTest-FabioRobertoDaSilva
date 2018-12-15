#pragma once

#include <vector>
#include <exception>

namespace market_data {

	typedef struct {
		char line_[238];
		char timestamp_[15];
	} market_line;

	class loader {
	public:
		loader(std::string file_name);
		std::string get_header() const;
		std::string get_trailer() const;

	private:
		void process_line(const std::string& line);

		std::string file_name_;
		std::vector<market_line> data_;
	};
}