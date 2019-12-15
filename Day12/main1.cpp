#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <cmath>

struct Moon
{
	int x, y, z;
	int vX, vY, vZ;

	Moon(int _x, int _y, int _z)
		: x(_x), y(_y), z(_z)
	{
		vX = 0;
		vY = 0;
		vZ = 0;
	}

	void applyAcceleration(Moon& other)
	{
		if (x > other.x)
			vX--;
		else if (x < other.x)
			vX++;

		if (y > other.y)
			vY--;
		else if (y < other.y)
			vY++;

		if (z > other.z)
			vZ--;
		else if (z < other.z)
			vZ++;
	}

	void applyVelocity()
	{
		x += vX;
		y += vY;
		z += vZ;
	}

	int getEnergy()
	{
		return (std::abs(x) + std::abs(y) + std::abs(z)) * (std::abs(vX) + std::abs(vY) + std::abs(vZ));
	}
};

std::vector<Moon> moons;

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	std::string line;
	while (std::getline(file, line))
	{
		std::istringstream iss(line);
		std::string num;

		int x;
		int y;
		int z;

		std::getline(iss, num, '=');
		std::getline(iss, num, '=');
		x = std::stoi(num);
		std::getline(iss, num, '=');
		y = std::stoi(num);
		std::getline(iss, num, '=');
		z = std::stoi(num);

		moons.push_back(Moon(x, y, z));
	}

	for (int i = 1; i <= 1000; i++)
	{
		for (auto&& moonA : moons)
			for (auto&& moonB : moons)
				if (&moonA != &moonB)
					moonA.applyAcceleration(moonB);

		int sum = 0;
		for (auto&& moon : moons)
		{
			moon.applyVelocity();
			sum += moon.getEnergy();
		}

		std::cout << i << ": " << sum << std::endl;
	}

	return 0;
}