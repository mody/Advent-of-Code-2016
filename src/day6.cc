#include <fmt/core.h>

#include <algorithm>
#include <cassert>
#include <iostream>
#include <map>
#include <vector>

using Freq = std::map<unsigned char, unsigned>;

int main()
{
    std::vector<Freq> data;

    std::string line;
    while (std::getline(std::cin, line)) {
        if(line.empty()) {
            break;
        }
        if(data.size() < line.size()) {
            data.resize(line.size());
        }

        for(unsigned i = 0; i < line.size(); ++i) {
            Freq& f = data.at(i);
            f.insert({line.at(i), 0}).first->second++;
        }
    }

    std::string result1, result2;

    for (auto const& f : data) {
        auto it = std::max_element(
            f.cbegin(), f.cend(), [](auto const& lhs, auto const& rhs) { return lhs.second < rhs.second; });
        assert(it != f.cend());
        result1 += it->first;
        it = std::max_element(
            f.cbegin(), f.cend(), [](auto const& lhs, auto const& rhs) { return lhs.second > rhs.second; });
        assert(it != f.cend());
        result2 += it->first;
    }

    fmt::print("1: {}\n", result1);
    fmt::print("2: {}\n", result2);

    return 0;
}
