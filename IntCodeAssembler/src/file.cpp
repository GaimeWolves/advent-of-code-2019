#include "file.hpp"
#include <iostream>

std::ifstream tryOpenFile(char* filePath)
{
	try
	{
		return std::ifstream(filePath);
	}
	catch(...)
	{
		std::cerr << "Cannot read file" << std::endl;
		exit(-1);
	}
}

void tryWriteFile(char* filePath, std::string content)
{
	try
	{
		std::ofstream fstr = std::ofstream(filePath);
		fstr << content;
		fstr.close();
	}
	catch(...)
	{
		std::cerr << "Cannot write file" << std::endl;
		exit(-1);
	}
	
}