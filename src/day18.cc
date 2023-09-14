#include <algorithm>
#include <fmt/core.h>
#include <iostream>
#include <string_view>
#include <unordered_set>

static const std::unordered_set<std::string_view> TRAP = {
    {"^^."},
    {"^.."},
    {".^^"},
    {"..^"},
};

int main()
{
    std::string line;
    std::getline(std::cin, line);

    line = "." + line + ".";

    auto count_traps = [](const std::string_view input) {
     return std::count_if(input.cbegin(), input.cend(), [](unsigned char c) { return c == '.'; }) - 2;
    };

    auto next_line = [](const std::string_view input) -> std::string {
        std::string result(input.size(), '.');
        for (unsigned i = 1; i < input.size() - 1; ++i) {
            if (TRAP.contains(input.substr(i - 1, 3))) {
                result[i] = '^';
            }
        }
        return result;
    };

    // fmt::print("{}\n", std::string_view(&line[1], line.size() - 2));
    unsigned count = count_traps(line);

    for (unsigned i = 0; i < 39; ++i) {
        line = next_line(line);
        count += count_traps(line);
        // fmt::print("{}\n", std::string_view(&line[1], line.size() - 2));
    }

    fmt::print("1: {}\n", count);

    return 0;
}
