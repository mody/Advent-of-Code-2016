#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cstdio>
#include <fmt/core.h>
#include <iostream>
#include <iterator>
#include <list>
#include <tuple>
#include <utility>
#include <vector>

struct Item 
{
    uint64_t from = 0, to = 0;

    bool operator< (Item const& oth) const noexcept {
        return std::tie(from, to) < std::tie(oth.from, oth.to);
    }
    bool operator== (Item const& oth) const noexcept {
        return std::tie(from, to) == std::tie(oth.from, oth.to);
    }

    std::string to_string() const noexcept { return fmt::format("{}-{}", from, to); }
};

using Data = std::list<Item>;

void part1(Data const& input, Data data)
{
    for (auto const& cur : input) {
        auto first = std::find_if(data.begin(), data.end(), [&](auto const& d) { return d.to >= cur.from; });
        if (first == data.end()) continue;
        if (cur.from > first->from) {
            const auto old = *first;
            *first = {old.from, cur.from - 1};
            ++first;
            if (first == data.end() || old.to > cur.to) {
                first = data.insert(first, {cur.to + 1, old.to});
                continue;
            }
        }
        while (first != data.end() && cur.to >= first->to) {
            first = data.erase(first);
        }
        if (first != data.end() && first->from <= cur.to) {
            first->from = cur.to + 1;
        }
    }

    // fmt::print("Data: ");
    // for (auto const& d : data) {
    //     fmt::print("[{}],", d.to_string());
    // }
    // fmt::print("\n");

    fmt::print("1: {}\n", data.front().from);
}

int main()
{
    Data data;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) break;

        size_t pos = 0;
        Item i;
        i.from = std::stoull(&line[0], &pos);
        ++pos;
        i.to = std::stoull(&line[pos], &pos);
        // fmt::print("{}-{}\n", i.from, i.to);
        data.push_back(std::move(i));
    }

    // part1(data, {{0, 9}});
    part1(data, {{0, 4294967295}});

    return 0;
}
