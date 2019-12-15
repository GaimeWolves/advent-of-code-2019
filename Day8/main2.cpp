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

	for (int y = 0; y < H; y++)
	{
		for (int x = 0; x < W; x++)
		{
			int index = y * W + x;
			int imageValue = 2;

			for (auto&& layer : imageData)
			{
				if (layer[index] != 2)
				{
					imageValue = layer[index];
					break;
				}
			}

			std::cout << (imageValue == 0 ? " " : "X");
		}
		std::cout << std::endl;
	}

	return 0;
}