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

std::map<std::pair<int, int>, bool> fullMap;
std::vector<std::pair<int, int>> map;
std::vector<Key> doors;
std::vector<Key> keys;
std::pair<int, int> position;

void quit()
{
	endwin();
}

void removeDeadEnds(std::pair<int, int> pos, std::pair<int, int> prev, std::set<std::pair<int, int>>& visited)
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
			removeDeadEnds(newPos, pos, visited);

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
		mvaddch(pos.first.second, pos.first.first, '#');

	for (auto&& pos : map)
		mvaddch(pos.second, pos.first, '.');

	for (auto&& key : keys)
		mvaddch(key.position.second, key.position.first, key.name);

	for (auto&& door : doors)
		mvaddch(door.position.second, door.position.first, std::toupper(door.name));

	mvaddch(position.second, position.first, '@');
}

inline int parsePos(int x, int y)
{
	return (y << 16) | (x & 0xFFFF);
}

inline std::pair<int, int> parseNode(int node)
{
	return {node & 0xFFFF, node >> 16};
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
		{
			fullMap[{x, y}] = chr != '#';
			if (chr != '#')
				map.push_back({x, y});
			if (std::islower(chr))
				keys.push_back(Key(chr, {x, y}));
			else if (std::isupper(chr))
				doors.push_back(Key(std::tolower(chr), {x, y}));
			else if (chr == '@')
				position = {x, y};
			x++;
		}

		y++;
	}

	initscr();
	start_color();
	atexit(quit);
	curs_set(0);
	noecho();

	drawMap();

	getch();

	std::set<std::pair<int, int>> set;
	removeDeadEnds(position, position, set);
	drawMap();

	getch();

	std::queue<Worker> workers;
	workers.push({ position, 0, 0 });

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
			{
				mvprintw(0, x + 2, "%d steps", worker.distance);
				break;
			}
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

	getch();

	return 0;
}