#include "point2d.h"

#include <boost/regex.hpp>

#include <boost/regex/v5/regex.hpp>
#include <boost/regex/v5/regex_match.hpp>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include <unordered_set>

using Coord = unsigned;
using Point = Gfx_2d::Point<Coord>;

struct Node {
    unsigned size = 0, used = 0, avail = 0;
};
using Disk = std::unordered_map<Point, Node>;

using PointPair = std::pair<Point, Point>;

template<>
struct std::hash<PointPair>
{
    size_t operator() (PointPair const& p) const noexcept {
        size_t seed = 0;
        boost::hash_combine(seed, std::hash<Point> {}(p.first));
        boost::hash_combine(seed, std::hash<Point> {}(p.second));
        return seed;
    }
};

void part1(Disk const& disk)
{
    std::unordered_set<PointPair> pairs;

    for (auto const& [px1, n1] : disk) {
        for (auto const& [px2, n2] : disk) {
            if (px1 == px2 || n2.used == 0) continue;
            if (n1.avail >= n2.used) {
                if (px1 < px2) {
                    pairs.insert({px1, px2});
                } else {
                    pairs.insert({px2, px1});
                }
            }
        }
    }
    fmt::print("1: {}\n", pairs.size());
}

int main()
{
    static const boost::regex RX("^/dev/grid/node-x([0-9]+)-y([0-9]+) +([0-9]+)T +([0-9]+)T +([0-9]+)T +([0-9]+)%$");

    Disk disk;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) break;

        boost::smatch m;
        if (!boost::regex_match(line, m, RX)) {
            continue;
        }

        Point px;
        px.x = std::stoi(m.str(1));
        px.y = std::stoi(m.str(2));

        Node n;
        n.size = std::stoi(m.str(3));
        n.used = std::stoi(m.str(4));
        n.avail = std::stoi(m.str(5));

        disk.insert({std::move(px), std::move(n)});
    };

    part1(disk);

    return 0;
}
