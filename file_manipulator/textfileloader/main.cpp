#include <iostream>
#include <tchar.h>
#include <fstream>
#include <string>
#include <iomanip>
#include <conio.h>
using namespace std;

class textfileloader {
public :
	enum FILEENC {
		FE_UNKNOWN,
		FE_ANSI,
		FE_UTF8,
		FE_UTF16LE,
		FE_UTF16BE,
		FE_MAX
	};
public :
	FILEENC encoding(const string& filename) {
		FILEENC ret = FE_UNKNOWN;

		fstream file;
		
		try {

			file.open(filename, ios_base::in | ios_base::binary);
			if (!file.good()) {
				return FE_UNKNOWN;
			}
			unsigned char buffer[3];
			file.read((char*)buffer, 3);

			cout << hex << uppercase;
			for (int i = 0; i < 3; i++) {
				cout << setw(2) << setfill('0') << (int)buffer[i] << ' ';
			}
			cout << endl;

			// BOM check
			unsigned char BOM[3][3] = { {0xFF, 0xFE},			// utf-16 little endian
										{0xFE, 0xFF},			// utf-16 big endian
										{0xEF, 0xBB, 0xBF} };	// utf-8
			if (::memcmp(BOM[0], buffer, sizeof(unsigned char) * 2) == 0) {
				ret = FE_UTF16LE;
			}
			else if (::memcmp(BOM[1], buffer, sizeof(unsigned char) * 2) == 0) {
				ret = FE_UTF16BE;
			}
			else if (::memcmp(BOM[2], buffer, sizeof(unsigned char) * 3) == 0) {
				ret = FE_UTF8;

				if (_is_valid_utf8(file)) {
					cout << "yes utf8 encoding." << endl;
				}
				else {
					cout << "no something wrong." << endl;
				}
			}
			// head part of file have no BOM bytes.
			// so, need to check all bytes.
			else {
				// move beginning of file.
				file.seekg(0);

				const streamsize buffersize = 100;
				unsigned char buffer[buffersize];
				streamsize readcount = 0;
				while (file.good()) {
					file.read((char*)buffer, buffersize);

					// print data as hex as much as read.
					readcount = file.gcount();
					cout << hex << uppercase;
					for (streamsize i = 0; i < readcount; i++) {
						cout << setw(2) << setfill('0') << (int)buffer[i] << ' ';
						if ((i + 1) % 10 == 0) cout << endl;
					}
				}
				file.clear();
				cout << "\nEOF" << endl;

				if (_is_valid_utf8(file)) {
					ret = FE_UTF8;
				}
				else if (_is_valid_utf16(file)) {
					ret = FE_UTF16BE;
				}
				else {
					ret = FE_ANSI;
				}
			}
		}
		catch (exception e) {
			cerr << e.what() << endl;
		}

		file.close();

		return ret;
	}
	//FILEENC encoding(const wstring& filename);
private :
	// TODO : UTF-16 encoding �ڷ� ���� �� �����ؾ���.
	bool _is_valid_utf16(fstream& file) {
		return false;
	}
	bool _is_valid_utf8(fstream& file) {
/*
Char. number range  |        UTF-8 octet sequence
   (hexadecimal)    |              (binary)
--------------------+---------------------------------------------
0000 0000-0000 007F | 0xxxxxxx
0000 0080-0000 07FF | 110xxxxx 10xxxxxx
0000 0800-0000 FFFF | 1110xxxx 10xxxxxx 10xxxxxx
0001 0000-0010 FFFF | 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx

byte < 0x80
0xC0 <= 1st byte < 0xE0 && 0x80 <= 2nd byte < 0xC0
0xE0 <= 1st byte < 0xF0 && 0x80 <= 2nd byte < 0xC0 && 0x80 <= 3rd byte < 0xC0
0xF0 <= 1st byte < 0xF8 && 0x80 <= 2nd byte < 0xC0 && 0x80 <= 3rd byte < 0xC0 && 0x80 <= 4th byte < 0xC0
*/
		const unsigned char masks[4][4] = {
			{0x80,0},
			{0xE0,0xC0,0},
			{0xF0,0xC0,0xC0,0},
			{0xF8,0xC0,0xC0,0xC0}
		};
		const unsigned char valid[4][4] = {
			{0x00,0},
			{0xC0,0x80,0},
			{0xE0,0x80,0x80,0},
			{0xF0,0x80,0x80,0x80}
		};

		// move read position to beginning of file.
		file.seekg(0);
		const streamsize buffersize = 100;
		streamsize readsize;
		streamsize pos = 0;
		unsigned char buffer[buffersize];

		while (file.good()) {
			pos = 0;
			// readsize ��ŭ �о���δ�.(readsize <= buffersize)
			readsize = file.read((char*)buffer, buffersize).gcount();

			cout << "\n============================" << endl;
			cout << hex << uppercase;
			for (streamsize i = 0; i < readsize; i++) {
				cout << setw(2) << setfill('0') << (int)buffer[i] << ' ';
				if ((i + 1) % 10 == 0) {
					cout << endl;
				}
			}
			cout << endl;

			bool check_valid = true;
			while (pos < readsize) {

				check_valid = false;

				// �� ����Ʈ�� ��ȿ�� üũ�� �Ѵ�.
				for (int i = 0; i < 4; i++) {
					// �о���� ���ۿ��� ���� ����Ʈ�� ũ�Ⱑ 
					// ��ȿ�� üũ�� ����Ʈ���� ũ�ų� ����� �񱳰� �����ϴ�.
					if (pos + i + 1 <= readsize) {
						// ��ȿ�� üũ
						if (_mask_byte_array(masks[i], buffer + pos, valid[i], i + 1)) {
							// ��ȿ�ϸ� ���� ��ŭ ������ ��ġ�� �̵�.
							pos += i + 1;

							check_valid = check_valid || true;

							break;
						}
						else {
							// ���� ����Ʈ �������� ��ȿ���� ������,
							// �ٸ� ũ��� ���غ���.
							check_valid = check_valid || false;

							continue;
						}
					}
					// �о���� ���ۿ��� ���� ����Ʈ�� ũ�Ⱑ ���ٸ�
					// ���� ���۸� �о�鿩�� �Ѵ�.
					// ���� ���� ������ ũ�⸸ŭ�� �ٽ� �о�� �ϹǷ�
					// �б�� ���� �����͸� �ڰ��� �Ѵ�.
					else {
						file.seekg(pos - readsize, ios_base::cur);

						// ��ȿ�� üũ�� ����.
						check_valid = false;

						break;
					}
				}

				// ��ȿ���� ���� ����Ʈ �������� ã���� ���� ����.
				if (!check_valid) {
					break;
				}
			}
		}

		return true;
	}
	bool _mask_byte_array(const unsigned char* mask,
		const unsigned char* data,
		const unsigned char* valid,
		unsigned int size) {

		for (unsigned int i = 0; i < size; i++) {
			if ((mask[i] & data[i]) != valid[i]) {
				return false;
			}
		}

		cout << "[";
		cout << hex << uppercase;
		for (unsigned int i = 0; i < size; i++) {
			cout << setw(2) << setfill('0') << (int)data[i];
			if ((i + 1) % size != 0) {
				cout << ' ';
			}
		}
		cout << "] ";

		return true;
	}
	void print_bytes(const unsigned char* data, unsigned size) {
		cout << hex << uppercase;
		for (unsigned i = 0; i < size; i++) {
			cout << setw(2) << setfill('0') << data[i];
		}
	}
};
int main(void) {

	// read filename from target.txt.
	fstream target;
	target.open("target.txt", ios_base::in);
	if (!target.good()) {
		cerr << "target file not found." << endl;
		return 1;
	}
	string targetfilename;
	getline(target, targetfilename);

	target.close();

	// check target file encoding.
	const char *szFileEncode[textfileloader::FE_MAX] = { "Unknown encoding",
	"ANSI", "UTF8", "UTF16 Little endian", "UTF16 Big endian" };
	textfileloader loader;
	cout << targetfilename << " encoding is ";
	cout << szFileEncode[loader.encoding(targetfilename)] << endl;

	::_getch();

	return 0;
}