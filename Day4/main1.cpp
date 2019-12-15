#include <iostream>

#include <set>
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

		std::set<char> numSet = std::set<char>();
		std::for_each(numString.begin(), numString.end(), [&numSet] (char c) -> void { numSet.insert(c); });

		if (numSet.size() == numString.size())
			continue;
		
		possible++;
	}

	std::cout << possible << std::endl;

	return 0;
}