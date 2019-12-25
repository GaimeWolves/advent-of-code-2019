#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>
#include <string>

std::map<std::string, std::vector<std::string>> planets = std::map<std::string, std::vector<std::string>>();

int traverseOrbitalMap(std::string COM, std::string parent, int total)
{
	int ret = total++;

	for (auto&& orbitor : planets[COM])
	{
		if (orbitor == parent)
			continue;

		ret += traverseOrbitalMap(orbitor, COM, total);
	}

	return ret;
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

	std::cout << traverseOrbitalMap("COM", "", 0) << std::endl;

	return 0;
}