#include <cassert>
#include <cmath>
#include <cstddef>
#include <deque>
#include <fmt/core.h>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <string_view>

std::size_t process(std::string_view data, std::size_t times, unsigned max_depht)
{
    std::size_t output = 0;

    if (!max_depht) {
        output = data.size();
    } else {
        std::size_t pos = 0;
        for (std::size_t next = data.find('('); next != std::string::npos; next = data.find('(', pos)) {
            output += next - pos;
            pos = next + 1;

            std::size_t skip = 0;
            std::size_t len = std::stoul(&data.at(pos), &skip);
            pos += skip;
            assert(data.at(pos) == 'x');
            ++pos;
            std::size_t repeat = std::stoul(&data.at(pos), &skip);
            pos += skip;
            assert(data.at(pos) == ')');
            ++pos;

            output += process(data.substr(pos, len), repeat, max_depht - 1);

            pos += len;
        }
        output += data.size() - pos;
    }
    return times * output;
}


int main()
{
    std::string line;
    std::getline(std::cin, line);

    fmt::print("1: {}\n", process(line, 1, 1));

    return 0;
}
