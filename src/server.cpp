#include <neg_server.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

	auto print_usage = [](){ cerr << "Usage: neg_file_server market_data_file.txt [port]" << endl; };

	if ((argc < 2) || (argc > 3)) {
		print_usage();
		return -1;
	}

	try {
		int port = 8000;
		if (argc == 3) {
			port = atoi(argv[2]);
			if (!port) {
				print_usage();
				return -1;
			}
		}
		boost::asio::io_service ios;
		auto server = make_shared<neg_server>(ios, port, argv[1]);
		ios.run();
	}
	catch (exception &e) {
		cerr << e.what() << endl;
	}
	return 0;
}

