#include "point2d.h"

#include <algorithm>
#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/detail/adjacency_list.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>

#include <fmt/core.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

using EdgeWeight = boost::property<boost::edge_weight_t, unsigned>;
using VertexDistance = boost::property<boost::vertex_distance_t, unsigned>;
using Graph = boost::adjacency_list<boost::vecS, boost::vecS, boost::directedS, VertexDistance, EdgeWeight>;
using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;

using World = std::unordered_map<Point, Vertex>;
using Doors = std::unordered_map<Point, unsigned>;

[[maybe_unused]] void dump(World const& world, Doors const& doors)
{
    unsigned max_x = 0, max_y = 0;

    for (auto const& [px, _] : world) {
        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
    }
    ++max_x;
    ++max_y;

    for (unsigned y = 0; y < max_y; ++y) {
        for (unsigned x = 0; x < max_x; ++x) {
            if (doors.contains({x, y})) {
                fmt::print("{:c}", doors.at({x,y}));
            } else if (world.contains({x, y})) {
                fmt::print(".");
            } else {
                fmt::print("#");
            }
        }
        fmt::print("#\n");
    }

    fmt::print("{}\n", std::string(max_x + 1, '#'));
}

void part1(World const& world, Doors const& doors, Graph g)
{
    std::map<unsigned char, std::map<unsigned char, unsigned>> distances;

    std::string sequence;
    for (auto const& [px, d] : doors) {
        sequence.push_back(d);

        auto dist_map = boost::get(boost::vertex_distance, g);
        std::vector<Vertex> pred(boost::num_vertices(g));
        boost::dijkstra_shortest_paths(g, world.at(px), boost::distance_map(dist_map).predecessor_map(&pred[0]));

        for (auto const& [px2, dst] : doors) {
            if (d == dst) continue;
            distances[d][dst] = dist_map[world.at(px2)];
        }
    }

    // for (auto const& [from, dst] : distances) {
    //     for (auto const& [to, length] : dst) {
    //         fmt::print("{:c} -> {:c} = {}\n", from, to, length);
    //     }
    //     fmt::print("\n");
    // }

    std::sort(sequence.begin(), sequence.end());
    sequence.erase(sequence.begin()); // remove '0'; we always start at '0'

    unsigned minimum = -1;
    do {
        unsigned path = 0;
        unsigned char last = '0';
        for (unsigned char d : sequence) {
            path += distances.at(last).at(d);
            last = d;
        }
        // fmt::print("path 0{} -> {}\n", sequence, path);
        minimum = std::min(minimum, path);
    } while(std::next_permutation(sequence.begin(), sequence.end()));

    fmt::print("1: {}\n", minimum);
}

int main()
{
    World world;
    Doors doors;

    unsigned y = 0;
    Graph g;

    std::string line;
    while(std::getline(std::cin, line)) {
        if(line.empty()) break;

        unsigned x = 0;
        for (unsigned char c : line) {
            const Point px{x, y};
            if (c >= '0' && c <= '9') {
                doors.insert({px, c});
                c = '.';
            }
            if (c == '.') {
                world.insert({px, boost::add_vertex(VertexDistance{1}, g)});
            }
            ++x;
        }
        ++y;
    }

    for (auto const& [px, from] : world) {
        for (auto const& dir : {Gfx_2d::Up, Gfx_2d::Down, Gfx_2d::Left, Gfx_2d::Right}) {
            const Point p2 = px + dir;
            auto it = world.find(p2);
            if (it != world.end()) {
                boost::add_edge(from, it->second, EdgeWeight{1}, g);
            }
        }
    }

    part1(world, doors, g);

    return 0;
}
