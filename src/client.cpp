#include <string>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: client_test <host> <port>" << std::endl;
			return 1;
		}

		boost::asio::io_context io_context;

		std::string buffer;
		std::string delim("\r\n");
		tcp::socket s(io_context);
		tcp::resolver resolver(io_context);
		boost::system::error_code err;
		boost::asio::connect(s, resolver.resolve(argv[1], argv[2]));

		while (1) {
			boost::asio::read_until(s, boost::asio::dynamic_buffer(buffer), delim, err);

			if (err)
				break;

			size_t pos = buffer.find(delim);
			if (pos == std::string::npos) {
				continue;
			}
			std::string received = buffer.substr(0, pos);
			std::cout << received << std::endl;
			buffer.erase(0, received.size() + delim.size());
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	return 0;
}