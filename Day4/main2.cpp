#include <iostream>

#include <map>
#include <algorithm>

int min = 248345;
int max = 746315;

int main(int argc, char** argv)
{
	int possible = 0;

	for (int num = min; num <= max; num++)
	{
		auto numString = std::to_string(num);

		int prev = -1;
		bool cont = false;
		for (auto&& chr : numString)
		{
			int number = std::stoi(std::string({chr}));
			if (prev > number)
			{
				cont = true;
				break;
			}
			prev = number;
		}

		if (cont) continue;

		std::map<char, int> numMap = std::map<char, int>();
		std::for_each(numString.begin(), numString.end(), [&numMap] (char c) -> void { numMap[c]++; });

		if (std::find_if(numMap.begin(), numMap.end(),[](const auto& n) {return n.second == 2; }) == numMap.end())
			continue;

		possible++;
	}

	std::cout << possible << std::endl;

	return 0;
}