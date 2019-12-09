#pragma once
#include <vector>
#include <functional>
#include <thread>
#include <map>
#include <atomic>

class IntCodePC
{
	struct Instruction
	{
		std::vector<int> modes;
		IntCodePC* pc;
		int opCode;

		Instruction(IntCodePC* pc, int value);

		int getValue(int parameter);
		void setValue(int parameter, int value);

		void execute();
	};

	struct OpCode
	{
		int size;
		std::function<void(Instruction&)> op;
	};

	friend Instruction;
public:
	void run();
	void reload(std::vector<int> ram);
	void reset();

	void waitForExit();

	int out();
	void in(int in);

	bool hasOutput();
	bool awaitsInput();
	bool running();

private:
	int output;
	int input;

	std::map<int, OpCode> opCodes = {
		{ 1, {4, [this](Instruction& i) { i.setValue(3, i.getValue(1) + i.getValue(2)); }}},
		{ 2, {4, [this](Instruction& i) { i.setValue(3, i.getValue(1) * i.getValue(2)); }}},
		{ 3, {2, [this](Instruction& i) { i.setValue(1, getInput()); }}},
		{ 4, {2, [this](Instruction& i) { setOutput(i.getValue(1)); }}},
		{ 5, {3, [this](Instruction& i) { if (i.getValue(1) != 0) rip = i.getValue(2) - 3; }}},
		{ 6, {3, [this](Instruction& i) { if (i.getValue(1) == 0) rip = i.getValue(2) - 3; }}},
		{ 7, {4, [this](Instruction& i) { i.setValue(3, i.getValue(1) < i.getValue(2) ? 1 : 0); }}},
		{ 8, {4, [this](Instruction& i) { i.setValue(3, i.getValue(1) == i.getValue(2) ? 1 : 0); }}},
		{99, {1, [this](Instruction& i) { }}}
	};

	int rip;
	std::vector<int> memory;

	std::thread thread;

	bool step();

	std::atomic_bool isRunning = {false};

	std::atomic_bool waitOutput = {false};
	std::atomic_bool waitInput = {false};

	int getInput();
	void setOutput(int);
};