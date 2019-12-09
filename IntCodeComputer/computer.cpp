#include <iostream>
#include <fstream>
#include <sstream>

#include "computer.hpp"

IntCodePC::Instruction::Instruction(IntCodePC* pc, int value)
{
	std::string str = std::to_string(value);
	opCode = std::stoi(str.substr(str.size() > 2 ? str.size() - 2 : 0));

	modes = std::vector<int>(pc->opCodes[opCode].size - 1, 0);

	for (int i = str.size() - 3, mI = 0; i >= 0; i--, mI++)
		modes[mI] = str[i] == '0' ? 0 : 1;

	this->pc = pc;
}

int IntCodePC::Instruction::getValue(int parameter)
{
	if (modes[parameter - 1] == 0)
		return pc->memory[pc->memory[pc->rip + parameter]];
	else
		return pc->memory[pc->rip + parameter];
}

void IntCodePC::Instruction::setValue(int parameter, int value)
{
	pc->memory[pc->memory[pc->rip + parameter]] = value;
}

void IntCodePC::Instruction::execute()
{
	pc->opCodes[opCode].op(*this);
	pc->rip += pc->opCodes[opCode].size;
}

bool IntCodePC::step()
{
	Instruction instruction = Instruction(this, memory[rip]);

	instruction.execute();

	if (instruction.opCode == 99) // HLT
		return false;

	return true;
}

void IntCodePC::run()
{
	isRunning = true;

	thread = std::thread([this]() { while(step()); isRunning = false; });
}

void IntCodePC::waitForExit()
{
	thread.join();
}

void IntCodePC::reload(std::vector<int> ram)
{
	memory = ram;
}

void IntCodePC::reset()
{
	rip = 0;
}

void IntCodePC::setOutput(int out)
{
	this->output = out;
	waitOutput = true;
}

int IntCodePC::getInput()
{
	waitInput = true;

	while(waitInput);
	return input;
}

int IntCodePC::out()
{
	while(!waitOutput);
	waitOutput = false;
	return output;
}

void IntCodePC::in(int in)
{
	while(!waitInput);
	this->input = in;
	waitInput = false;
}

bool IntCodePC::hasOutput()
{
	return waitOutput;
}

bool IntCodePC::awaitsInput()
{
	return waitInput;
}

bool IntCodePC::running()
{
	return isRunning;
}