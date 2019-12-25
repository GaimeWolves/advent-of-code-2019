#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

std::vector<int> programMemory = std::vector<int>();
int rip = 0;

void step()
{
	switch(programMemory[rip])
	{
		case 1:
			programMemory[programMemory[rip + 3]] = programMemory[programMemory[rip + 1]] + programMemory[programMemory[rip + 2]];
			break;
		case 2:
			programMemory[programMemory[rip + 3]] = programMemory[programMemory[rip + 1]] * programMemory[programMemory[rip + 2]];
			break;
		case 99:
			std::cout << "Programm finished. Value at 0 " << programMemory[0] << std::endl;
			exit(0);
			break;
		default:
			std::cerr << "Invalid opcode at " << rip << std::endl;
			exit(-1);
			break;
	}

	rip += 4;
}

void patch()
{
	programMemory[1] = 12;
	programMemory[2] = 2;
}

int main(int argc, char** argv)
{
	std::ifstream file("in.txt");

	std::string line;
	while (std::getline(file, line))
	{
		std::string intCodeLine;
		std::istringstream iss(line);

		while(std::getline(iss, intCodeLine, ','))
		{
			int intCode = std::stoi(intCodeLine);
			programMemory.push_back(intCode);
		}
	}

	patch();
	while(true) step();

	return 0;
}