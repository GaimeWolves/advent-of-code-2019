#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <vector>
#include <set>
#include <cctype>
#include <queue>
#include <algorithm>

#include <ncurses.h>

struct Key
{
	char name;
	std::pair<int, int> position;

	Key(char _name, std::pair<int, int> _position)
		: name(_name), position(_position)
	{}

	uint32_t getIndex()
	{
		return (1 << (uint32_t)(name - 'a'));
	}
};

struct Worker
{
	std::pair<int, int> pos;
	uint32_t visited;
	int distance;
};

struct State
{
	std::pair<int, int> pos;
	uint32_t visited;

	bool operator == (const State& other) const { return pos == other.pos && visited == other.visited; }
	bool operator != (const State& other) const { return pos != other.pos || visited != other.visited; }
	bool operator <  (const State& other) const { return (pos != other.pos) ? (pos < other.pos) : (visited < other.visited); }
};

std::map<std::pair<int, int>, char> fullMap;
std::vector<std::pair<int, int>> map1, map2, map3, map4;
std::vector<Key> doors1, doors2, doors3, doors4;
std::vector<Key> keys1, keys2, keys3, keys4;
std::pair<int, int> pos1, pos2, pos3, pos4;

void quit()
{
	endwin();
}

void removeDeadEnds(std::pair<int, int> pos, std::pair<int, int> prev, std::set<std::pair<int, int>>& visited, std::vector<std::pair<int, int>>& map, std::vector<Key>& doors, std::vector<Key>& keys)
{
	if (std::find(map.begin(), map.end(), pos) == map.end())
		return;

	std::pair<int, int> dir = {0, 1};

	if (!visited.insert(pos).second)
		return;

	for (int _ = 0; _ < 4; _++)
	{
		std::pair<int, int> newPos = { pos.first + dir.first, pos.second + dir.second };
		if (newPos != prev)
			removeDeadEnds(newPos, pos, visited, map, doors, keys);

		dir = { -dir.second, dir.first };
	}

	if (std::find_if(keys.begin(), keys.end(), [pos](Key& key){ return key.position == pos; }) != keys.end())
		return;

	for (int _ = 0; _ < 4; _++)
	{
		std::pair<int, int> newPos = { pos.first + dir.first, pos.second + dir.second };
		if (newPos != prev && std::find(map.begin(), map.end(), newPos) != map.end())
			return;

		dir = { -dir.second, dir.first };
	}

	map.erase(std::find(map.begin(), map.end(), pos));
}

void drawMap()
{
	for (auto&& pos : fullMap)
		mvaddch(pos.first.second, pos.first.first, pos.second);
}

void drawMapFeatures(std::vector<Key>& keys, std::vector<Key>& doors, std::vector<std::pair<int, int>>& map, std::pair<int, int>& pos, int xR, int yR, int X, int Y)
{
	for (int x = X; x < xR; x++)
		for (int y = Y; y < yR; y++)
			mvaddch(y, x, '#');

	for (auto&& pos : map)
		mvaddch(pos.second, pos.first, '.');

	for (auto&& key : keys)
		mvaddch(key.position.second, key.position.first, key.name);

	for (auto&& door : doors)
		mvaddch(door.position.second, door.position.first, std::toupper(door.name));

	mvaddch(pos.second, pos.first, '@');
}

inline int parsePos(int x, int y)
{
	return (y << 16) | (x & 0xFFFF);
}

inline std::pair<int, int> parseNode(int node)
{
	return {node & 0xFFFF, node >> 16};
}

int solveQuadrant(std::vector<Key>& keys, std::vector<Key>& doors, std::vector<std::pair<int, int>>& map, std::pair<int, int>& pos)
{
	std::queue<Worker> workers;

	Worker worker = { pos, 0xFFFFFFFF, 0 };
	for (auto&& key : keys)
		worker.visited &= ~key.getIndex();
	workers.push(worker);

	std::map<State, int> states;

	while(!workers.empty())
	{
		auto worker = workers.front();
		workers.pop();

		auto key = std::find_if(keys.begin(), keys.end(), [worker](Key& key){ return key.position == worker.pos; });
		if (key != keys.end())
		{
			worker.visited |= key->getIndex();
			if (std::all_of(keys.begin(), keys.end(), [worker](Key& key){ return (key.getIndex() & worker.visited) > 0; }))
				return worker.distance;
		}
		else
		{
			auto door = std::find_if(doors.begin(), doors.end(), [worker](Key& door){ return door.position == worker.pos; });
			if (door != doors.end())
			{
				if (!(worker.visited & door->getIndex()))
					continue;
			}
		}

		State state = { worker.pos, worker.visited };
		auto foundState = states.find(state);
		int dist = foundState != states.end() ? foundState->second : std::numeric_limits<int>::max();
		if (worker.distance >= dist)
			continue;

		states[state] = worker.distance++;
		std::pair<int, int> dir = {0, 1};
		for (int i = 0; i < 4; i++)
		{
			std::pair<int, int> newPos = { worker.pos.first + dir.first, worker.pos.second + dir.second };
			if (std::find(map.begin(), map.end(), newPos) != map.end())
				workers.push(Worker{newPos, worker.visited, worker.distance});

			dir = {-dir.second, dir.first};
		}
	}

	return -1;
}

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	int y = 0;
	int x = 0;

	std::string line;
	while (std::getline(file, line))
	{
		x = 0;

		for (auto&& chr : line)
			fullMap[{x++, y}] = chr;

		y++;
	}

	auto pos = std::find_if(fullMap.begin(), fullMap.end(), [](std::pair<std::pair<int, int>, char> pos){ return pos.second == '@'; })->first;
	fullMap[pos] = '#';

	fullMap[{pos.first + 1, pos.second}] = '#';
	fullMap[{pos.first - 1, pos.second}] = '#';
	fullMap[{pos.first, pos.second + 1}] = '#';
	fullMap[{pos.first, pos.second - 1}] = '#';

	fullMap[{pos.first + 1, pos.second + 1}] = '@';
	fullMap[{pos.first - 1, pos.second - 1}] = '@';
	fullMap[{pos.first - 1, pos.second + 1}] = '@';
	fullMap[{pos.first + 1, pos.second - 1}] = '@';

	for (int tX = 0; tX < x / 2; tX++)
	{
		for (int tY = 0; tY < y / 2; tY++)
		{
			std::pair<int, int> pos = {tX, tY};
			char chr = fullMap[pos];

			if (chr != '#')
				map1.push_back(pos);
			if (islower(chr))
				keys1.push_back({chr, pos});
			if (isupper(chr))
				doors1.push_back({(char)tolower(chr), pos});
			if (chr == '@')
				pos1 = pos;
		}
	}

	for (int tX = x / 2; tX < x; tX++)
	{
		for (int tY = 0; tY < y / 2; tY++)
		{
			std::pair<int, int> pos = {tX, tY};
			char chr = fullMap[pos];

			if (chr != '#')
				map2.push_back(pos);
			if (islower(chr))
				keys2.push_back({chr, pos});
			if (isupper(chr))
				doors2.push_back({(char)tolower(chr), pos});
			if (chr == '@')
				pos2 = pos;
		}
	}

	for (int tX = 0; tX < x / 2; tX++)
	{
		for (int tY = y / 2; tY < y; tY++)
		{
			std::pair<int, int> pos = {tX, tY};
			char chr = fullMap[pos];

			if (chr != '#')
				map3.push_back(pos);
			if (islower(chr))
				keys3.push_back({chr, pos});
			if (isupper(chr))
				doors3.push_back({(char)tolower(chr), pos});
			if (chr == '@')
				pos3 = pos;
		}
	}

	for (int tX = x / 2; tX < x; tX++)
	{
		for (int tY = y / 2; tY < y; tY++)
		{
			std::pair<int, int> pos = {tX, tY};
			char chr = fullMap[pos];

			if (chr != '#')
				map4.push_back(pos);
			if (islower(chr))
				keys4.push_back({chr, pos});
			if (isupper(chr))
				doors4.push_back({(char)tolower(chr), pos});
			if (chr == '@')
				pos4 = pos;
		}
	}

	initscr();
	start_color();
	atexit(quit);
	curs_set(0);
	noecho();

	drawMap();

	getch();

	std::set<std::pair<int, int>> set;
	removeDeadEnds(pos1, pos1, set, map1, doors1, keys1);
	set.clear();
	removeDeadEnds(pos2, pos2, set, map2, doors2, keys2);
	set.clear();
	removeDeadEnds(pos3, pos3, set, map3, doors3, keys3);
	set.clear();
	removeDeadEnds(pos4, pos4, set, map4, doors4, keys4);
	drawMapFeatures(keys1, doors1, map1, pos1, x / 2, y / 2, 0, 0);
	drawMapFeatures(keys2, doors2, map2, pos2, x, y / 2, x / 2, 0);
	drawMapFeatures(keys3, doors3, map3, pos3, x / 2, y, 0, y / 2);
	drawMapFeatures(keys4, doors4, map4, pos4, x, y, x / 2, y / 2);

	getch();

	int s1 = solveQuadrant(keys1, doors1, map1, pos1);
	int s2 = solveQuadrant(keys2, doors2, map2, pos2);
	int s3 = solveQuadrant(keys3, doors3, map3, pos3);
	int s4 = solveQuadrant(keys4, doors4, map4, pos4);
	
	mvprintw(0, x + 2, "W1 requires %d steps", s1);
	mvprintw(1, x + 2, "W2 requires %d steps", s2);
	mvprintw(2, x + 2, "W3 requires %d steps", s3);
	mvprintw(3, x + 2, "W4 requires %d steps", s4);
	mvprintw(4, x + 2, "In total    %d steps", s1 + s2 + s3 + s4);

	getch();

	return 0;
}