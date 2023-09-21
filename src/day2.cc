#include "point2d.h"

#include <range/v3/algorithm/for_each.hpp>
#include <range/v3/view/getlines.hpp>
#include <range/v3/view/take_while.hpp>

#include <cmath>
#include <iostream>
#include <string>
#include <unordered_map>
#include <map>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using Dial = std::unordered_map<Point, unsigned char>;

static const std::map<unsigned char, Gfx_2d::Direction> MOVE_MAP = {
    {'U', Gfx_2d::Up},
    {'R', Gfx_2d::Right},
    {'D', Gfx_2d::Down},
    {'L', Gfx_2d::Left},
};

int main()
{

    std::string code1;
    std::string code2;

    auto in =
        ranges::getlines_view(std::cin) | ranges::views::take_while([](std::string const& s) { return !s.empty(); });
    ranges::for_each(in, [&code1, &code2](std::string const& line) {

            // clang-format off
            const Dial dial1 = {
            {{0, 0}, '1'},
            {{1, 0}, '2'},
            {{2, 0}, '3'},
            {{0, 1}, '4'},
            {{1, 1}, '5'},
            {{2, 1}, '6'},
            {{0, 2}, '7'},
            {{1, 2}, '8'},
            {{2, 2}, '9'},
            };
            Point px1 = {1, 1}; // start at '5'

            const Dial dial2 = {
            {{2, 0}, '1'},
            {{1, 1}, '2'},
            {{2, 1}, '3'},
            {{3, 1}, '4'},
            {{0, 2}, '5'},
            {{1, 2}, '6'},
            {{2, 2}, '7'},
            {{3, 2}, '8'},
            {{4, 2}, '9'},
            {{1, 3}, 'A'},
            {{2, 3}, 'B'},
            {{3, 3}, 'C'},
            {{2, 4}, 'D'},
            };
            Point px2 = {0, 2}; // start at '5'
            // clang-format on

            auto processor = [](unsigned char c, Point& px, const Dial& dial) {
                Point p2 = px + MOVE_MAP.at(c);
                if (dial.contains(p2)) {
                    px = p2;
                }
            };

            for (auto const& c : line) {
                processor(c, px1, dial1);
                processor(c, px2, dial2);
            }
            code1 += dial1.at(px1);
            code2 += dial2.at(px2);
        });

    std::cout << "1: " << code1 << std::endl;
    std::cout << "2: " << code2 << std::endl;

    return 0;
}
