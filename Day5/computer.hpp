#pragma once
#include <vector>
#include <functional>

struct Instruction
{
	std::vector<int> modes;
	int opCode;

	Instruction(int value);

	int getValue(int parameter);
	void setValue(int parameter, int value);

	void execute();
};

struct OpCode
{
	int size;
	std::function<void(Instruction)> op;
};