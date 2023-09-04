#include "point2d.h"

#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

int main()
{
    const std::unordered_map<Point, int> dial = {
        {{0, 0}, 1},
        {{1, 0}, 2},
        {{2, 0}, 3},
        {{0, 1}, 4},
        {{1, 1}, 5},
        {{2, 1}, 6},
        {{0, 2}, 7},
        {{1, 2}, 8},
        {{2, 2}, 9},
    };

    const std::map<unsigned char, Gfx_2d::Direction> move_map = {
        {'U', Gfx_2d::Up},
        {'R', Gfx_2d::Right},
        {'D', Gfx_2d::Down},
        {'L', Gfx_2d::Left},
    };

    Point px = {1, 1}; // start at '5'
    unsigned code = 0;

    std::string line;
    while(std::getline(std::cin, line))
    {
        if (line.empty()) {
            break;
        }
        for(auto const& c : line) {
            Point p2 = px + move_map.at(c);
            if (dial.contains(p2)) {
                px = p2;
            }
        }
        code = code * 10 + dial.at(px);
    }

    std::cout << "1: " << code << std::endl;

    return 0;
}
