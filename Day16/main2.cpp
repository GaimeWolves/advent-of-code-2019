#include <iostream>
#include <fstream>
#include <sstream>

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	std::string line;
	while (std::getline(file, line))
	{

	}

	return 0;
}