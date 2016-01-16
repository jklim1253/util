#include <iostream>
#include <fstream>
#include <string>
#include <list>
using namespace std;

struct file_exist {
	fstream file;
	file_exist(const string& fn) {
		file.open(fn, ios_base::binary | ios_base::in);
	}
	~file_exist() {
		file.close();
	}
	bool operator()() {
		return file.good();
	}
};
struct file_size {
	fstream file;
	file_size(const string& fn) {
		file.open(fn, ios_base::binary | ios_base::ate);
	}
	~file_size() {
		file.close();
	}
	fstream::pos_type operator()() {
		return file.tellg();
	}
};
template<streamsize SIZE>
struct block {
	char* contents;
	streamsize last;
	block() : contents(nullptr), last(0) {
		contents = new char[SIZE];
	}
	block(const block& other) : contents(nullptr), last(0) {
		contents = other.contents;
		last = other.last;
		const_cast<block&>(other).contents = nullptr;
	}
	~block() {
		if (contents) {
			delete[] contents;
		}
	}
	friend ostream& operator << (ostream& os, const block& obj) {
		os.write(obj.contents, obj.last);
		return os;
	}
	friend istream& operator >> (istream& is, block& obj) {
		cout << SIZE << "reading..." << endl;
		is.read(obj.contents, SIZE);
		obj.last = is.gcount();
		return is;
	}
};
struct memory {
	typedef block<1024> block_type;
	list<block_type> page;
};
int main(int argc, char* argv[]) {
	if (argc != 2) {
		cerr << "Usage : test [filename]" << endl;
		return 1;
	}
	if (!file_exist(argv[1])()) {
		cerr << argv[1] << " not found" << endl;
		return 2;
	}
	// open file and load file contents to memory.
	memory buffer;
	{
		fstream in;
		in.open(argv[1], ios_base::binary | ios_base::in);

		streamsize readcount = 0;
		while (in.good()) {
			memory::block_type blk;
			in >> blk;
			readcount += blk.last;
			cout << blk.last << " read." << endl;
			buffer.page.push_back(blk);
		}
		cout << readcount << " bytes loaded." << endl;
		in.close();
	}

	// edit contents on memory.
	{
		for (memory::block_type& blk : buffer.page) {
			cout << "about " << blk.last << " bytes." << endl;
			for (auto i = 0; i < blk.last; ) {
				cout << "current byte : " << blk.contents[i] << int(blk.contents[i]) << endl;
				if (blk.contents[i] == 'o') {
					cout << "o found." << endl;
					for (auto j = i; j < blk.last-1; ++j) {
						blk.contents[j] = blk.contents[j+1];
					}
					--blk.last;
				}
				else {
					++i;
				}
			}
			// check.
			cout << "check" << endl;
			for (auto i = 0; i < blk.last; ++i) {
				cout << blk.contents[i] << int(blk.contents[i]) << endl;
			}
		}
	}
	// save memory to file.
	{
		fstream out;
		out.open(argv[1], ios_base::binary | ios_base::trunc | ios_base::out);

		streamsize writecount = 0;
		for (memory::block_type& blk : buffer.page) {
			writecount += blk.last;
			out << blk;
		}
		cout << writecount << " bytes written to file." << endl;
		out.close();
	}

	return 0;
}
