#include <iostream>
#include <fstream>
#include <sstream>

#include "computer.hpp"

std::vector<int> ram = std::vector<int>();

IntCodePC pcA = IntCodePC();
IntCodePC pcB = IntCodePC();
IntCodePC pcC = IntCodePC();
IntCodePC pcD = IntCodePC();
IntCodePC pcE = IntCodePC();

int main(int argc, char** argv)
{
	if (argc != 2)
	{
		std::cerr << "Usage: " << argv[0] << " <file>" << std::endl;
		exit(-1);
	}

	std::ifstream file(argv[1]);

	std::string line;
	while (std::getline(file, line))
	{
		std::string intCodeLine;
		std::istringstream iss(line);

		while(std::getline(iss, intCodeLine, ','))
		{
			int intCode = std::stoi(intCodeLine);
			ram.push_back(intCode);
		}
	}

	int best = 0;
	for (int A = 5; A < 10; A++)
	{
		for (int B = 5; B < 10; B++)
		{
			if (B == A) continue;
			for (int C = 5; C < 10; C++)
			{
				if (C == B || C == A) continue;
				for (int D = 5; D < 10; D++)
				{
					if (D == C || D == B || D == A) continue;
					for (int E = 5; E < 10; E++)
					{
						if (E == D || E == C || E == B || E == A) continue;

						pcA.reload(ram);
						pcA.reset();
						pcB.reload(ram);
						pcB.reset();
						pcC.reload(ram);
						pcC.reset();
						pcD.reload(ram);
						pcD.reset();
						pcE.reload(ram);
						pcE.reset();

						pcA.run();
						pcB.run();
						pcC.run();
						pcD.run();
						pcE.run();
	
						pcA.in(A);
						pcA.in(0);
						pcB.in(B);
						pcC.in(C);
						pcD.in(D);
						pcE.in(E);

						while(pcE.running())
						{
							if (pcA.hasOutput())
								pcB.in(pcA.out());
							if (pcB.hasOutput())
								pcC.in(pcB.out());
							if (pcC.hasOutput())
								pcD.in(pcC.out());
							if (pcD.hasOutput())
								pcE.in(pcD.out());
							if (pcA.running() && pcE.hasOutput())
								pcA.in(pcE.out());
						}

						int out = pcE.out();

						if (out > best)
							best = out;

						std::cout << out << std::endl;

						pcA.waitForExit();
						pcB.waitForExit();
						pcC.waitForExit();
						pcD.waitForExit();
						pcE.waitForExit();

					}
				}
			}
		}
	}

	std::cout << "Best " << best << std::endl;

	return 0;
}