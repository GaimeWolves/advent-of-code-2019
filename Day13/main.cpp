#include <iostream>
#include <fstream>
#include <sstream>

#include <map>
#include <algorithm>

#include <ncurses.h>
#include <stdlib.h>

#include "computer.hpp"

std::vector<int64_t> ram = std::vector<int64_t>();

std::map<std::pair<int, int>, int> game;

std::pair<int, int> prevPos = {-1, -1};
std::pair<int, int> currentPos = {-1, -1};

int paddlePos = -1;

std::map<int, char> tiles = {
	{0, ' '},
	{1, '#'},
	{2, 'x'},
	{3, '~'},
	{4, '@'}
};

IntCodePC pc;

void quit()
{
	endwin();
}

void traceBall()
{
	auto tmpGame = game;
	if (prevPos.first != -1)
	{
		auto dir = std::make_pair(currentPos.first - prevPos.first, currentPos.second - prevPos.second);

		for (int x = currentPos.first, y = currentPos.second, i = 0; i < 100 ;x += dir.first, y += dir.second, i++)
		{
			if (x > 20)
				return;

			if (game[{x, y}] == 0)
			{
				attron(COLOR_PAIR(1));
				mvaddch(x + 1, y, '.');
				attroff(COLOR_PAIR(1));
			}

			if ((tmpGame[{x + dir.first, y + dir.second}] == 1 || tmpGame[{x + dir.first, y + dir.second}] == 2) && tmpGame[{x + dir.first, y}] == 0 && tmpGame[{x, y + dir.second}] == 0)
			{
				if (tmpGame[{x + dir.first, y + dir.second}] == 2)
				{
					attron(COLOR_PAIR(5));
					mvaddch(x + 1 + dir.first, y + dir.second, '.');
					attroff(COLOR_PAIR(5));
					tmpGame[{x + dir.first, y + dir.second}] = 0;
				}

				dir.first = -dir.first;
				dir.second = -dir.second;
			}

			if ((tmpGame[{x + dir.first, y}] == 1 || tmpGame[{x + dir.first, y}] == 2) && (tmpGame[{x, y + dir.second}] == 1 || tmpGame[{x, y + dir.second}] == 2))
			{
				if (tmpGame[{x + dir.first, y}] == 2)
				{
					attron(COLOR_PAIR(5));
					mvaddch(x + 1 + dir.first, y, '.');
					attroff(COLOR_PAIR(5));
					tmpGame[{x + dir.first, y}] = 0;
				}

				if (tmpGame[{x, y + dir.second}] == 2)
				{
					attron(COLOR_PAIR(5));
					mvaddch(x + 1, y + dir.second, '.');
					attroff(COLOR_PAIR(5));
					tmpGame[{x, y + dir.second}] = 0;
				}

				dir.first = -dir.first;
				dir.second = -dir.second;

				x -= dir.first;
				y -= dir.second;
			}
			else if (tmpGame[{x + dir.first, y}] == 1 || tmpGame[{x + dir.first, y}] == 2)
			{
				if (tmpGame[{x + dir.first, y}] == 2)
				{
					attron(COLOR_PAIR(5));
					mvaddch(x + 1 + dir.first, y, '.');
					attroff(COLOR_PAIR(5));
					tmpGame[{x + dir.first, y}] = 0;
				}

				dir.first = -dir.first;
				x -= dir.first;
				y -= dir.second;
			}
			else if (tmpGame[{x, y + dir.second}] == 1 || tmpGame[{x, y + dir.second}] == 2)
			{
				if (tmpGame[{x, y + dir.second}] == 2)
				{
					attron(COLOR_PAIR(5));
					mvaddch(x + 1, y + dir.second, '.');
					attroff(COLOR_PAIR(5));
					tmpGame[{x, y + dir.second}] = 0;
				}

				dir.second = -dir.second;
				x -= dir.first;
				y -= dir.second;
			}
		}
	}
}

void drawGame()
{
	for (auto&& tile : game)
	{
		if (tile.first.first == -1 || tile.first.second == -1)
			mvprintw(0, 0, "SCORE: %d", tile.second);
		else
		{
			if (tile.second == 4)
				attron(COLOR_PAIR(2));
			else if (tile.second == 2)
				attron(COLOR_PAIR(4));
			else if (tile.second == 1)
				attron(COLOR_PAIR(3));
			mvaddch(tile.first.first + 1, tile.first.second, tiles[tile.second]);
			if (tile.second == 4)
				attroff(COLOR_PAIR(2));
			else if (tile.second == 2)
				attroff(COLOR_PAIR(4));
			else if (tile.second == 1)
				attroff(COLOR_PAIR(3));
		}
	}
	traceBall();
	refresh();
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

	ram[0] = 2;

	pc.reload(ram);
	pc.reset();
	pc.run();

	initscr();
	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);
	init_pair(2, COLOR_YELLOW, COLOR_YELLOW);
	init_pair(3, COLOR_WHITE, COLOR_WHITE);
	init_pair(4, COLOR_BLUE, COLOR_BLUE);
	init_pair(5, COLOR_CYAN, COLOR_CYAN);
	atexit(quit);
	curs_set(0);
	noecho();

	while(true)
	{
		while(pc.awaitsInput())
		{
			if (currentPos.second > paddlePos)
				pc.in(1);
			else if (currentPos.second < paddlePos)
				pc.in(-1);
			else
				pc.in(0);
		}

		while (!pc.awaitsInput())
		{
			if (pc.hasOutput())
			{
				int x = (int)pc.out();
				int y = (int)pc.out();
				int id = (int)pc.out();

				if (id == 4)
				{
					prevPos = currentPos;
					currentPos = {y, x};
				}
				else if (id == 3)
				{
					paddlePos = x;
				}

				game[std::make_pair(y, x)] = id;
			}
		}

		drawGame();
		std::this_thread::sleep_for(std::chrono::milliseconds(30));
	}

	std::cout << std::count_if(game.begin(), game.end(), [](std::pair<std::pair<int, int>, int> tile) { return tile.second == 2; }) << std::endl;

	return 0;
}