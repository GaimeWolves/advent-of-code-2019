#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

std::vector<int> programMemory = std::vector<int>();
std::vector<int> programRam = std::vector<int>();
int rip = 0;

bool step()
{
	switch(programMemory[rip])
	{
		case 1: // ADD: 1, a1, a2, a3: mem[a3] = mem[a1] + mem[a2]
			programMemory[programMemory[rip + 3]] = programMemory[programMemory[rip + 1]] + programMemory[programMemory[rip + 2]];
			rip += 4;
			break;
		case 2: // MUL: 1, a1, a2, a3: mem[a3] = mem[a1] * mem[a2]
			programMemory[programMemory[rip + 3]] = programMemory[programMemory[rip + 1]] * programMemory[programMemory[rip + 2]];
			rip += 4;
			break;
		case 99: // HLT
			rip += 1;
			return false;
			break;
		default:
			std::cerr << "Invalid opcode at " << rip << std::endl;
			exit(-1);
			break;
	}

	return true;
}

void run()
{
	while(step());
}

void reset()
{
	rip = 0;
	programMemory = programRam;
}

void tryPatches()
{
	for (int noun = 0; noun < 100; noun++)
	{
		for (int verb = 0; verb < 100; verb++)
		{
			reset();
			programMemory[1] = noun;
			programMemory[2] = verb;
			run();
			if (programMemory[0] == 19690720)
			{
				std::cout << (100 * noun + verb) << std::endl;
				exit(0);
			}
		}
	}
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
			programRam.push_back(intCode);
		}
	}

	tryPatches();

	return 0;
}