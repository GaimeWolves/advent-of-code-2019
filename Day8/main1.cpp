#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <algorithm>

std::vector<std::vector<int>> imageData;

constexpr int W = 25;
constexpr int H = 6;

int main(int argc, char** argv)
{
	std::ifstream file("in.txt");

	imageData.push_back({});

	std::string line;
	while (std::getline(file, line))
	{
		for (int i = 0; i < line.length(); i++)
		{
			int num = (int)line[i] - 48; //Convert ASCII number to actual number

			if (imageData[imageData.size() - 1].size() == W * H)
				imageData.push_back({});

			imageData[imageData.size() - 1].push_back(num);
		}
	}

	auto choice = *std::min_element(imageData.begin(), imageData.end(), [](std::vector<int> v1, std::vector<int> v2) { return std::count(v1.begin(), v1.end(), 0) < std::count(v2.begin(), v2.end(), 0); });
	int value = std::count(choice.begin(), choice.end(), 1) * std::count(choice.begin(), choice.end(), 2);
	std::cout << value << std::endl;

	return 0;
}