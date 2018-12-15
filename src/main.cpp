#include <market_data.h>
#include <iostream>
#include <string>

using namespace std;

int main(int argc, char* argv[]) {

	if (argc <= 1) {
		cerr << "Usage: application market_data_file.txt" << endl;
		return -1;
	}

	try {
		market_data::loader loader(argv[1]);
		cout << loader.get_header() << endl;
		cout << loader.get_trailer() << endl;
	}
	catch (exception &e) {
		cerr << e.what() << endl;
	}
	return 0;
}

