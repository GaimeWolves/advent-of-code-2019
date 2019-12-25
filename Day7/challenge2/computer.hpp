#pragma once
#include <vector>
#include <functional>
#include <thread>

class IntCodePC
{
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

	int getInput();
	void setOutput(int);

	int rip;
	std::vector<int> memory;
private:
	int output;
	int input;

	std::thread thread;

	bool step();

	bool isRunning;

	bool waitOutput;
	bool waitInput;
};

struct Instruction
{
	std::vector<int> modes;
	int opCode;

	Instruction(int value);

	int getValue(IntCodePC* pc, int parameter);
	void setValue(IntCodePC* pc, int parameter, int value);

	void execute(IntCodePC* pc);
};

struct OpCode
{
	int size;
	std::function<void(Instruction, IntCodePC*)> op;
};