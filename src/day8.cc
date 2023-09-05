#include "point2d.h"

#include <boost/regex.hpp>
#include <boost/regex/v5/regex_fwd.hpp>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <unordered_set>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

struct Screen
{
    void fill(Coord dx, Coord dy)
    {
        for (Coord y = 0; y < dy; ++y) {
            for (Coord x = 0; x < dx; ++x) {
                data.insert({x, y});
            }
        }
    }

    void rotate_row(const Coord y, unsigned cnt)
    {
        for (unsigned i = 0; i < cnt; ++i) {
            for (Coord x = X_SIZE - 1; x >= 0; --x) {
                if (data.erase({x, y})) {
                    data.insert({x + 1, y});
                }
            }
            assert(!data.contains({0, y}));
            if (data.erase({X_SIZE, y})) {
                data.insert({0, y});
            }
        }
    }

    void rotate_col(const Coord x, unsigned cnt)
    {
        for (unsigned i = 0; i < cnt; ++i) {
            for (Coord y = Y_SIZE - 1; y >= 0; --y) {
                if (data.erase({x, y})) {
                    data.insert({x, y + 1});
                }
            }
            assert(!data.contains({x, 0}));
            if (data.erase({x, Y_SIZE})) {
                data.insert({x, 0});
            }
        }
    }

    size_t size() const { return data.size(); }

    void print() const
    {
        for (Coord y = 0; y < Y_SIZE; ++y) {
            for (Coord x = 0; x < X_SIZE; ++x) {
                if (data.contains({x, y})) {
                    fmt::print("#");
                } else {
                    fmt::print(".");
                }
            }
            fmt::print("\n");
        }
    }

protected:
    static constexpr Coord X_SIZE = 50;
    static constexpr Coord Y_SIZE = 6;

    std::unordered_set<Point> data;

};

int main()
{
    Screen s;

    static const boost::regex RX_RECT("rect (\\d+)x(\\d+)");
    static const boost::regex RX_ROT_ROW("rotate row y=(\\d+) by (\\d+)");
    static const boost::regex RX_ROT_COL("rotate column x=(\\d+) by (\\d+)");

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) break;

        boost::smatch res;
        if (boost::regex_search(line, res, RX_RECT)) {
            s.fill(std::stoi(res[1].str()), std::stoi(res[2].str()));
        } else if (boost::regex_search(line, res, RX_ROT_COL)) {
            s.rotate_col(std::stoi(res[1].str()), std::stoi(res[2].str()));
        } else if (boost::regex_search(line, res, RX_ROT_ROW)) {
            s.rotate_row(std::stoi(res[1].str()), std::stoi(res[2].str()));
        } else {
            assert(false);
        }
    }

    fmt::print("1: {}\n", s.size());

    return 0;
}
