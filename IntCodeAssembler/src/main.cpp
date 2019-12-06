#include <iostream>
#include <fstream>

#include "compile.hpp"

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <infile> <outfile>" << std::endl;
		exit(-1);
	}

	std::ifstream file = std::ifstream(argv[1]);
	auto out = compileFile(&file);
	file.close();

	std::ofstream outFile = std::ofstream(argv[2]);
	outFile << out;
	outFile.close();

	return 0;
}