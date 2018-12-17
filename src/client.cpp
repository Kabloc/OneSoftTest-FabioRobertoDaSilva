#include <string>
#include <iostream>
#include <sstream>
#include <boost/asio.hpp>

using boost::asio::ip::tcp;

int main(int argc, char* argv[])
{
	std::stringstream all;
	try
	{
		if (argc != 3)
		{
			std::cerr << "Usage: client_test <host> <port>" << std::endl;
			return 1;
		}

		boost::asio::io_context io_context;

		std::string reply;
		tcp::socket s(io_context);
		tcp::resolver resolver(io_context);
		boost::asio::connect(s, resolver.resolve(argv[1], argv[2]));

		while (1) {
			size_t reply_length = boost::asio::read_until(s,
				boost::asio::dynamic_buffer(reply), "\r\n");
			std::cout << reply << std::endl;
			reply.clear();
		}
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << std::endl;
	}
	std::cout << all.str();
	return 0;
}