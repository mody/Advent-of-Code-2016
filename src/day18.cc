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

    unsigned result1 = 0;
    unsigned count = count_traps(line);

    for (unsigned i = 1; i < 400000; ++i) {
        line = next_line(line);
        count += count_traps(line);

        if (i == 39) {
            result1 = count;
        }
    }

    fmt::print("1: {}\n", result1);
    fmt::print("2: {}\n", count);

    return 0;
}
