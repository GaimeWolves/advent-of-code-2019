#include <iostream>
#include <fstream>
#include <sstream>

#include "computer.hpp"

std::vector<int> ram = std::vector<int>();

IntCodePC pc;

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
			int intCode = std::stoi(intCodeLine);
			ram.push_back(intCode);
		}
	}

	pc.reload(ram);
	pc.reset();
	pc.run();
	pc.in(10);
	std::cout << pc.out() << std::endl;
	pc.waitForExit();

	return 0;
}