#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <cmath>
#include <map>

#include <algorithm>

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

	std::map<float, std::pair<int, int>> angles{};
	std::vector<std::pair<float, std::pair<int, int>>> sortedAngles{};
	int max = 0;

	for (auto center : asteroids)
	{
		angles.clear();

		for (auto other : asteroids)
		{
			if (center == other)
				continue;

			auto pos = std::make_pair(other.first - center.first, other.second - center.second);
			float angle = std::atan2(pos.first, pos.second);
			
			if (angles.find(angle) != angles.end())
			{
				auto posTmp = angles[angle];
				auto pos2 = std::make_pair(posTmp.first - center.first, posTmp.second - center.second);

				if (std::sqrt(std::pow(pos.first, 2) + std::pow(pos.second, 2)) < std::sqrt(std::pow(pos2.first, 2) + std::pow(pos2.second, 2)))
					angles[angle] = other;
			}
			else
				angles[angle] = other;
		}

		if (angles.size() > max)
		{
			max = angles.size();

			if (angles.size() >= 200)
			{
				sortedAngles.clear();
				for (auto&& pair : angles)
					sortedAngles.push_back(pair);
			}
		}
	}

	std::cout << max << std::endl;

	std::sort(sortedAngles.begin(), sortedAngles.end(), [](std::pair<float, std::pair<int, int>> p1, std::pair<float, std::pair<int, int>> p2){ return p1.first > p2.first; });
	std::cout << sortedAngles[199].second.first * 100 + sortedAngles[199].second.second << std::endl;

	return 0;
}