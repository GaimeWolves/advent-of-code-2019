#include <iostream>
#include <fstream>
#include <sstream>

#include "computer.hpp"

IntCodePC::Instruction::Instruction(IntCodePC* pc, int64_t value)
{
	std::string str = std::to_string(value);
	opCode = std::stoi(str.substr(str.size() > 2 ? str.size() - 2 : 0));

	modes = std::vector<int64_t>(pc->opCodes[opCode].size - 1, 0);

	for (int64_t i = str.size() - 3, mI = 0; i >= 0; i--, mI++)
		modes[mI] = (int64_t)str[i] - 48;

	this->pc = pc;
}

int64_t IntCodePC::Instruction::getValue(int64_t parameter)
{
	int64_t pointer = pc->rip + parameter;
	if (pointer >= (int64_t)pc->memory.size())
		pc->memory.resize(pointer + 2, 0);

	int64_t value = pc->memory[pointer];

	if (modes[parameter - 1] != 1)
	{
		int64_t address = (modes[parameter - 1] == 2 ? pc->rbase : 0) + value;

		if (address >= (int64_t)pc->memory.size())
			pc->memory.resize(address + 2, 0);

		value = pc->memory[address];
	}

	return value;
}

void IntCodePC::Instruction::setValue(int64_t parameter, int64_t value)
{
	int64_t pointer = pc->rip + parameter;
	if (pointer >= (int64_t)pc->memory.size())
		pc->memory.resize(pointer + 2, 0);

	int64_t param = pc->memory[pointer];
	int64_t address = (modes[parameter - 1] == 2 ? pc->rbase : 0) + param;

	if (address >= (int64_t)pc->memory.size())
		pc->memory.resize(address + 2, 0);

	if (modes[parameter - 1] != -1)
		pc->memory[address] = value;
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

	thread = std::thread([this]() {
		while(step());
		std::lock_guard<std::mutex> lck {mtx};
		isRunning = false;
	});
}

void IntCodePC::waitForExit()
{
	thread.join();
}

void IntCodePC::reload(std::vector<int64_t> ram)
{
	memory = std::vector<int64_t>();
	memory = ram;
}

void IntCodePC::reset()
{
	rip = 0;
}

void IntCodePC::setOutput(int64_t out)
{
	while(waitOutput);

	std::lock_guard<std::mutex> lck {mtx};
	this->output = out;
	waitOutput = true;
}

int64_t IntCodePC::getInput()
{
	waitInput = true;
	while(waitInput);

	std::lock_guard<std::mutex> lck {mtx};
	return input;
}

int64_t IntCodePC::out()
{
	while(!waitOutput);

	std::lock_guard<std::mutex> lck {mtx};
	waitOutput = false;
	return output;
}

void IntCodePC::in(int64_t in)
{
	while(!waitInput);

	std::lock_guard<std::mutex> lck {mtx};
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
	std::lock_guard<std::mutex> lck {mtx};
	return isRunning;
}