#include <market_data.h>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>
#include <memory>

namespace market_data {

loader::loader(std::string file_name)
	: file_name_(file_name)
{
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
}

std::string loader::get_header() const {
	std::string first_line(data_[0].line_);
	std::string last_line(data_[data_.size() - 1].line_);
	std::stringstream ss;

	ss 
		<< "RH;"                                           //Identification
		<< std::setw(20) << std::left << file_name_ << ";" //Name of file
		<< first_line.substr(0, 10) << ";"                 //Initial date
		<< last_line.substr(0, 10) << ";"                  //End date
		<< "000000000";                                    //Total of lines
	return ss.str();
}

std::string loader::get_trailer() const {
	std::string first_line(data_[0].line_);
	std::string last_line(data_[data_.size() - 1].line_);
	std::stringstream ss;

	ss 
		<< "RT;"                                                             //Identification
		<< std::setw(20) << std::left << file_name_ << ";"                   //Name of file
		<< first_line.substr(0, 10) << ";"                                   //Initial date
		<< last_line.substr(0, 10) << ";"                                    //End date
		<< std::setw(9) << std::setfill('0') << std::right << data_.size();  //Total of lines
	return ss.str();
}

void loader::process_line(const std::string& line) {
	if (line.size() > 237) {
		throw std::runtime_error("wrong line size");
	}
	market_line mkt_line;
	strncpy(mkt_line.line_, line.c_str(), sizeof(mkt_line.line_));
	strncpy(mkt_line.timestamp_, line.substr(113, 12).c_str(), sizeof(mkt_line.timestamp_));
	data_.push_back(mkt_line);
}
}