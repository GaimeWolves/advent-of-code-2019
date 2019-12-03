#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <utility>
#include <limits>
#include <cmath>

std::vector<std::pair<int, int>> wireA;

std::pair<int, int> getIntersectionPoint(int x1, int y1, int x2, int y2, int x3, int y3, int x4, int y4)
{
	if (x1 == x2) //Vertical
	{
		if (x3 == x4) //Also vertical
			return std::make_pair(0, 0);

		if ((y3 >= y1 && y3 <= y2) || (y3 <= y1 && y3 >= y2)) //Not passing line
			if ((x3 <= x1 && x4 >= x1) || (x3 >= x1 && x4 <= x1)) //Intersecting
				return std::make_pair(x1, y3);
	}
	else //Horizontal
	{
		if (x3 != x4) //Also horizontal
			return std::make_pair(0, 0);

		if ((x3 >= x1 && x3 <= x2) || (x3 <= x1 && x3 >= x2)) //Not passing line
			if ((y3 <= y1 && y4 >= y1) || (y3 >= y1 && y4 <= y1)) //Intersecting
				return std::make_pair(x3, y1);
	}

	return std::make_pair(0, 0);
}

int main(int argc, char** argv)
{
	std::ifstream file("test.txt");

	wireA.push_back(std::make_pair(0, 0));

	std::string line;
	std::getline(file, line);

	std::string token;
	std::istringstream iss(line);
	while(std::getline(iss, token, ','))
	{
		int delta = std::stoi(token.substr(1, std::string::npos));

		switch(token[0])
		{
			case 'U':
				wireA.push_back(std::make_pair(wireA[wireA.size() - 1].first, wireA[wireA.size() - 1].second + delta));
				break;
			case 'D':
				wireA.push_back(std::make_pair(wireA[wireA.size() - 1].first, wireA[wireA.size() - 1].second - delta));
				break;
			case 'R':
				wireA.push_back(std::make_pair(wireA[wireA.size() - 1].first + delta, wireA[wireA.size() - 1].second));
				break;
			case 'L':
				wireA.push_back(std::make_pair(wireA[wireA.size() - 1].first - delta, wireA[wireA.size() - 1].second));
				break;
		}
	}

	int best = std::numeric_limits<int>::max();
	int x1 = 0, x2 = 0, y1 = 0, y2 = 0;

	std::getline(file, line);
	iss = std::istringstream(line);
	while(std::getline(iss, token, ','))
	{
		x1 = x2;
		y1 = y2;
		int delta = std::stoi(token.substr(1, std::string::npos));
		switch(token[0])
		{
			case 'U':
				y2 += delta;
				break;
			case 'D':
				y2 -= delta;
				break;
			case 'R':
				x2 += delta;
				break;
			case 'L':
				x2 -= delta;
				break;
		}

		for (int i = 1; i < wireA.size(); i++)
		{
			int x3 = wireA[i - 1].first;
			int y3 = wireA[i - 1].second;

			int x4 = wireA[i].first;
			int y4 = wireA[i].second;

			auto p = getIntersectionPoint(x1, y1, x2, y2, x3, y3, x4, y4);

			if (p.first != 0 && p.second != 0)
			{
				if (std::abs(p.first) + std::abs(p.second) < best)
					best = std::abs(p.first) + std::abs(p.second);
			}
		}
	}

	std::cout << best << std::endl;

	return 0;
}