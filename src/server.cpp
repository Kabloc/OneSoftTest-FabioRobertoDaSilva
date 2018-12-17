#include <neg_server.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

	if (argc <= 1) {
		cerr << "Usage: neg_file_server market_data_file.txt" << endl;
		return -1;
	}

	try {
		boost::asio::io_service ios;
		auto server = make_shared<neg_server>(ios, 8000, argv[1]);
		ios.run();
	}
	catch (exception &e) {
		cerr << e.what() << endl;
	}
	return 0;
}

