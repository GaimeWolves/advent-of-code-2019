#include <iostream>
#include <fstream>
#include <sstream>

#include <set>
#include <map>

std::map<int, uint32_t> levels;
int min = 0;
int max = 0;

uint32_t outerMask = 0b1111110001100011000111111;
uint32_t innerMask = 0b0000000100010100010000000;

void printState()
{
	for (int i = min; i <= max; i++)
	{
		std::cout << "Depth: " << i << '\n';
		for (int idx = 0; idx < 25; idx++)
		{
			if (idx > 0 && idx % 5 == 0)
				std::cout << '\n';

			if (levels[i] & (1 << idx))
				std::cout << '#';
			else
				std::cout << '.';
		}
		std::cout << '\n';
		std::cout << '\n';
	}

	std::cout << '\n';
	std::cout << '\n';
	std::cout << '\n';
}

int countBugs()
{
	int bugs = 0;

	for (int i = min; i <= max; i++)
	{
		for (int idx = 0; idx < 25; idx++)
		{
			if (levels[i] & (1 << idx))
				bugs++;
		}
	}

	return bugs;
}

void addNeededLevels()
{
	if ((levels[min] & outerMask))
		levels[--min] = 0;

	if ((levels[max] & innerMask))
		levels[++max] = 0;
}

int idx(int x, int y)
{
	return (1 << (y * 5 + x));
}

void step()
{
	addNeededLevels();

	auto old = levels;
	for (int i = min; i <= max; i++)
	{
		int cnt = 0;

		for (int x = 0; x < 5; x++)
		{
			for (int y = 0; y < 5; y++)
			{
				if (x == 2 && y == 2)
					continue;

				cnt = 0;
				if (x == 0)
				{
					if (i > min)
						if (old[i - 1] & idx(1, 2))
							cnt++;
				}
				else if (x == 3 && y == 2)
				{
					if (i < max)
					{
						for (int tY = 0; tY < 5; tY++)
							if (old[i + 1] & idx(4, tY))
								cnt++;
					}
				}
				else
				{
					if (old[i] & idx(x - 1, y))
						cnt++;
				}

				if (x == 4)
				{
					if (i > min)
						if (old[i - 1] & idx(3, 2))
							cnt++;
				}
				else if (x == 1 && y == 2)
				{
					if (i < max)
					{
						for (int tY = 0; tY < 5; tY++)
							if (old[i + 1] & idx(0, tY))
								cnt++;
					}
				}
				else
				{
					if (old[i] & idx(x + 1, y))
						cnt++;
				}

				if (y == 0)
				{
					if (i > min)
						if (old[i - 1] & idx(2, 1))
							cnt++;
				}
				else if (x == 2 && y == 3)
				{
					if (i < max)
					{
						for (int tX = 0; tX < 5; tX++)
							if (old[i + 1] & idx(tX, 4))
								cnt++;
					}
				}
				else
				{
					if (old[i] & idx(x, y - 1))
						cnt++;
				}

				if (y == 4)
				{
					if (i > min)
						if (old[i - 1] & idx(2, 3))
							cnt++;
				}
				else if (x == 2 && y == 1)
				{
					if (i < max)
					{
						for (int tX = 0; tX < 5; tX++)
							if (old[i + 1] & idx(tX, 0))
								cnt++;
					}
				}
				else
				{
					if (old[i] & idx(x, y + 1))
						cnt++;
				}

				bool bug = (old[i] & (1 << (y * 5 + x))) > 0;
				if (cnt != 1 && bug)
					levels[i] ^= 1 << (y * 5 + x);
			
				if ((cnt == 1 || cnt == 2) && !bug)
					levels[i] ^= 1 << (y * 5 + x);
			}
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
			levels[0] |= ((uint32_t)(chr == '#') << i++);

	levels[0] &= 0b1111111111111111111111111;

	for (int i = 0; i < 200; i++)
		step();

	printState();
	std::cout << "Bugs: " << countBugs() << '\n';

	return 0;
}