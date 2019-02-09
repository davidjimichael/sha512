// main.cpp

#include "SHA512.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <algorithm>
#include <fstream>
#include <iterator>
#include <vector>
#include <bitset>
#include <numeric>
#include <cstring>

int main(int argc, const char *argv[]) {
	if (argc != 2) {
		std::cout << "Must include file path\n";
		return 1;
	}

	// read file binary
	std::ifstream input(argv[1], std::ios::binary);

	// copy binary to buffer
	std::vector<unsigned char> buffer(std::istreambuf_iterator<char>(input), {});

	std::string s, msg;
	for (auto const& s : buffer) { 
		msg += s; 
	}
	msg = "abc";
	std::string hashedMsg = hash(msg);
	std::cout << "SHA512:" << std::endl << hashedMsg;
	std::cout << "Press any key to continue...\n";
	std::cin.get();
}

// preprocessing
std::vector<uint64_t> pad(std::string msg) {
	const unsigned char eighty = 0x80;
	uint64_t msgLength = msg.length() * 8;
	uint64_t msgBitSize = __builtin_bswap64(msgLength);
	uint64_t k = 1024 - ((msgLength + 64 + 1) % 1024);
	uint64_t finalSize = msgLength + 1 + k + 64;
	
	std::vector<uint64_t> words(finalSize / 64);

	// copy msg, add padding
	memcpy(words.data(), msg.c_str(), msg.length());
	memcpy((unsigned char *)words.data() + msg.length(), &eighty, 1);
	memcpy((unsigned char *)words.data() + msg.length() + 1 + ((k - 7) / 8), &msgBitSize, 8);
	
	return words;
}

std::string hash(std::string msg) {
	uint64_t h[] = {
		0x6a09e667f3bcc908, 0xbb67ae8584caa73b, 0x3c6ef372fe94f82b, 0xa54ff53a5f1d36f1, 
        0x510e527fade682d1, 0x9b05688c2b3e6c1f, 0x1f83d9abfb41bd6b, 0x5be0cd19137e2179
	};
	std::vector<uint64_t> words = pad(msg);
	
	// Processing
	uint64_t a[8]; // a, b, c, d, e, f, g, h
	uint64_t t0, t1;
	for (int chunk = 0; chunk < words.size(); chunk += 16) {
		std::vector<uint64_t> schedule(80);

		for (int i = 0; i < 16; i++) {
			schedule[i] = __builtin_bswap64(words[chunk + i]);
		}
		for (int i = 16; i < 80; i++) {
			schedule[i] = (s1(schedule[i - 2]) + schedule[i - 7] + s0(schedule[i - 15]) + schedule[i - 16]);
		}
		
		std::copy(std::begin(h), std::end(h), std::begin(a));
		
		for (int t = 0; t < 80; t++) {
			t0 = (a[7] + S1(a[4]) + ch(a[4], a[5], a[6]) + sha::words[t] + schedule[t]);
			t1 = (S0(a[0]) + maj(a[0], a[1], a[2]));
			a[7] = a[6];
			a[6] = a[5];
			a[5] = a[4];
			a[4] = (a[3] + t0);
			a[3] = a[2];
			a[2] = a[1];
			a[1] = a[0];
			a[0] = (t0 + t1);
		}
		for (int i = 0; i < 8; i++) {
			h[i] = (h[i] + a[i]);
		}
	}
	
	return concatHash(h);
}

std::string concatHash(uint64_t h[]) {
	std::stringstream ss;

	// setw and setfill prevent truncating leading zeros
	for (int i = 0; i < 8; i++) {
		ss << std::setw(16) << std::setfill('0') << std::hex << h[i];
	}
	return ss.str();
}