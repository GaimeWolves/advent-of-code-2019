#include <iostream>
#include <fstream>

#include "compile.hpp"
#include "file.hpp"

int main(int argc, char** argv)
{
	if (argc != 3)
	{
		std::cerr << "Usage: " << argv[0] << " <infile> <outfile>" << std::endl;
		exit(-1);
	}

	std::ifstream file = tryOpenFile(argv[1]);
	auto out = compileFile(&file);
	file.close();

	tryWriteFile(argv[2], out);

	return 0;
}