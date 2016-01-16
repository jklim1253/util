#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <cctype>
#include <exception>
#include <boost/filesystem.hpp>
using namespace std;

struct error {
	static const int exit_normal = 0;
	static const int wrong_command_line = 1;
};
struct function_check {
	const string function_name;
	function_check(const string& fn) : function_name(fn) {
		cout << endl << function_name << "(";
	}
	~function_check() {
		cout << ") // " << function_name << endl;
	}
};
struct file_exist {
	const string filename;
	file_exist(const string& fn) : filename(fn) {}
	bool operator ()() {
		function_check fc("file_exist::operator()");
		fstream f(filename, ios_base::binary | ios_base::in);
		if (!f.good()) {
			return false;
		}
		return true;
	}
};
struct file_size {
	fstream file;
	const string filename;
	file_size(const string& fn) : filename(fn) { }
	streamsize operator ()() {
		if (!file_exist(filename)())
			return 0;

		streamsize size = 0;
		file.open(filename, ios_base::binary | ios_base::in);
		file.seekg(0, ios_base::end);
		size = file.tellg();
		file.close();

		return size;
	}
};
struct zeros {
	char* zero;
	zeros(streamsize& n) : zero(nullptr) {
		zero = new char[n];
		memset(zero, 0, sizeof(char)*n);
	}
	~zeros() {
		if (zero) {
			delete[] zero;
		}
	}
	char* operator()() {
		function_check fc("zeros::operator()");
		return zero;
	}
};

bool parse_command_line(int& argc, char**& argv) {
	function_check fc("parse_command_line");
	static const char* const usage = "Usage : tiny [files]";
	if (argc < 2) {
		cerr << usage << endl;
		return false;
	}
	--argc;
	++argv;

	return true;
}
void make_filelist(std::list<std::string>& filelist, int argc, char** argv) {
	function_check fc("make_filelist");
	// TODO : get current working directory.
	boost::filesystem::path path = boost::filesystem::current_path();
	while (argc--) {
		string filename = path.string() + "\\" + string(*argv++);
		if (file_exist(filename)()) {
			filelist.push_back(filename);
		}
	}
}
class checker {
	std::fstream file;
	const string filename;
public :
	checker(const string& fn) : filename(fn) {
		open(filename);
	}
	~checker() {
		close();
	}
	bool open(const string& filename) {
		function_check fc("checker::open");
		file.exceptions(fstream::failbit | fstream::badbit);
		try {
			file.open(filename, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
			if (!file.is_open()) {
				return false;
			}
		}
		catch (fstream::failure& e) {
			cerr << "checker::open throw" << endl;
			throw e;
		}
		catch (exception& e) {
			cerr << "checker::open throw" << endl;
			throw e;
		}

		return true;
	}
	void close() {
		function_check fc("checker::close");
		file.close();
	}
	int check() {
		function_check fc("checker::check");
		int count_changed = 0;

		typedef int (checker::*check_detail)(std::fstream&);

		check_detail detail[] = {&checker::trailing_whitespace,};

		try {
			for (auto& f : detail) {
				count_changed += (this->*f)(file);
				file.flush();
			}
		}
		catch (fstream::failure& e) {
			cerr << "checker::check throw" << endl;
			throw e;
		}
		catch (exception& e) {
			cerr << "checker::check throw" << endl;
			throw e;
		}

		return count_changed;
	}
	int trailing_whitespace(std::fstream& fs) {
		function_check fc("trailing_whitespace");
		streamsize size = file_size(filename)();
		std::fstream::pos_type current = 0, wc = 0;
		char buffer[size];
		bool found_ws = false;
		int count_changed = 0;

		try {
			current = fs.tellg();
			while (fs.good()) {
				streamsize buffer_size = 0;
				if (size >= 1024LL) {
					buffer_size = 1024LL;
					size -= 1024LL;
				}
				else {
					buffer_size = size-1;
				}
				streamsize bufferpos = fs.tellg();
				fs.read(buffer, buffer_size);
				cout << buffer_size << " read" << endl;

				// find trailing whitespace.
				streamsize index = 0;
				for (char& c : buffer) {
					if ((c == 0 || c == ' ' || c == '\t') && !found_ws) {
						cout << "whitespace found" << endl;
						found_ws = true;
						wc = bufferpos + index;
					}
					else if ((c == 0 || c == ' ' || c == '\t') && found_ws) {
						// nothing...
					}
					else if ((c == '\n' || c == '\r') && found_ws) {
						cout << "need to erase trailing whitespace" << endl;
						// erase latest whitespace position to current-1
						current = bufferpos + index;
						cout << "current : " << current << ", wc : " << wc << endl;
						streamsize erase_size = current - wc;
						zeros z(erase_size);
						fs.seekp(wc, ios_base::beg);
						fs.write(z(), erase_size);
						cout << erase_size << " erased" << endl;
						found_ws = false;
						++count_changed;
					}
					else if (found_ws) {
						cout << c << int(c) << endl;
						found_ws = false;
					}

					++index;
				}
			}
		}
		catch (fstream::failure& e) {
			cerr << "checker::trailing_whitespace throw" << endl;
			throw e;
		}
		catch (exception& e) {
			cerr << "checker::trailing_whitespace throw" << endl;
			throw e;
		}

		return count_changed;
	}
};

int main(int argc, char* argv[]) {
	function_check fc("main");
	// command line check
	if (!parse_command_line(argc, argv)) {
		return error::wrong_command_line;
	}
	int count_changed = 0;
	try {
		// check each input files.
		list<string> filelist;
		make_filelist(filelist, argc, argv);

		int i = 0;
		for (auto& filename : filelist) {
			cout << i << " : " << filename << endl;
			checker ch(filename);
			count_changed += ch.check();
		}
	}
	catch (exception& e) {
		cerr << "Error : " << e.what() << endl;
	}

	cout << count_changed << " changed" << endl;

	return error::exit_normal;
}
