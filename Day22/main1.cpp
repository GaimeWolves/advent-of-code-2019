#include <iostream>
#include <fstream>
#include <sstream>

#include <deque>
#include <vector>
#include <algorithm>

std::deque<int> deck;

void dealNew()
{
	std::reverse(deck.begin(), deck.end());
}

void rotate(int num)
{
	if (num < 0)
		std::rotate(deck.rbegin(), deck.rbegin() - num, deck.rend());
	else
		std::rotate(deck.begin(), deck.begin() + num, deck.end());
}

void dealWithIncrement(int inc)
{
	int i = 0;
	std::vector<int> temp = std::vector<int>(deck.size(), -1);
	while(deck.size() > 0)
	{
		if (temp[i] == -1)
		{
			temp[i] = deck.front();
			deck.pop_front();
		}

		i = (i + inc) % temp.size();
	}

	for (auto&& num : temp)
		deck.push_back(num);
}

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	for (int i = 0; i < 10007; i++)
		deck.push_back(i);

	std::string line;
	while (std::getline(file, line))
	{
		if (line.find("new") != std::string::npos) dealNew();
		if (line.find("with") != std::string::npos) dealWithIncrement(std::stoi(line.substr(19)));
		if (line.find("cut") != std::string::npos) rotate(std::stoi(line.substr(3)));
	}

	std::cout << std::find(deck.begin(), deck.end(), 2019) - deck.begin();
	std::cin.get();

	return 0;
}