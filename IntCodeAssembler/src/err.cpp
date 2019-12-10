#include "err.hpp"

#include <iostream>

void throwError(Error err)
{
	if (err.code == NO_ERROR)
		return;

	switch (err.code)
	{
	case SYNTAX_ERROR_INVALID_NUMBER:
		std::cerr << "Invalid number";
		break;
	case SYNTAX_ERROR_INVALID_INSTRUCTION:
		std::cerr << "Invalid instruction";
		break;
	case SYNTAX_ERROR_INVALID_PARAMETER_PREFIX:
		std::cerr << "Invalid parameter prefix";
		break;
	case SYNTAX_ERROR_WRONG_PARAMETER_COUNT:
		std::cerr << "Invalid parameter count";
		break;
	case SYNTAX_ERROR_STRAY_COMMA:
		std::cerr << "Stray comma";
		break;
	
	default:
		break;
	}

	std::cerr << " in line " << err.lineNr << " at position " << (err.index + 1) << std::endl << std::endl;

	std::cerr << err.line << std::endl;

	std::cerr << "\033[1;31m";
	if (err.index != std::string::npos)
		while(err.index-- > 0)
			std::cerr << ' ';

	std::cerr << '^';

	while(err.count-- > 0)
		std::cerr << '~';
	std::cout << "\033[0m";
	
	std::cerr << std::endl;

	exit(err.code);
}