#pragma once

#include <string>
#include <cstdint>

#define NO_ERROR 0
#define SYNTAX_ERROR_INVALID_INSTRUCTION 1
#define SYNTAX_ERROR_WRONG_PARAMETER_COUNT 2
#define SYNTAX_ERROR_INVALID_PARAMETER_PREFIX 3
#define SYNTAX_ERROR_INVALID_NUMBER 4
#define SYNTAX_ERROR_STRAY_COMMA 5

struct Error
{
	size_t code;
	std::string line;
	size_t lineNr;
	size_t index;
	size_t count;
};

void throwError(Error err);