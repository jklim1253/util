#include <iostream>
#include <fstream>
#include <string>
#include <list>
#include <cctype>
using namespace std;

struct error {
	static const int exit_normal = 0;
	static const int wrong_command_line = 1;
};

bool parse_command_line(int& argc, char**& argv) {
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
	// TODO : get current working directory.
	std::string path = ".\\";
	while (argc--) {
		filelist.push_back(path + std::string(*argv++));
	}
}
class checker {
public :
	int check(const string& filename) throw(...) {
		static const char* const not_found = " not found.";
		std::fstream file;
		file.open(filename, std::ios_base::binary | std::ios_base::in | std::ios_base::out);
		if (!file.good()) {
			cerr << filename << not_found << endl;
			return 0;
		}
		int count_changed = 0;

		typedef int (checker::*check_detail)(std::fstream&);

		check_detail detail[] = {&checker::trailing_whitespace,};

		for (auto& f : detail) {
			count_changed += (this->*f)(file);
		}

		return count_changed;
	}
	int trailing_whitespace(std::fstream& fs) {
		static const int size = 1024;
		std::fstream::pos_type current = 0, wc = 0, prev_wc = 0;
		char buffer[size];
		while (fs.read(buffer, size)) {
			// find trailing whitespace.
//			for (char& c : buffer) {
//				if (std::isspace(c)) {
//				}
//				else {
//				}
//			}

			current += size;
		}

		return 0;
	}
};

int main(int argc, char* argv[]) {
	// command line check
	if (!parse_command_line(argc, argv)) {
		return error::wrong_command_line;
	}
	// check each input files.
	list<string> filelist;
	make_filelist(filelist, argc, argv);

	checker ch;
	for (auto& filename : filelist) {
		ch.check(filename);
	}
	
	return error::exit_normal;
}