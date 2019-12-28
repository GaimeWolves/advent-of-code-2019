#include <iostream>
#include <fstream>
#include <sstream>
#include <set>

std::set<uint32_t> states;
uint32_t area = 0;

void printState()
{
	for (int i = 0; i < 32; i++)
	{
		if (i % 5 == 0)
			std::cout << '\n';
		if (area & (1 << i))
			std::cout << '#';
		else
			std::cout << '.';
	}
	std::cout << '\n';
}

void step()
{
	uint32_t old = area;
	for (int x = 0; x < 5; x++)
	{
		for (int y = 0; y < 5; y++)
		{
			int cnt = 0;
			if (x > 0 && (old & (1 << (y * 5 + (x - 1))))) cnt++;
			if (x < 4 && (old & (1 << (y * 5 + (x + 1))))) cnt++;
			if (y > 0 && (old & (1 << ((y - 1) * 5 + x)))) cnt++;
			if (y < 4 && (old & (1 << ((y + 1) * 5 + x)))) cnt++;

			bool bug = (old & (1 << (y * 5 + x))) > 0;
			if (cnt != 1 && bug)
				area ^= 1 << (y * 5 + x);
			
			if ((cnt == 1 || cnt == 2) && !bug)
				area ^= 1 << (y * 5 + x);
		}
	}
}

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	int i = 0;

	std::string line;
	while (std::getline(file, line))
		for (auto&& chr : line)
			area |= ((uint32_t)(chr == '#') << i++);

	area &= 0b1111111111111111111111111;

	printState();
	states.insert(area);
	while (true)
	{
		step();
		printState();

		if (!states.insert(area).second)
		{
			std::cout << "First duplicate State with biodiversity of " << area << " :";
			printState();
			break;
		}
	}

	return 0;
}