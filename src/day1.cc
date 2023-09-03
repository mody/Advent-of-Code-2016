#include "point2d.h"

#include <array>
#include <iostream>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

int main()
{
    static const std::array<Gfx_2d::Direction, 4> directions = { Gfx_2d::North, Gfx_2d::East, Gfx_2d::South, Gfx_2d::West };
    Point px = {0,0};
    int dir = 0;

    std::string line;
    std::getline(std::cin, line);
    std::stringstream ss(line);
    std::string in;
    while(ss >> in) {
        const char& turn = in.at(0);
        const int steps = std::stoi(&in.at(1));
        std::cout << turn << " ... " << steps << std::endl;

        if (turn == 'R') {
            ++dir;
        } else if (turn == 'L') {
            --dir;
            if (dir < 0) {
                dir += directions.size();
            }
        }
        dir = dir % directions.size();
        for (Coord i = 0; i < steps; ++i) {
            px += directions.at(dir);
        }
    }

    std::cout << "1: " << (std::abs(px.x) + std::abs(px.y)) << std::endl;

    return 0;
}
