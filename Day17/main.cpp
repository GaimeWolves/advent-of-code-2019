#include <iostream>
#include <fstream>
#include <sstream>

#include "computer.hpp"
#include <ncurses.h>
#include <map>
#include <algorithm>

std::vector<int64_t> ram = std::vector<int64_t>();

IntCodePC pc;

std::map<std::pair<int, int>, char> map;

void quit()
{
	endwin();
}

bool checkMethod(std::vector<std::string> method)
{
	int size = 0;
	std::for_each(method.begin(), method.end(), [&size](std::string instr) { size += instr.size() + 1; });
	size--;
	return size <= 20;
}

std::vector<std::string> makeConfig(std::vector<std::string> path, std::vector<std::string> a, std::vector<std::string> b, std::vector<std::string> c)
{
	std::vector<std::string> config;

	int i = 0;
	while(i < (int)path.size())
	{
		if (std::vector<std::string>(path.begin() + i, path.begin() + i + a.size()) == a)
		{
			config.push_back("A");
			i += a.size();
		}
		else if (std::vector<std::string>(path.begin() + i, path.begin() + i + b.size()) == b)
		{
			config.push_back("B");
			i += b.size();
		}
		else if (std::vector<std::string>(path.begin() + i, path.begin() + i + c.size()) == c)
		{
			config.push_back("C");
			i += c.size();
		}
		else
		{
			config.clear();
			return config;
		}
	}

	if (!checkMethod(config))
	{
		config.clear();
		return config;
	}

	return config;
}

std::vector<std::vector<std::string>> getMethods(std::vector<std::string> path)
{
	for (int aLen = 1; aLen < (int)path.size(); aLen++)
	{
		for (int bLen = 1; bLen < (int)path.size(); bLen++)
		{
			for (int cLen = 1; cLen < (int)path.size(); cLen++)
			{
				for (int aCnt = 1; ; aCnt++)
				{
					if (aLen * aCnt + bLen + cLen > (int)path.size())
						break;

					for (int bCnt = 1; ; bCnt++)
					{
						if (aLen * aCnt + bLen * bCnt + cLen > (int)path.size())
							break;

						auto aMethod = std::vector<std::string>(path.begin(), path.begin() + aLen);
						auto bMethod = std::vector<std::string>(path.begin() + aLen * aCnt, path.begin() + aLen * aCnt + bLen);
						auto cMethod = std::vector<std::string>(path.begin() + aLen * aCnt + bLen * bCnt, path.begin() + aLen * aCnt + bLen * bCnt + cLen);

						if (!checkMethod(aMethod) || !checkMethod(bMethod) || !checkMethod(cMethod))
							continue;

						auto config = makeConfig(path, aMethod, bMethod, cMethod);
						if (config.size() > 0)
							return {config, aMethod, bMethod, cMethod};
					}
				}
			}
		}
	}

	return {};
}

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

	initscr();
	start_color();
	atexit(quit);
	curs_set(0);
	noecho();

	pc.reload(ram);
	pc.reset();
	pc.run();

	int x = 0;
	int y = 0;

	int maxX = 0;
	int maxY = 0;
	int outCnt = 0;

	std::pair<int, int> pos;

	while(pc.running())
	{
		while(pc.hasOutput())
		{
			char chr = (char)pc.out();
			outCnt++;
			if (chr == '\n')
			{
				y++;
				if (maxX == 0)
					maxX = x - 1;
				x = 0;
			}
			else
			{
				if (chr == '^')
					pos = std::make_pair(x, y);
				mvaddch(y, x, chr);
				map[std::make_pair(x, y)] = chr;
				x++;
			}
		}
	}
	pc.waitForExit();

	maxY = y;
	
	std::vector<std::pair<int, int>> crossings;
	int sum = 0;

	for (int x = 1; x < maxX; x++)
	{
		for(int y = 1; y < maxY; y++)
		{
			if (map[std::make_pair(x, y)] == '#')
			{
				if (map[std::make_pair(x + 1, y)] == '#' &&
					map[std::make_pair(x - 1, y)] == '#' &&
					map[std::make_pair(x, y + 1)] == '#' &&
					map[std::make_pair(x, y - 1)] == '#')
				{
					crossings.push_back(std::make_pair(x, y));
					sum += x * y;
				}
			}
		}
	}

	for (auto&& pos : crossings)
		mvaddch(pos.second, pos.first, 'O');

	mvprintw(maxY, 0, "Part 1: %d", sum);

	std::pair<int, int> direction = {0, -1};

	std::vector<std::string> path;
	int steps = 0;

	while(true)
	{
		std::pair<int, int> leftDir = { direction.second, -direction.first };

		bool up = map[{pos.first + direction.first, pos.second + direction.second}] == '#';

		if (up)
		{
			steps++;
			pos.first += direction.first;
			pos.second += direction.second;
		}
		else
		{
			bool left = map[{pos.first + leftDir.first, pos.second + leftDir.second}] == '#';
			bool right = map[{pos.first - leftDir.first, pos.second - leftDir.second}] == '#';

			if (steps > 0)
			{
				path.push_back(std::to_string(steps));
				steps = 0;
			}

			if (!left && !right)
				break;
			else if (left)
			{
				path.push_back("L");
				direction = leftDir;
			}
			else
			{
				path.push_back("R");
				direction = {-leftDir.first, -leftDir.second};
			}
		}
	}

	std::string pathString;
	std::for_each(path.begin(), path.end(), [&pathString](std::string instr){ pathString += "," + instr; });

	mvprintw(maxY + 1, 0, "Full path: %s", pathString.substr(1).c_str());

	auto config = getMethods(path);

	std::string m = "", a = "", b = "", c = "";

	std::for_each(config[0].begin(), config[0].end(), [&m](std::string i){ m += "," + i; });
	m = m.substr(1);

	std::for_each(config[1].begin(), config[1].end(), [&a](std::string i){ a += "," + i; });
	a = a.substr(1);

	std::for_each(config[2].begin(), config[2].end(), [&b](std::string i){ b += "," + i; });
	b = b.substr(1);

	std::for_each(config[3].begin(), config[3].end(), [&c](std::string i){ c += "," + i; });
	c = c.substr(1);

	getch();

	ram[0] = 2;
	pc.reload(ram);
	pc.reset();
	pc.run();

	int i = 0;
	x = 0;
	y = 0;
	while(i < outCnt)
	{
		while(pc.hasOutput())
		{
			char chr = (char)pc.out();
			i++;
			if (chr == '\n')
			{
				y++;
				if (maxX == 0)
					maxX = x - 1;
				x = 0;
			}
			else
			{
				if (chr == '^')
					pos = std::make_pair(x, y);
				mvaddch(y, x, chr);
				map[std::make_pair(x, y)] = chr;
				x++;
			}
		}
	}

	std::string prompt = "";
	while(true)
	{
		char chr = (char)pc.out();
		if (chr == '\n')
			break;
		prompt += chr;
	}
	mvprintw(maxY + 3, 0, "%s %s", prompt.c_str(), m.c_str());

	mvprintw(maxY + 4, 0, ">");
	getch();

	for (auto&& chr : m)
		pc.in((int64_t)chr);
	pc.in((int64_t)'\n');

	prompt = "";
	while(true)
	{
		char chr = (char)pc.out();
		if (chr == '\n')
			break;
		prompt += chr;
	}
	mvprintw(maxY + 4, 0, "%s %s", prompt.c_str(), a.c_str());

	mvprintw(maxY + 5, 0, ">");
	getch();

	for (auto&& chr : a)
		pc.in((int64_t)chr);
	pc.in((int64_t)'\n');

	prompt = "";
	while(true)
	{
		char chr = (char)pc.out();
		if (chr == '\n')
			break;
		prompt += chr;
	}
	mvprintw(maxY + 5, 0, "%s %s", prompt.c_str(), b.c_str());

	mvprintw(maxY + 6, 0, ">");
	getch();

	for (auto&& chr : b)
		pc.in((int64_t)chr);
	pc.in((int64_t)'\n');

	prompt = "";
	while(true)
	{
		char chr = (char)pc.out();
		if (chr == '\n')
			break;
		prompt += chr;
	}
	mvprintw(maxY + 6, 0, "%s %s", prompt.c_str(), c.c_str());

	mvprintw(maxY + 7, 0, ">");
	getch();

	for (auto&& chr : c)
		pc.in((int64_t)chr);
	pc.in((int64_t)'\n');

	prompt = "";
	while(true)
	{
		char chr = (char)pc.out();
		if (chr == '\n')
			break;
		prompt += chr;
	}
	mvprintw(maxY + 7, 0, "%s %s", prompt.c_str(), "y");

	mvprintw(maxY + 8, 0, "Press to step animation...");
	getch();

	pc.in((int64_t)'y');
	pc.in((int64_t)'\n');
	pc.out();

	while(pc.running())
	{
		i = 0;
		x = 0;
		y = 0;
		while(i < outCnt)
		{
			i++;
			char chr = (char)pc.out();
			if (chr == '\n')
			{
				y++;
				x = 0;
			}
			else
			{
				mvaddch(y, x, chr);
				map[std::make_pair(x, y)] = chr;
				x++;
			}
		}

		getch();
	}

	int64_t dust = pc.out();

	mvprintw(maxY + 8, 0, "Dust collected: %ld                          ", dust);
	getch();
	while(true);

	return 0;
}