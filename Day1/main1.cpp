#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char** argv)
{
	std::ifstream file("in.txt");

	int total = 0;

	std::string line;
	while (std::getline(file, line))
	{
		int mass = std::stoi(line);
		total += (mass / 3) - 2;
	}

	std::cout << total << std::endl;

	return 0;
}