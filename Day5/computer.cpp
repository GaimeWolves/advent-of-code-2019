#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <map>

#include "computer.hpp"

std::vector<int> programMemory = std::vector<int>();
std::vector<int> programRam = std::vector<int>();
int rip = 0;

//Indexing with the OpCode returnes a OpCode-Struct with the size and the instruction code.
std::map<int, OpCode> opCodes = {
	{1, {4, [](Instruction i) { i.setValue(3, i.getValue(1) + i.getValue(2)); }}},
	{2, {4, [](Instruction i) { i.setValue(3, i.getValue(1) * i.getValue(2)); }}},
	{3, {2, [](Instruction i) { std::string num; std::cin >> num; i.setValue(1, std::stoi(num)); }}},
	{4, {2, [](Instruction i) { std::cout << i.getValue(1) << std::endl; }}},
	{5, {3, [](Instruction i) { if (i.getValue(1) != 0) rip = i.getValue(2) - 3; }}},
	{6, {3, [](Instruction i) { if (i.getValue(1) == 0) rip = i.getValue(2) - 3; }}},
	{7, {4, [](Instruction i) { i.setValue(3, i.getValue(1) < i.getValue(2) ? 1 : 0); }}},
	{8, {4, [](Instruction i) { i.setValue(3, i.getValue(1) == i.getValue(2) ? 1 : 0); }}},
	{99, {1, [](Instruction i) { std::cout << "HALT" << std::endl; }}}
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

	reset();
	run();

	return 0;
}