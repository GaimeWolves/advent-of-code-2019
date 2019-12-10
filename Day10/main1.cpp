#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <set>
#include <cmath>

std::vector<std::pair<int, int>> asteroids{};

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	int y = 0;

	std::string line;
	while (std::getline(file, line))
	{
		for (int x = 0; x < line.size(); x++)
			if (line[x] == '#')
				asteroids.push_back(std::make_pair(x, y));
		y++;
	}

	std::set<float> angles{};
	int max = 0;

	for (auto center : asteroids)
	{
		angles.clear();

		for (auto other : asteroids)
		{
			if (center == other)
				continue;

			auto pos = std::make_pair(other.first - center.first, other.second - center.second);
			angles.insert(std::atan2(pos.second, pos.first));
		}

		if (angles.size() > max)
			max = angles.size();
	}

	std::cout << max << std::endl;

	return 0;
}