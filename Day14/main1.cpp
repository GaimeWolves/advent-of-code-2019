#include <iostream>
#include <fstream>
#include <sstream>

#include <vector>
#include <map>
#include <algorithm>

std::string trim(const std::string& str, const std::string& whitespace = " \t")
{
    const auto strBegin = str.find_first_not_of(whitespace);
    if (strBegin == std::string::npos)
        return "";

    const auto strEnd = str.find_last_not_of(whitespace);
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

struct Reaction
{
	std::vector<std::pair<std::string, int64_t>> educts;
	std::pair<std::string, int64_t> product;
};

std::vector<Reaction> reactions;

void makeEduct(std::string educt, int64_t amount, std::map<std::string, int64_t>& reserves)
{
	if (educt == "ORE")
	{
		reserves[educt] += amount;
		return;
	}

	if (reserves[educt] >= amount)
	{
		reserves[educt] -= amount;
		return;
	}

	auto reaction = std::find_if(reactions.begin(), reactions.end(), [educt](Reaction r) { return r.product.first == educt; });

	while(reserves[educt] < amount)
	{
		for (auto&& educt : reaction->educts)
			makeEduct(educt.first, educt.second, reserves);

		reserves[educt] += reaction->product.second;
	}

	reserves[educt] -= amount;
}

int findOreAmount()
{
	std::map<std::string, int64_t> reserves;
	auto fuelReaction = std::find_if(reactions.begin(), reactions.end(), [](Reaction r) { return r.product.first == "FUEL"; });

	for (auto&& educt : fuelReaction->educts)
		makeEduct(educt.first, educt.second, reserves);

	return reserves["ORE"];
}

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	std::string line;
	while (std::getline(file, line))
	{
		Reaction reaction;

		std::istringstream reactionStream(line);
		std::string reactionSide;
		std::getline(reactionStream, reactionSide, '=');

		std::istringstream eductsStream(reactionSide);
		std::string chemical;
		while(std::getline(eductsStream, chemical, ','))
		{
			size_t* pos = (size_t*)malloc(sizeof(size_t));
			int64_t amount = std::stoll(chemical, pos);
			chemical = chemical.substr(*pos);
			chemical = trim(chemical);
			reaction.educts.push_back({chemical, amount});
			free((void*)pos);
		}

		std::getline(reactionStream, reactionSide, '=');
		chemical = reactionSide;
		chemical = chemical.substr(1);
		size_t* pos = (size_t*)malloc(sizeof(size_t));
		int64_t amount = std::stoll(chemical, pos);
		chemical = chemical.substr(*pos);
		chemical = trim(chemical);
		reaction.product = {chemical, amount};
		free((void*)pos);

		reactions.push_back(reaction);
	}

	std::cout << findOreAmount() << std::endl;

	return 0;
}