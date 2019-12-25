#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>
#include <queue>
#include <algorithm>

enum Direction
{
	L = 0,
	U = 1,
	R = 2,
	D = 3
};

std::pair<int, int> vectorFromDirection(int dir)
{
	switch (dir)
	{
	case L:
		return {-1, 0};
	case R:
		return {1, 0};
	case D:
		return {0, 1};
	case U:
		return {0, -1};
	default:
		throw std::runtime_error("Invalid direction");
	}
}

struct Portal
{
	std::pair<int, int> inner, outer;
	int innerDir, outerDir;
};

struct Worker
{
	std::pair<int, int> pos;
	int lvl;
	int dst;
};

struct Position
{
	std::pair<int, int> pos;
	int lvl;

	bool operator == (const Position& other) const { return pos == other.pos && lvl == other.lvl; }
	bool operator != (const Position& other) const { return pos != other.pos || lvl != other.lvl; }
	bool operator <  (const Position& other) const { return (pos != other.pos) ? (pos < other.pos) : (lvl < other.lvl); }
};

std::map<std::pair<int, int>, char> fullMap;

std::pair<int, int> start, end;
std::map<std::string, Portal> portals;

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	int maxY = 0;
	int maxX = 0;

	std::string line;
	while (std::getline(file, line))
	{
		maxX = 0;
		for (auto&& chr : line)
			fullMap[{maxX++, maxY}] = chr;
		maxY++;
	}

	for (auto&& pos : fullMap)
	{
		if (pos.second == '.')
		{
			std::pair<int, int> left = {pos.first.first - 1, pos.first.second};
			std::pair<int, int> right = {pos.first.first + 1, pos.first.second};
			std::pair<int, int> up = {pos.first.first, pos.first.second - 1};
			std::pair<int, int> down = {pos.first.first, pos.first.second + 1};

			std::string ident = "";

			if (std::isupper(fullMap[left]))
				ident = {fullMap[{left.first - 1, left.second}], fullMap[left]};
			if (std::isupper(fullMap[right]))
				ident = {fullMap[right], fullMap[{right.first + 1, right.second}]};
			if (std::isupper(fullMap[up]))
				ident = {fullMap[{up.first, up.second - 1}], fullMap[up]};
			if (std::isupper(fullMap[down]))
				ident = {fullMap[down], fullMap[{down.first, down.second + 1}]};

			if (ident != "")
			{
				if (ident == "AA")
					start = pos.first;
				else if (ident == "ZZ")
					end = pos.first;
				else
				{
					if (portals.find(ident) != portals.end())
					{
						if (pos.first.first <= 5 || pos.first.first >= maxX - 5 || pos.first.second <= 5 || pos.first.second >= maxY - 5)
							portals[ident].outer = pos.first;
						else
							portals[ident].inner = pos.first;
					}
					else
					{
						Portal portal;

						if (pos.first.first <= 5 || pos.first.first >= maxX - 5 || pos.first.second <= 5 || pos.first.second >= maxY - 5)
							portal.outer = pos.first;
						else
							portal.inner = pos.first;

						portals[ident] = portal;
					}
				}
			}
		}
	}

	std::map<Position, int> states;
	std::queue<Worker> workers;
	workers.push({start, 0, 0});

	while(!workers.empty())
	{
		auto worker = workers.front();
		workers.pop();

		if (worker.pos == end && worker.lvl == 0)
		{
			std::cout << "Steps required " << worker.dst << '\n';
			break;
		}

		Position state = {worker.pos, worker.lvl};
		auto found = states.find(state);
		int dist = found != states.end() ? found->second : std::numeric_limits<int>::max();
		if (worker.dst >= dist)
			continue;
		
		states[state] = worker.dst++;
		for (int i = 0; i < 4; i++)
		{
			auto dir = vectorFromDirection(i);
			std::pair<int, int> pos = { worker.pos.first + dir.first, worker.pos.second + dir.second };

			if (fullMap[pos] == '.')
				workers.push({pos, worker.lvl, worker.dst});
			else if (isupper(fullMap[pos]))
			{
				auto foundPortal = std::find_if(portals.begin(), portals.end(), [worker](std::pair<std::string, Portal> p){ return p.second.inner == worker.pos || p.second.outer == worker.pos; });
				if (foundPortal != portals.end())
				{
					if (foundPortal->second.inner == worker.pos)
					{
						pos = foundPortal->second.outer;
						workers.push({pos, worker.lvl + 1, worker.dst});
					}
					else if (foundPortal->second.outer == worker.pos && worker.lvl > 0)
					{
						pos = foundPortal->second.inner;
						workers.push({pos, worker.lvl - 1, worker.dst});
					}
				}
			}
		}
	}

	std::cout << "Press any key";
	std::cin.get();

	return 0;
}