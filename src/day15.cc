#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include <vector>

struct Wheel
{
    unsigned positions = 0;
    unsigned offset = 0;
};



int main()
{
    std::vector<Wheel> wheels;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) break;

        std::vector<std::string> parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "));
        Wheel w;
        w.positions = std::stoul(parts.at(3));
        w.offset = std::stoul(parts.at(11));
        wheels.emplace_back(std::move(w));
    }

    for(unsigned t = 0; t < 500000; ++t) {
        std::vector<unsigned> positions(wheels.size(), 0u);
        for (unsigned i = 0; i < wheels.size(); ++i) {
            positions.at(i) = (wheels.at(i).offset + t + i + 1) % wheels.at(i).positions;
        }

        if (std::all_of(positions.cbegin(), positions.cend(), [](unsigned const& v) { return v == 0; })) {
            fmt::print("1: {}\n", t);
            break;
        }
    }

    return 0;
}

#if 0

w1_max = 5
w1_start = 4

w1_pos = (w1_start + (t % w1_max)) % w1_max

Time    w1      w2
 0	4	1
 1	0	0
 2	1	1
 3	2	0
 4	3	1
 5	4	0
 6	0	1
 7	1	0
 8	2	1
 9	3	0
10	4	1
11	0	0
12	1	1
13	2	0
14	3	1
15	4	0
16	0	1
17	1	0
18	2	1
19	3	0
20	4	1


w1_pos = (w1_start + t) % w1_max

#endif
