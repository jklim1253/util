#include <iostream>
#include <fstream>
#include <vector>
#include <exception>
#include <string>
#include <sstream>
#include <iomanip>
using namespace std;

struct filesize {
	static constexpr const char* const unit[] = {".", "B", "KB", "MB", "GB", "TB"};
	string operator()(size_t n) {
		size_t u = 0;
		double prev = n*1.;
		while (n > 0) {
			prev /= 1024.;
			n /= 1024;
			u++;
		}
		ostringstream os;
		os << setprecision(4) << (prev*1024.) << " " << unit[u];
		return os.str();
	}
};
constexpr const char* const filesize::unit[];

struct function_check {
	const char* function_name;
	function_check(const char* fn) : function_name(fn) {
		cout << function_name << "(";
	}
	~function_check() {
		cout << ") " << endl;
	}
};

class File {
private :
	fstream file;
public :
	File() {
	}
	File(const char* filename) {
		open(filename);
	}
	~File() {
		file.close();
	}
	bool open(const char* filename) {
		function_check fc("File::open");
		file.exceptions(fstream::failbit | fstream::badbit);
		try {
			file.open(filename, ios_base::binary | ios_base::in);
		}
		catch (fstream::failure& e) {
			throw e;
		}
		return true;
	}
	size_t size() {
		function_check fc("File::size");
		fstream::pos_type p = 0;
		try {
			file.seekg(0, ios_base::end);
			p = file.tellg();
			file.seekg(0, ios_base::beg);
		}
		catch (exception& e) {
			throw e;
		}
		return p;
	}
};

int main(int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "Usage : test [filename]" << endl;
		return 1;
	}
	try {
		File f(argv[1]);

		filesize fs;
		size_t s = f.size();
		cout << "file size : " << s << endl;
		cout << "friendly file size : " << fs(s) << endl;
	}
	catch (exception& e) {
		cerr << "error : " << e.what() << endl;
	}

	return 0;
}
