#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <map>

#include "computer.hpp"

//Indexing with the OpCode returnes a OpCode-Struct with the size and the instruction code.
std::map<int, OpCode> opCodes = {
	{1, {4, [](Instruction i, IntCodePC* pc) { i.setValue(pc, 3, i.getValue(pc, 1) + i.getValue(pc, 2)); }}},
	{2, {4, [](Instruction i, IntCodePC* pc) { i.setValue(pc, 3, i.getValue(pc, 1) * i.getValue(pc, 2)); }}},
	{3, {2, [](Instruction i, IntCodePC* pc) { i.setValue(pc, 1, pc->getInput()); }}},
	{4, {2, [](Instruction i, IntCodePC* pc) { pc->setOutput(i.getValue(pc, 1)); }}},
	{5, {3, [](Instruction i, IntCodePC* pc) { if (i.getValue(pc, 1) != 0) pc->rip = i.getValue(pc, 2) - 3; }}},
	{6, {3, [](Instruction i, IntCodePC* pc) { if (i.getValue(pc, 1) == 0) pc->rip = i.getValue(pc, 2) - 3; }}},
	{7, {4, [](Instruction i, IntCodePC* pc) { i.setValue(pc, 3, i.getValue(pc, 1) < i.getValue(pc, 2) ? 1 : 0); }}},
	{8, {4, [](Instruction i, IntCodePC* pc) { i.setValue(pc, 3, i.getValue(pc, 1) == i.getValue(pc, 2) ? 1 : 0); }}},
	{99, {1, [](Instruction i, IntCodePC* pc) { }}}
};

Instruction::Instruction(int value)
{
	std::string str = std::to_string(value);
	opCode = std::stoi(str.substr(str.size() > 2 ? str.size() - 2 : 0));

	modes = std::vector<int>(opCodes[opCode].size - 1, 0);

	for (int i = str.size() - 3, mI = 0; i >= 0; i--, mI++)
		modes[mI] = str[i] == '0' ? 0 : 1;
}

int Instruction::getValue(IntCodePC* pc, int parameter)
{
	if (modes[parameter - 1] == 0)
		return pc->memory[pc->memory[pc->rip + parameter]];
	else
		return pc->memory[pc->rip + parameter];
}

void Instruction::setValue(IntCodePC* pc, int parameter, int value)
{
	pc->memory[pc->memory[pc->rip + parameter]] = value;
}

void Instruction::execute(IntCodePC* pc)
{
	opCodes[opCode].op(*this, pc);
	pc->rip += opCodes[opCode].size;
}

bool IntCodePC::step()
{
	Instruction instruction = Instruction(memory[rip]);

	instruction.execute(this);

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