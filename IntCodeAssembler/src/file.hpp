#pragma once

#include <fstream>

std::ifstream tryOpenFile(char* filePath);
void tryWriteFile(char* filePath, std::string content);