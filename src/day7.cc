#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <set>
#include <string_view>
#include <tuple>
#include <vector>

bool find_abba(const std::string_view input)
{
    if (input.size() < 4) {
        return false;
    }
    for (unsigned i = 0; i < input.size() - 3; ++i) {
        if (input.at(i) != input.at(i + 1) && input.at(i) == input.at(i + 3) && input.at(i + 1) == input.at(i + 2)) {
            return true;
        }
    }
    return false;
}

std::set<std::tuple<unsigned char, unsigned char>> find_aba(const std::string_view input)
{
    std::set<std::tuple<unsigned char, unsigned char>> result;

    if (input.size() > 2) {
        for (unsigned i = 0; i < input.size() - 2; ++i) {
            if (input.at(i) == input.at(i + 2) && input.at(i) != input.at(i + 1)) {
                result.insert({input.at(i), input.at(i + 1)});
            }
        }
    }

    return result;
}

bool find_bab(const std::string_view input, std::tuple<unsigned char, unsigned char> const& aba)
{
    if (input.size() > 2) {
        for (unsigned i = 0; i < input.size() - 2; ++i) {
            if (input.at(i) == std::get<1>(aba) && input.at(i + 1) == std::get<0>(aba)
                && input.at(i + 2) == std::get<1>(aba))
            {
                return true;
            }
        }
    }
    return false;
}


int main()
{
    unsigned count1 = 0, count2 = 0;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        std::vector<std::string> parts, base, hypernet;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of("[]"));
        for (unsigned i = 0; i < parts.size(); ++i) {
            if ((i % 2) == 0) {
                base.push_back(parts.at(i));
            } else {
                hypernet.push_back(parts.at(i));
            }
        }
        if (std::any_of(base.cbegin(), base.cend(), find_abba)
            && std::none_of(hypernet.cbegin(), hypernet.cend(), find_abba))
        {
            ++count1;
        }

        std::set<std::tuple<unsigned char, unsigned char>> abas;
        for (auto const& s : base) {
            auto candidates = find_aba(s);
            abas.insert(candidates.cbegin(), candidates.cend());
        }
        for (auto const& s : hypernet) {
            if (std::any_of(abas.cbegin(), abas.cend(), [&s](auto const& aba) { return find_bab(s, aba); })) {
                ++count2;
                break;
            }
        }
    }

    fmt::print("1: {}\n", count1);
    fmt::print("2: {}\n", count2);

    return 0;
}
