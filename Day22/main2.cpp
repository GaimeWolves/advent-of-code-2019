#include <iostream>
#include <fstream>
#include <sstream>

#include <deque>
#include <vector>
#include <algorithm>

#include <boost/multiprecision/cpp_int.hpp>

boost::multiprecision::int128_t modexp(boost::multiprecision::int128_t base, boost::multiprecision::int128_t exp, boost::multiprecision::int128_t mod)
{
	boost::multiprecision::int128_t res = 1;
	while (exp > 0) {
		if (exp % 2 == 1)
			res = (res * base) % mod;
		exp = exp >> 1;
		base = (base * base) % mod;
	}
	return res;
}

boost::multiprecision::int128_t modinv(boost::multiprecision::int128_t n, boost::multiprecision::int128_t mod)
{
	return modexp(n, mod - 2, mod);
}

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	boost::multiprecision::int128_t size = 119315717514047;
	boost::multiprecision::int128_t count = 101741582076661;

	boost::multiprecision::int128_t inc = 1;
	boost::multiprecision::int128_t off = 0;

	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("new") != std::string::npos) { inc *= -1; off += inc; };
		if (line.find("with") != std::string::npos) inc *= modinv(std::stoll(line.substr(19)), size);
		if (line.find("cut") != std::string::npos) off += inc * std::stoll(line.substr(3));
	}

	off = off * (1 - modexp(inc, count, size)) * modinv(1 - inc, size);
	inc = modexp(inc, count, size);

	std::cout << (off + 2020 * inc) % size;
	std::cin.get();

	return 0;
}