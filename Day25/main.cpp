#include <iostream>
#include <fstream>
#include <sstream>

#include "computer.hpp"

std::vector<int64_t> ram = std::vector<int64_t>();

std::string items[] = {
	"whirled peas",
	"planetoid",
	"mouse",
	"klein bottle",
	"mutex",
	"dark matter",
	"antenna",
	"fuel cell"
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
	std::cout << out;
	return out;
}

void readAll(std::string& last)
{
	last = "";
	while(last != "Command?\n")
		last = getString();
}

void takeItem(int item)
{
	std::string take = "take ";
	take += items[item];
	take += '\n';
	sendString(take);
}

void dropItem(int item)
{
	std::string take = "drop ";
	take += items[item];
	take += '\n';
	sendString(take);
}

void bruteForce()
{
	std::string last = "";
	for (int i = 0; i < 256; i++)
	{
		if (i & (1 << 0)) takeItem(0); else dropItem(0); readAll(last);
		if (i & (1 << 1)) takeItem(1); else dropItem(1); readAll(last);
		if (i & (1 << 2)) takeItem(2); else dropItem(2); readAll(last);
		if (i & (1 << 3)) takeItem(3); else dropItem(3); readAll(last);
		if (i & (1 << 4)) takeItem(4); else dropItem(4); readAll(last);
		if (i & (1 << 5)) takeItem(5); else dropItem(5); readAll(last);
		if (i & (1 << 6)) takeItem(6); else dropItem(6); readAll(last);
		if (i & (1 << 7)) takeItem(7); else dropItem(7); readAll(last);
		sendString("north\n");
		readAll(last);
		if (last != "Command?\n")
			break;
	}
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

	while(true)
	{
		if (pc.hasOutput())
			getString();
		
		if (pc.awaitsInput())
		{
			std::string cmd;
			std::getline(std::cin, cmd);

			if (cmd == "brute")
				bruteForce();
			else
			{
				cmd += '\n';
				sendString(cmd);
			}
		}
	}

	return 0;
}