#include <boost/config.hpp>
#include <iostream>
#include <fstream>
#include <sstream>

#include <map>

#include <boost/graph/graph_traits.hpp>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/property_map/property_map.hpp>

int toNode(int x, int y)
{
	return (y << 16) | (x & 0xFFFF);
}

std::pair<int, int> toPos(int node)
{
	return {node & 0xFFFF, node >> 16};
}

std::map<std::pair<int, int>, char> fullMap;

std::pair<int, int> start, end;
std::map<std::string, std::vector<std::pair<int, int>>> portals;

int main(int argc, char** argv)
{
	std::ifstream file(argv[1]);

	int maxY = 0;
	int maxX = 0;

	std::string line;
	while (std::getline(file, line))
	{
		maxX = 0;
		for (auto&& chr : line)
			fullMap[{maxX++, maxY}] = chr;
		maxY++;
	}

	for (auto&& pos : fullMap)
	{
		if (pos.second == '.')
		{
			std::pair<int, int> left = {pos.first.first - 1, pos.first.second};
			std::pair<int, int> right = {pos.first.first + 1, pos.first.second};
			std::pair<int, int> up = {pos.first.first, pos.first.second - 1};
			std::pair<int, int> down = {pos.first.first, pos.first.second + 1};

			std::string ident = "";

			if (std::isupper(fullMap[left]))
				ident = {fullMap[{left.first - 1, left.second}], fullMap[left]};
			if (std::isupper(fullMap[right]))
				ident = {fullMap[right], fullMap[{right.first + 1, right.second}]};
			if (std::isupper(fullMap[up]))
				ident = {fullMap[{up.first, up.second - 1}], fullMap[up]};
			if (std::isupper(fullMap[down]))
				ident = {fullMap[down], fullMap[{down.first, down.second + 1}]};

			if (ident != "")
			{
				if (ident == "AA")
					start = pos.first;
				else if (ident == "ZZ")
					end = pos.first;
				else
				{
					if (portals.find(ident) != portals.end())
						portals[ident].push_back(pos.first);
					else
						portals[ident] = { pos.first };
				}
			}
		}
	}

	int nodes = 0;

	std::vector<std::pair<int, int>> edges;
	for (auto&& portal : portals)
		edges.push_back({ toNode(portal.second[0].first, portal.second[0].second), toNode(portal.second[1].first, portal.second[1].second) });

	for (int x = 0; x < maxX; x++)
	{
		for (int y = 0; y < maxY; y++)
		{
			if (fullMap[{x, y}] == '.')
			{
				nodes++;

				if (fullMap[{x + 1, y}] == '.')
					edges.push_back({ toNode(x, y), toNode(x + 1, y) });
				
				if (fullMap[{x, y + 1}] == '.')
					edges.push_back({ toNode(x, y), toNode(x, y + 1) });
			}
		}
	}

	typedef boost::adjacency_list<boost::vecS, boost::vecS, boost::undirectedS, boost::no_property, boost::property<boost::edge_weight_t, int>> graph_t;
	typedef boost::graph_traits<graph_t>::vertex_descriptor vertex_descriptor;

	std::vector<int> weights = std::vector<int>(edges.size(), 1);
	graph_t g(edges.begin(), edges.end(), weights.begin(), nodes);
	std::vector<vertex_descriptor> p(num_vertices(g));
	std::vector<int> d(num_vertices(g));
	vertex_descriptor startNode = vertex(toNode(start.first, start.second), g);

	boost::dijkstra_shortest_paths(
		g,
		startNode,
		boost::predecessor_map(boost::make_iterator_property_map(p.begin(), get(boost::vertex_index, g)))
			.distance_map(boost::make_iterator_property_map(d.begin(), get(boost::vertex_index, g)))
	);

	vertex_descriptor goalNode = vertex(toNode(end.first, end.second), g);
	std::vector<boost::graph_traits<graph_t>::vertex_descriptor > path;
	boost::graph_traits<graph_t>::vertex_descriptor current = goalNode;

	while(current != startNode)
	{
		path.push_back(current);
		current = p[current];
	}

	std::cout << "Steps required " << path.size() << "\n";
	std::cin.get();

	return 0;
}