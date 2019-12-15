#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <cmath>

#include <algorithm>
#include <numeric>

struct Moon
{
	int x, y, z;
	int initX, initY, initZ;
	int vX, vY, vZ;

	Moon(int _x, int _y, int _z)
		: x(_x), y(_y), z(_z), initX(_x), initY(_y), initZ(_z), vX(0), vY(0), vZ(0)
	{}

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

	bool hasPreviousStateX()
	{
		return x == initX && vX == 0;
	}

	bool hasPreviousStateY()
	{
		return y == initY && vY == 0;
	}

	bool hasPreviousStateZ()
	{
		return z == initZ && vZ == 0;
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

	int64_t xCycle = -1;
	int64_t yCycle = -1;
	int64_t zCycle = -1;

	int64_t iter = 0;
	while(true)
	{
		iter++;
		for (auto&& moonA : moons)
			for (auto&& moonB : moons)
				if (&moonA != &moonB)
					moonA.applyAcceleration(moonB);

		for (auto&& moon : moons)
			moon.applyVelocity();

		if (xCycle == -1 && std::all_of(moons.begin(), moons.end(), [](Moon& m) { return m.hasPreviousStateX(); }))
		{
			xCycle = iter;
			std::cout << xCycle << std::endl;
		}

		if (yCycle == -1 && std::all_of(moons.begin(), moons.end(), [](Moon& m) { return m.hasPreviousStateY(); }))
		{
			yCycle = iter;
			std::cout << yCycle << std::endl;
		}

		if (zCycle == -1 && std::all_of(moons.begin(), moons.end(), [](Moon& m) { return m.hasPreviousStateZ(); }))
		{
			zCycle = iter;
			std::cout << zCycle << std::endl;
		}

		if (xCycle > 0 && yCycle > 0 && zCycle > 0)
			break;
	}

	std::cout << std::lcm(std::lcm(xCycle, yCycle), zCycle) << std::endl;

	return 0;
}