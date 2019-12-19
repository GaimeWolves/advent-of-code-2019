#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>

std::string applyPhaseUnoptimized(const std::string& input, const std::vector<int>& phase, int offset)
{
	int index = offset;

	std::string output = input;

	while(index++ < (int)input.size())
	{
		int result = 0;
		for (int i = index - 1; i < (int)input.size(); i++)
			result += phase[((i + 1) / index) % phase.size()] * (int)(input[i] - '0');

		result *= result < 0 ? -1 : 1;
		output[index - 1] = ('0' + (char)(result % 10));
	}

	return output;
}

std::string applyPhaseOptimizedAfterMiddle(const std::string& input, const std::vector<int>& phase, int offset)
{
	int index = (int)input.size();

	std::string output = input;
	int sum = 0;
	while(index-- > offset)
	{
		sum += (int)(input[index] - '0');
		output[index] = ('0' + (char)(sum % 10));
	}

	return output;
}

std::string applyPhase(const std::string& input, const std::vector<int>& phase, int offset)
{
	if (offset > input.size() / 2)
		return applyPhaseOptimizedAfterMiddle(input, phase, offset);
	else
		return applyPhaseUnoptimized(input, phase, offset);
}

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	std::string input;
	std::getline(file, input);

	std::string part1Input = input;
	std::string part2Input = input;

	for (int i = 0; i < 9999; i++)
		part2Input += input;

	std::vector<int> phase = {0, 1, 0, -1};

	for (int i = 0; i < 100; i++)
		part1Input = applyPhase(part1Input, phase, 0);

	std::cout << part1Input << "\n\n";

	int offset = std::stoi(input.substr(0, 7));
	std::cout << offset << "  " << part2Input.size() << '\n';

	for (int i = 0; i < 100; i++)
		part2Input = applyPhaseOptimizedAfterMiddle(part2Input, phase, offset);

	std::cout << part2Input.substr(offset, 8) << '\n';

	return 0;
}