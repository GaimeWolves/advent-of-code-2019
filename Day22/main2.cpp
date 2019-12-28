#include <iostream>
#include <fstream>
#include <sstream>

#include <deque>
#include <vector>
#include <algorithm>

#include <boost/multiprecision/cpp_int.hpp>

typedef boost::multiprecision::int128_t int128_t;

static inline int128_t mod(int128_t a, int128_t b)
{
	return (a >= 0) ? (a % b) : (b + a % b);
}

static int128_t gcd_extended(int128_t a, int128_t b, int128_t& x, int128_t& y)
{
	if (a == 0) 
	{
		x = 0;
		y = 1;
		return b;
	}

	int128_t x1, y1;
	int128_t gcd = gcd_extended(b % a, a, x1, y1);
	x = y1 - (b / a) * x1;
	y = x1;
	return gcd;
}

static int128_t modinv(int128_t b, int128_t n)
{
	int128_t x, y;
	int128_t g = gcd_extended(b, n, x, y);
	return (g != 1) ? -1 : mod(x, n);
}

static int128_t moddiv(int128_t a, int128_t b, int128_t n)
{
	a = mod(a, n);
	int128_t inv = modinv(b, n);
	return (inv == -1) ? -1 : (a * inv) % n;
}

int128_t modexp(int128_t base, int128_t exponent, int128_t n)
{
	assert(exponent >= 0);
	if (exponent == 0)
		return (base == 0) ? 0 : 1;

	int128_t bit = 1;
	int128_t power = mod(base, n);
	int128_t out = 1;
	while (bit <= exponent)
	{
		if (exponent & bit)
			out = mod(out * power, n);

		power = mod(power * power, n);
		bit <<= 1;
	}

	return out;
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
		int128_t arg;
		int128_t a;
		int128_t b;

		if (line.find("new") != std::string::npos)
		{
			a = -1;
			b = size - 1;
		}
		else if (line.find("with") != std::string::npos)
		{
			arg = (int128_t)std::stoll(line.substr(19));
			a = arg;
			b = 0;
		}
		else if (line.find("cut") != std::string::npos)
		{
			arg = (int128_t)std::stoll(line.substr(3));
			if (arg < 0)
				arg += size;

			a = 1;
			b = size - arg;
		}

		inc = mod(a * inc, size);
		off = mod(a * off + b, size);
	}

	off = mod(off * moddiv(modexp(inc, count, size) - 1, inc - 1, size), size);
	inc = modexp(inc, count, size);

	std::cout << mod(moddiv(mod(2020 - off, size), inc, size), size);
	std::cin.get();

	return 0;
}