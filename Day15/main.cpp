#include <iostream>
#include <fstream>
#include <sstream>

#include "computer.hpp"

#include <stack>
#include <algorithm>
#include <set>

#include <ncurses.h>
#include <stdlib.h>

std::vector<int64_t> ram = std::vector<int64_t>();

IntCodePC pc;

std::stack<int> movement;
std::stack<int> tried;
std::stack<int> directions;
std::set<std::pair<int, int>> positions;
std::set<std::pair<int, int>> oldPos;
std::set<std::pair<int, int>> walls;
std::pair<int, int> position = {0, 0};
int currentDirection = 2;
int tries = 0;
size_t dist = 0;
size_t max = 0;

void quit()
{
	endwin();
}

static void move()
{
	switch (currentDirection)
	{
	case 1:
		position = std::make_pair(position.first, position.second + 1);
		positions.insert(position);
		break;
	case 2:
		position = std::make_pair(position.first, position.second - 1);
		positions.insert(position);
		break;
	case 3:
		position = std::make_pair(position.first - 1, position.second);
		positions.insert(position);
		break;
	case 4:
		position = std::make_pair(position.first + 1, position.second);
		positions.insert(position);
		break;

	default:
		break;
	}
}

static bool revert()
{
	if (movement.size() == 0)
		return true;
	if (tries >= 5)
	{
		switch (movement.top())
		{
		case 1:
			currentDirection = 2;
			break;
		case 2:
			currentDirection = 1;
			break;
		case 3:
			currentDirection = 4;
			break;
		case 4:
			currentDirection = 3;
			break;
		
		default:
			break;
		}

		pc.in(currentDirection);
		pc.out();

		move();

		currentDirection = movement.top();
		movement.pop();
		tries = tried.top();
		tried.pop();
		return false;
	}
	return true;
}

static void checkBacktracking()
{
	if (movement.size() == 0)
		return;
	switch (currentDirection)
	{
	case 1:
		if (movement.top() == 2)
			currentDirection = currentDirection % 4 + 1;
		break;
	case 2:
		if (movement.top() == 1)
			currentDirection = currentDirection % 4 + 1;
		break;
	case 3:
		if (movement.top() == 4)
			currentDirection = currentDirection % 4 + 1;
		break;
	case 4:
		if (movement.top() == 3)
			currentDirection = currentDirection % 4 + 1;
		break;

	default:
		break;
	}
}

static bool alreadyVisited()
{
	switch (currentDirection)
	{
	case 1:
		return std::find(positions.begin(), positions.end(), std::make_pair(position.first, position.second + 1)) != positions.end();
		break;
	case 2:
		return std::find(positions.begin(), positions.end(), std::make_pair(position.first, position.second - 1)) != positions.end();
		break;
	case 3:
		return std::find(positions.begin(), positions.end(), std::make_pair(position.first - 1, position.second)) != positions.end();
		break;
	case 4:
		return std::find(positions.begin(), positions.end(), std::make_pair(position.first + 1, position.second)) != positions.end();
		break;

	default:
		return false;
		break;
	}
}

void drawMap(bool old)
{
	mvprintw(0, 0, "Distance to oxygen tank (Part 1): %d", dist);
	mvprintw(1, 0, "Max oxygen distance     (Part 2): %d", max);
	if (old)
	{
		for (auto&& pos : oldPos)
		{
			int x = getmaxx(stdscr) / 2 + pos.first;
			int y = getmaxy(stdscr) / 2 + pos.second;
			mvaddch(y, x, ' ');
		}

		attron(COLOR_PAIR(2));
		for (auto&& pos : positions)
		{
			int x = getmaxx(stdscr) / 2 + pos.first;
			int y = getmaxy(stdscr) / 2 + pos.second;
			mvaddch(y, x, 'O');
		}
		attroff(COLOR_PAIR(2));
	}
	else
	{
		for (auto&& pos : positions)
		{
			int x = getmaxx(stdscr) / 2 + pos.first;
			int y = getmaxy(stdscr) / 2 + pos.second;
			mvaddch(y, x, ' ');
		}
	}
	attron(COLOR_PAIR(3));
	for (auto&& pos : walls)
	{
		int x = getmaxx(stdscr) / 2 + pos.first;
		int y = getmaxy(stdscr) / 2 + pos.second;
		mvaddch(y, x, '#');
	}
	attroff(COLOR_PAIR(3));
	int x = getmaxx(stdscr) / 2 + position.first;
	int y = getmaxy(stdscr) / 2 + position.second;
	attron(COLOR_PAIR(1));
	mvaddch(y, x, '@');
	attroff(COLOR_PAIR(1));
	refresh();
}

void addWall()
{
	switch (currentDirection)
	{
	case 1:
		walls.insert(std::make_pair(position.first, position.second + 1));
		break;
	case 2:
		walls.insert(std::make_pair(position.first, position.second - 1));
		break;
	case 3:
		walls.insert(std::make_pair(position.first - 1, position.second));
		break;
	case 4:
		walls.insert(std::make_pair(position.first + 1, position.second));
		break;

	default:
		break;
	}
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
	init_pair(1, COLOR_RED, COLOR_BLACK);
	init_pair(2, COLOR_BLUE, COLOR_BLUE);
	init_pair(3, COLOR_WHITE, COLOR_WHITE);
	atexit(quit);
	curs_set(0);
	noecho();

	pc.reload(ram);
	pc.reset();
	pc.run();

	positions.insert(position);

	bool brk = false;

	while (!brk)
	{
		//Try movement
		if (alreadyVisited())
		{
			tries++;
			currentDirection = currentDirection % 4 + 1;
		}
		else
		{
			pc.in(currentDirection);
			int status = pc.out();
			tries++;
			switch (status)
			{
			case 0:
				addWall();
				currentDirection = currentDirection % 4 + 1;
				break;
			case 1:
				movement.push(currentDirection);
				tried.push(tries);
				tries = 0;

				move();

				break;
			case 2:
				movement.push(currentDirection);
				move();
				getch();
				dist = movement.size();
				brk = true;
				break;

			default:
				break;
			}
		}

		if (revert())
			checkBacktracking();
		drawMap(false);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}

	brk = false;
	movement = std::stack<int>();
	tried = std::stack<int>();
	tries = 0;
	currentDirection = 2;
	oldPos = positions;
	positions.clear();
	positions.insert(position);

	while (!brk)
	{
		if (alreadyVisited())
		{
			tries++;
			currentDirection = currentDirection % 4 + 1;
		}
		else
		{
			pc.in(currentDirection);
			int status = pc.out();
			tries++;
			switch (status)
			{
			case 0:
				addWall();
				currentDirection = currentDirection % 4 + 1;
				break;
			case 1:
				movement.push(currentDirection);
				tried.push(tries);
				if (movement.size() > max)
					max = movement.size();
				tries = 0;

				move();
				break;
			case 2:
				if (tries >= 4)
					brk = true;

				movement.push(currentDirection);
				tried.push(tries);
				if (movement.size() > max)
					max = movement.size();
				tries = 0;

				move();
				break;

			default:
				break;
			}
		}

		if (revert())
			checkBacktracking();
		drawMap(true);
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	
	pc.waitForExit();

	return 0;
}