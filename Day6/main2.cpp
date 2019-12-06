#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>
#include <string>
#include <algorithm>

std::map<std::string, std::vector<std::string>> planets = std::map<std::string, std::vector<std::string>>();

std::string findCOM(std::string orbitor)
{
	for (auto it = planets.begin(); it != planets.end(); ++it)
		if (std::find(std::begin(it->second), std::end(it->second), orbitor) != std::end(it->second))
			return it->first;

	return "";
}

bool traverseOrbitalMap(std::string parent, std::string current, std::string goal, int& total)
{
	total++;

	for (auto&& orbitor : planets[current])
	{
		if (orbitor == parent)
			continue;

		if (orbitor == goal)
			return true;

		int tmpTotal = total;
		if (traverseOrbitalMap(current, orbitor, goal, tmpTotal))
		{
			total = tmpTotal;
			return true;
		}
	}

	int tmpTotal = total;

	std::string com = findCOM(current);
	if (com == parent)
		return false;
	if (traverseOrbitalMap(current, com, goal, tmpTotal))
	{
		total = tmpTotal;
		return true;
	}

	return false;
}

int main(int argc, char** argv)
{
	std::ifstream file("in.txt");

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss = std::istringstream(line);
		std::string planet;

		std::getline(iss, planet, ')');
		std::string com = planet;

		std::getline(iss, planet, '\r');
		std::string orbitor = planet;

		if (planets.find(com) != planets.end())
			planets[com].push_back(orbitor);
		else
			planets[com] = { orbitor };
	}

	int total = 0;
	traverseOrbitalMap("", findCOM("YOU"), findCOM("SAN"), total);

	std::cout << total << std::endl;

	return 0;
}