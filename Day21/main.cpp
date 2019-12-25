#include <iostream>
#include <fstream>
#include <sstream>

#include "computer.hpp"

std::vector<int64_t> ram = std::vector<int64_t>();

std::string part1[] = {
	"NOT A J\n",
	"NOT B T\n",
	"OR T J\n",
	"NOT C T\n",
	"OR T J\n",
	"AND D J\n",
	"WALK\n"
};

std::string part2[] = {
	"NOT A J\n"

	"NOT C T\n"
	"AND H T\n"
	"OR T J\n"

	"NOT B T\n"
	"AND A T\n"
	"AND C T\n"
	"AND D T\n"
	"OR T J\n"

	"AND D J\n"

	"RUN\n"
};

IntCodePC pc;

void sendString(std::string str)
{
	std::cout << str;
	for (auto chr : str)
		pc.in((int64_t)chr);
}

std::string getString()
{
	std::string out = "";
	while(true)
	{
		auto chr = pc.out();

		if (chr > 255)
			throw std::runtime_error(std::to_string(chr));

		out += (char)chr;
		if (chr == '\n')
			break;
	}
	return out;
}

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
		exit(-1);
	}

	std::ifstream file(argv[1]);

	std::string line;
	while (std::getline(file, line))
	{
		std::string intCodeLine;
		std::istringstream iss(line);

		while(std::getline(iss, intCodeLine, ','))
		{
			int64_t intCode = std::stoll(intCodeLine);
			ram.push_back(intCode);
		}
	}

	pc.reload(ram);
	pc.reset();
	pc.run();

	std::cout << getString();

	for (auto instr : part2)
		sendString(instr);

	while(true)
		std::cout << getString();
	
	pc.waitForExit();

	return 0;
}