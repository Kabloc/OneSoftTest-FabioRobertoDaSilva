#include <market_data.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <algorithm>
#include <iomanip>
#include <memory>

namespace market_data {

// Loader constructor 
loader::loader(const std::string& file_name)
	: file_name_(file_name)
{
	// Read all file content
	std::ifstream is(file_name);
	char line[300];
	if (!is.is_open()) {
		throw std::runtime_error("error while opening file");
	}
	while (is.getline(line, 300)) {
		process_line(line);
	}
	if (is.bad()) {
		throw std::runtime_error("error while reading file");
	}

	// Sort all lines by trade id
	std::sort(data_.begin(), data_.end(),
		[](const trade_line& la, const trade_line& lb)->bool {
			return la.trade_id_ < lb.trade_id_;
		}
	);
}

// Trade line access
trade_line loader::get_line(std::size_t line_number) const {
	if (line_number >= data_.size()) {
		throw std::runtime_error("data_ vector out of range");
	}
	return data_[line_number];
}

// Header creation
std::string loader::get_header() const {
	std::stringstream ss;

	ss 
		<< "RH;"                                              //Identification
		<< std::setw(20) << std::left << file_name_ << ";"    //Name of file
		<< data_[0].line_.substr(0, 10) << ";"                //Initial date
		<< data_[data_.size() - 1].line_.substr(0, 10) << ";" //End date
		<< "000000000";                                       //Total of lines
	return ss.str();
}

// Trailer creation
std::string loader::get_trailer() const {
	std::stringstream ss;

	ss 
		<< "RT;"                                                            //Identification
		<< std::setw(20) << std::left << file_name_ << ";"                  //Name of file
		<< data_[0].line_.substr(0, 10) << ";"                              //Initial date
		<< data_[data_.size() - 1].line_.substr(0, 10) << ";"               //End date
		<< std::setw(9) << std::setfill('0') << std::right << data_.size(); //Total of lines
	return ss.str();
}

std::size_t loader::size() const {
	return data_.size();
}

// Line process
void loader::process_line(const std::string& line) {
	if (line.size() != 237) {
		throw std::runtime_error("wrong line size");
	}

	// Create the line information and add to vector
	data_.emplace_back(
		trade_line
		{
			std::atoi(line.substr(62, 10).c_str()),
			string_to_milliseconds(line.substr(113, 12)),
			line 
		}
	);
}

std::chrono::milliseconds loader::string_to_milliseconds(const std::string& time) {
	int h = 0, m = 0, s = 0, ms = 0;
	std::sscanf(time.c_str(), "%d:%d:%d.%d", &h, &m, &s, &ms);
	auto dms = std::chrono::milliseconds(ms);
	dms += std::chrono::milliseconds(std::chrono::seconds(s));
	dms += std::chrono::milliseconds(std::chrono::minutes(m));
	dms += std::chrono::milliseconds(std::chrono::hours(h));

	return dms;
}

}