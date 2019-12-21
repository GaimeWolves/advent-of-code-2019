#include <iostream>
#include <fstream>
#include <sstream>

#include <ncurses.h>
#include <stack>

#include "computer.hpp"

std::vector<int64_t> ram = std::vector<int64_t>();

IntCodePC pc;

void quit()
{
	endwin();
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

	int cnt = 0;

	for (int x = 0; x < 50; x++)
	{
		for (int y = 0; y < 50; y++)
		{
			pc.reload(ram);
			pc.reset();
			pc.run();

			pc.in(x);
			pc.in(y);
			int out = pc.out();

			pc.waitForExit();

			if (out)
				mvaddch(y, x, '#');
			else
				mvaddch(y, x, '.');

			cnt += out;
		}
	}

	mvprintw(0, 51, "Drones being pulled: %d", cnt);
	getch();

	std::stack<std::pair<int, int>> last;

	int yMin = 0;
	int yMax = 10000;
	int y = 5000;
	int txtY = 2;
	while(true)
	{
		int x = 0;
		int cnt = 0;
		while(true)
		{
			pc.reload(ram);
			pc.reset();
			pc.run();

			pc.in(x);
			pc.in(y);
			if (pc.out())
			{
				pc.waitForExit();
				break;
			}

			x++;

			pc.waitForExit();
		}

		while(true)
		{
			pc.reload(ram);
			pc.reset();
			pc.run();

			pc.in(x + 99);
			pc.in(y);
			if (!pc.out())
			{
				pc.waitForExit();
				break;
			}

			pc.waitForExit();

			pc.reload(ram);
			pc.reset();
			pc.run();

			pc.in(x);
			pc.in(y + 99);
			if (pc.out())
				cnt++;

			pc.waitForExit();
			x++;
		}

		if (cnt == 1)
		{
			last.push({x, y});
			break;
		}

		if (cnt == 0)
			yMin = y;
		else
			yMax = y;

		mvprintw(txtY++, 51, "With y = %d there are %d possible 100x100 squares", y, cnt);
		y = (yMin + yMax) / 2;
		refresh();
	}

	mvprintw(txtY++, 51, "With y = %d there is 1 possible 100x100 square", y);
	refresh();

	while (y > yMin)
	{
		int x = 0;
		int cnt = 0;
		while(true)
		{
			pc.reload(ram);
			pc.reset();
			pc.run();

			pc.in(x);
			pc.in(y);
			if (pc.out())
			{
				pc.waitForExit();
				break;
			}

			x++;

			pc.waitForExit();
		}

		while(true)
		{
			pc.reload(ram);
			pc.reset();
			pc.run();

			pc.in(x + 99);
			pc.in(y);
			if (!pc.out())
			{
				pc.waitForExit();
				break;
			}

			pc.waitForExit();

			pc.reload(ram);
			pc.reset();
			pc.run();

			pc.in(x);
			pc.in(y + 99);
			if (pc.out())
				cnt++;

			pc.waitForExit();
			x++;
		}

		if (cnt == 1)
		{
			mvprintw(txtY++, 51, "With y = %d a square is still possible", y);
			last.push({x - 1, y});
		}
		else
		{
			mvprintw(txtY++, 51, "With y = %d a square is not possible", y);
		}
		
		refresh();
		y--;
	}

	auto pos = last.top();
	mvprintw(txtY++, 51, "At %d, %d the first 100x100 square is possible", pos.first, pos.second);
	mvprintw(txtY++, 51, "The result is %d", (pos.first * 10000 + pos.second));
	refresh();
	std::cin.get();

	return 0;
}