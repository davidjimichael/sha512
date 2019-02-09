// main.cpp

#include "SHA512.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <iterator>
#include <vector>
#include <bitset>
#include <numeric>
#include <cstring>

int main(int argc, const char * argv[]) {
	std::ifstream input("uhall.jpg", std::ios::binary);

	// copies all data into buffer
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

	// prints buffer to console
	std::string s, msg;
	for (auto const& s : buffer) { msg += s; }
		std::cout << msg;

	std::string hashedMsg = hash(msg);
	std::cout << "Hash:" << std::endl << hashedMsg;
	std::cin.get();
}

std::string hash(std::string msg) {
	//Preprocessing 
	const unsigned char eighty = 0x80;
	uint64_t msgLength = msg.length() * 8;
	uint64_t msgBitSize = __builtin_bswap64(msgLength);
	uint64_t k = 1024 - ((msgLength + 64 + 1) % 1024);
	uint64_t finalSize = msgLength + 1 + k + 64;
	uint64_t hashValues[] = {
		0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 
        0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
	};
	std::vector<uint64_t> words(finalSize / 64);
	memcpy(words.data(), msg.c_str(), msg.length());
	memcpy((unsigned char *)words.data() + msg.length(), &eighty, 1);
	memcpy((unsigned char *)words.data() + msg.length() + 1 + ((k - 7) / 8), &msgBitSize, 8);
	unsigned char* byte = (unsigned char*)words.data();
	for (int i = 0; i < words.size() * 8; i++) {
		std::bitset<8> b(byte[i]);
		std::cout << b << " " << std::hex << std::setfill('0') << std::setw(2) << (int)byte[i] << std::endl;
	}
	// Processing
	uint64_t workingValues[8];
	uint64_t a;
	uint64_t b;
	uint64_t c;
	uint64_t d;
	uint64_t e;
	uint64_t f;
	uint64_t g;
	uint64_t h;
	uint64_t temp1;
	uint64_t temp2;
	for (int chunk = 0; chunk < words.size(); chunk += 16) {
		std::vector<uint64_t> schedule(80);
		for (int i = 0; i < 16; i++) {
			schedule[i] = __builtin_bswap64(words[chunk + i]);
		}
		for (int i = 16; i < 80; i++) {
			schedule[i] = (s1(schedule[i - 2]) + schedule[i - 7] + s0(schedule[i - 15]) + schedule[i - 16]);
		}
		for (int i = 0; i < 8; i++) {
			workingValues[i] = hashValues[i];
		}
		a = workingValues[0];
		b = workingValues[1];
		c = workingValues[2];
		d = workingValues[3];
		e = workingValues[4];
		f = workingValues[5];
		g = workingValues[6];
		h = workingValues[7];
		for (int t = 0; t < 80; t++) {
			temp1 = (h + S1(e) + ch(e, f, g) + sha::words[t] + schedule[t]);
			temp2 = (S0(a) + maj(a, b, c));
			h = g;
			g = f;
			f = e;
			e = (d + temp1);
			d = c;
			c = b;
			b = a;
			a = (temp1 + temp2);
		}
		hashValues[0] = (hashValues[0] + a);
		hashValues[1] = (hashValues[1] + b);
		hashValues[2] = (hashValues[2] + c);
		hashValues[3] = (hashValues[3] + d);
		hashValues[4] = (hashValues[4] + e);
		hashValues[5] = (hashValues[5] + f);
		hashValues[6] = (hashValues[6] + g);
		hashValues[7] = (hashValues[7] + h);
	}
	// Return final message
	std::stringstream ss;
	for (int i = 0; i < 8; i++) {
		ss << std::hex << hashValues[i];
	}
	return ss.str();
}