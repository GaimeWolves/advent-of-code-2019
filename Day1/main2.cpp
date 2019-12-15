#include <iostream>
#include <fstream>
#include <sstream>

int getFuelOfModule(int mass)
{
	int value = (mass / 3) - 2;
	if (value <= 0)
		return 0;
	return value + getFuelOfModule(value);
}

int main(int argc, char** argv)
{
	std::ifstream file("in.txt");

	int total = 0;

	std::string line;
	while (std::getline(file, line))
	{
		int mass = std::stoi(line);
		total += getFuelOfModule(mass);
	}

	std::cout << total << std::endl;

	return 0;
}