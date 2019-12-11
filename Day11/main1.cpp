#include <iostream>
#include <fstream>
#include <sstream>

#include <chrono>
#include <thread>

#include <map>
#include <algorithm>

#include "computer.hpp"

std::vector<int64_t> ram = std::vector<int64_t>();

IntCodePC pc;

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
		exit(-1);
	}

	std::ifstream file(argv[1]);

	std::string line;
	while (std::getline(file, line))
	{
		std::string intCodeLine;
		std::istringstream iss(line);

		while(std::getline(iss, intCodeLine, ','))
		{
			int64_t intCode = std::stoll(intCodeLine);
			ram.push_back(intCode);
		}
	}

	std::map<std::pair<int, int>, bool> picture;
	std::pair<int, int> currentPos = std::make_pair(0, 0);
	picture[currentPos] = true;
	std::pair<int, int> direction = std::make_pair(0, 1);

	pc.reload(ram);
	pc.reset();
	pc.run();

	while (pc.running())
	{
		int64_t num = 0;
		if (picture.find(currentPos) != picture.end())
			num = picture[currentPos] ? 1 : 0;
		
		pc.in(num);
		bool toPaint = pc.out() == 1 ? true : false;
		bool dir = pc.out() == 1 ? true : false;

		picture[currentPos] = toPaint;

		if (dir)
			direction = std::make_pair(direction.second, -direction.first);
		else
			direction = std::make_pair(-direction.second, direction.first);

		currentPos = std::make_pair(currentPos.first + direction.first, currentPos.second + direction.second);

		std::cout << currentPos.first << " " << currentPos.second << std::endl;
	}

	std::cout << picture.size() << std::endl;
	
	pc.waitForExit();

	auto minmaxX = std::minmax_element(picture.begin(), picture.end(), [](std::pair<std::pair<int, int>, bool> p1, std::pair<std::pair<int, int>, bool> p2) { return p1.first.first < p2.first.first; });
	auto minmaxY = std::minmax_element(picture.begin(), picture.end(), [](std::pair<std::pair<int, int>, bool> p1, std::pair<std::pair<int, int>, bool> p2) { return p1.first.second < p2.first.second; });

	for (int y = minmaxY.second->first.second - minmaxY.first->first.second; y >= 0; y--)
	{
		for (int x = 0; x < minmaxX.second->first.first - minmaxX.first->first.first + 1; x++)
		{
			auto p = std::make_pair(minmaxX.first->first.first + x, minmaxY.first->first.second + y);

			if (picture[p])
				std::cout << '#';
			else
				std::cout << ' ';
		}
		std::cout << std::endl;
	}

	return 0;
}