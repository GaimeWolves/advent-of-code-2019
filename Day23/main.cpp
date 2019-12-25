#include <iostream>
#include <fstream>
#include <sstream>

#include <queue>
#include <set>
#include <algorithm>

#include "computer.hpp"

std::vector<int64_t> ram = std::vector<int64_t>();

struct Packet
{
	int64_t addr;
	int64_t x;
	int64_t y;

	bool operator==(Packet& other)
	{
		return addr == other.addr && x == other.x && y == other.y;
	}
};

struct NIC
{
	IntCodePC pc;
	std::queue<Packet> in;

	NIC() {}

	NIC(NIC&& nic)
	{
		pc = nic.pc;
		in = nic.in;
	}
};

std::vector<NIC*> pcs;
Packet NATPacket;
Packet oldNATPacket;

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
			int64_t intCode = std::stoll(intCodeLine);
			ram.push_back(intCode);
		}
	}

	for(int64_t i = 0; i < 50; i++)
	{
		pcs.push_back(new NIC());
		pcs[i]->pc.reload(ram);
		pcs[i]->pc.reset();
		pcs[i]->pc.run();
		pcs[i]->pc.in(i);
	}

	while(true)
	{
		for (int i = 0; i < 50; i++)
		{
			NIC* nic = pcs[i];
			
			if (nic->pc.awaitsInput())
			{
				if (nic->in.empty())
					nic->pc.in(-1);
				else
				{
					Packet pkt = nic->in.front();
					nic->in.pop();
					nic->pc.in(pkt.x);
					nic->pc.in(pkt.y);
				}
			}

			if (nic->pc.hasOutput())
			{
				int64_t addr = nic->pc.out();
				int64_t x = nic->pc.out();
				int64_t y = nic->pc.out();

				std::cout << "PACKET: " << addr << "  " << x << "  " << y << "\n";

				if (addr == 255)
					NATPacket = {addr, x, y};
				else
					pcs[addr]->in.push({addr, x, y});
			}
		}

		bool allAwaitInput = std::all_of(pcs.begin(), pcs.end(), [](NIC* nic){ return nic->pc.awaitsInput(); });
		if (allAwaitInput)
		{
			bool allEmpty = std::all_of(pcs.begin(), pcs.end(), [](NIC* nic){ return nic->in.empty(); });
			if (allEmpty)
			{
				if (oldNATPacket == NATPacket)
				{
					std::cout << "Twice PACKET: " << NATPacket.addr << "  " << NATPacket.x << "  " << NATPacket.y << "\n";
					std::cin.get();
					return 0;
				}

				std::cout << "All idle sending to PC0: " << NATPacket.x << "  " << NATPacket.y << "\n";
				oldNATPacket = NATPacket;
				pcs[0]->pc.in(NATPacket.x);
				pcs[0]->pc.in(NATPacket.y);
			}
		}
	}

	return 0;
}