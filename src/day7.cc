#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>

#include <algorithm>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <string_view>
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

int main()
{
    unsigned count1 = 0;

    std::string line;
    while(std::getline(std::cin, line))
    {
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
    }

    fmt::print("1: {}\n", count1);

    return 0;
}
