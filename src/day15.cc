#include <algorithm>
#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <fmt/core.h>
#include <iostream>
#include <numeric>
#include <string>
#include <vector>

struct Wheel
{
    unsigned positions = 0;
    unsigned offset = 0;
};

using Wheels = std::vector<Wheel>;


unsigned find_time(Wheels wheels)
{
    const unsigned max_time = std::accumulate(
        wheels.cbegin(), wheels.cend(), 1, [](unsigned const& a, Wheel const& w2) { return a * w2.positions; });

    for(unsigned t = 0; t < max_time; ++t) {
        std::vector<unsigned> positions(wheels.size(), 0u);
        for (unsigned i = 0; i < wheels.size(); ++i) {
            positions.at(i) = (wheels.at(i).offset + t + i + 1) % wheels.at(i).positions;
        }

        if (std::all_of(positions.cbegin(), positions.cend(), [](unsigned const& v) { return v == 0; })) {
            return t;
        }
    }
    return 0;
}

int main()
{
    Wheels wheels;

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

    fmt::print("1: {}\n", find_time(wheels));

    wheels.push_back(Wheel{.positions = 11});

    fmt::print("2: {}\n", find_time(wheels));

    return 0;
}
