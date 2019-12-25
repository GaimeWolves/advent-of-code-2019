#pragma once
#include <vector>
#include <functional>
#include <thread>
#include <map>
#include <atomic>
#include <mutex>
#include <chrono>

class IntCodePC
{
	struct Instruction
	{
		std::vector<int64_t> modes;
		IntCodePC* pc;
		int64_t opCode;

		Instruction(IntCodePC* pc, int64_t value);

		int64_t getValue(int64_t parameter);
		void setValue(int64_t parameter, int64_t value);

		void execute();
	};

	struct OpCode
	{
		int64_t size;
		std::function<void(Instruction&)> op;
	};

	friend Instruction;
public:
	void run();
	void reload(std::vector<int64_t> ram);
	void reset();

	void waitForExit();

	int64_t out();
	void in(int64_t in);

	bool hasOutput();
	bool awaitsInput();
	bool running();

private:
	std::atomic<int64_t> output;
	std::atomic<int64_t> input;

	std::map<int64_t, OpCode> opCodes = {
		{ 1, {4, [this](Instruction& i) { i.setValue(3, i.getValue(1) + i.getValue(2)); }}},
		{ 2, {4, [this](Instruction& i) { i.setValue(3, i.getValue(1) * i.getValue(2)); }}},
		{ 3, {2, [this](Instruction& i) { i.setValue(1, getInput()); }}},
		{ 4, {2, [this](Instruction& i) { setOutput(i.getValue(1)); }}},
		{ 5, {3, [this](Instruction& i) { if (i.getValue(1) != 0) rip = i.getValue(2) - 3; }}},
		{ 6, {3, [this](Instruction& i) { if (i.getValue(1) == 0) rip = i.getValue(2) - 3; }}},
		{ 7, {4, [this](Instruction& i) { i.setValue(3, i.getValue(1) < i.getValue(2) ? 1 : 0); }}},
		{ 8, {4, [this](Instruction& i) { i.setValue(3, i.getValue(1) == i.getValue(2) ? 1 : 0); }}},
		{ 9, {2, [this](Instruction& i) { rbase += i.getValue(1); }}},
		{99, {1, [this](Instruction& i) { }}}
	};

	int64_t rip;
	int64_t rbase;
	std::vector<int64_t> memory;

	std::thread thread;
	std::mutex mtx;

	bool step();

	std::atomic_bool isRunning = {false};

	std::atomic_bool waitOutput = {false};
	std::atomic_bool waitInput = {false};

	int64_t getInput();
	void setOutput(int64_t);
};