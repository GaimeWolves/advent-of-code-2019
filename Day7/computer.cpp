#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <map>

#include "computer.hpp"

/*    PC    */

std::vector<int> programMemory = std::vector<int>();
std::vector<int> programRam = std::vector<int>();
int rip = 0;

std::string out = "";

int inputCallback();
void outputCallback(int out);

//Indexing with the OpCode returnes a OpCode-Struct with the size and the instruction code.
std::map<int, OpCode> opCodes = {
	{1, {4, [](Instruction i) { i.setValue(3, i.getValue(1) + i.getValue(2)); }}},
	{2, {4, [](Instruction i) { i.setValue(3, i.getValue(1) * i.getValue(2)); }}},
	{3, {2, [](Instruction i) { i.setValue(1, inputCallback()); }}},
	{4, {2, [](Instruction i) { outputCallback(i.getValue(1)); }}},
	{5, {3, [](Instruction i) { if (i.getValue(1) != 0) rip = i.getValue(2) - 3; }}},
	{6, {3, [](Instruction i) { if (i.getValue(1) == 0) rip = i.getValue(2) - 3; }}},
	{7, {4, [](Instruction i) { i.setValue(3, i.getValue(1) < i.getValue(2) ? 1 : 0); }}},
	{8, {4, [](Instruction i) { i.setValue(3, i.getValue(1) == i.getValue(2) ? 1 : 0); }}},
	{99, {1, [](Instruction i) { }}}
};

Instruction::Instruction(int value)
{
	std::string str = std::to_string(value);
	opCode = std::stoi(str.substr(str.size() > 2 ? str.size() - 2 : 0));

	modes = std::vector<int>(opCodes[opCode].size - 1, 0);

	for (int i = str.size() - 3, mI = 0; i >= 0; i--, mI++)
		modes[mI] = str[i] == '0' ? 0 : 1;
}

int Instruction::getValue(int parameter)
{
	if (modes[parameter - 1] == 0)
		return programMemory[programMemory[rip + parameter]];
	else
		return programMemory[rip + parameter];
}

void Instruction::setValue(int parameter, int value)
{
	programMemory[programMemory[rip + parameter]] = value;
}

void Instruction::execute()
{
	opCodes[opCode].op(*this);
	rip += opCodes[opCode].size;
}

bool step()
{
	Instruction instruction = Instruction(programMemory[rip]);

	instruction.execute();

	if (instruction.opCode == 99) // HLT
		return false;

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

/*  END PC  */

int A = -1;
int B = -1;
int C = -1;
int D = -1;
int E = -1;

int currentAmp = 0;
int outputValue = 0;

int record = 0;

bool askPhase = true;

void outputCallback(int out)
{
	outputValue = out;
	currentAmp++;
}

int inputCallback()
{
	if (askPhase)
	{
		askPhase = false;
		switch (currentAmp)
		{
		case 0:
			return A;
			break;
		case 1:
			return B;
			break;
		case 2:
			return C;
			break;
		case 3:
			return D;
			break;
		case 4:
			return E;
			break;
		}
	}
	else
	{
		askPhase = !askPhase;
		return outputValue;
	}

	return 0;
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
			int intCode = std::stoi(intCodeLine);
			programRam.push_back(intCode);
		}
	}

	for (A = 0; A < 5; A++)
	{
		for (B = 0; B < 5; B++)
		{
			if (B == A)
				continue;
			for (C = 0; C < 5; C++)
			{
				if (C == B || C == A)
					continue;
				for (D = 0; D < 5; D++)
				{
					if (D == C || D == B || D == A)
						continue;
					for (E = 0; E < 5; E++)
					{
						if (E == D || E == C || E == B || E == A)
							continue;
						
						currentAmp = 0;
						outputValue = 0;
						askPhase = true;

						std::cout << "Config: " << A << B << C << D << E << " results in ";
						reset();
						run();
						rip = 0;
						run();
						rip = 0;
						run();
						rip = 0;
						run();
						rip = 0;
						run();
						std::cout << outputValue << std::endl;

						if (outputValue > record)
							record = outputValue;
					}
				}
			}
		}
	}

	std::cout << "Max value " << record << std::endl;

	return 0;
}