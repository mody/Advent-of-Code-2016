#include <algorithm>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <string>

int main()
{
    std::string input = "abcdefgh";
    fmt::print("! {}\n", input);

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) break;

        if (line.starts_with("swap position ")) {
            const unsigned p1 = std::stoi(&line.at(14));
            const unsigned p2 = std::stoi(&line.at(30));
            std::swap(input[p1], input[p2]);
        } else if (line.starts_with("swap letter ")) {
            const unsigned char c1 = line.at(12);
            const unsigned char c2 = line.at(26);
            assert(std::count(input.cbegin(), input.cend(), c1) == 1);
            assert(std::count(input.cbegin(), input.cend(), c2) == 1);
            std::swap(input.at(input.find(c1)), input.at(input.find(c2)));
        } else if (line.starts_with("reverse positions ")) {
            const unsigned p1 = std::stoi(&line.at(18));
            const unsigned p2 = std::stoi(&line.at(28));
            std::reverse(std::next(input.begin(), p1), std::next(input.begin(), p2 + 1));
        } else if (line.starts_with("move position ")) {
            const unsigned p1 = std::stoi(&line.at(14));
            const unsigned p2 = std::stoi(&line.at(28));
            unsigned char c = input.at(p1);
            input.erase(p1, 1);
            input.insert(std::next(input.begin(), p2), c);
        } else if (line.starts_with("rotate left ")) {
            const unsigned n1 = std::stoi(&line.at(12));
            std::rotate(input.begin(), std::next(input.begin(), n1), input.end());
        } else if (line.starts_with("rotate right ")) {
            const unsigned n1 = std::stoi(&line.at(12));
            std::rotate(input.rbegin(), std::next(input.rbegin(), n1), input.rend());
        } else if (line.starts_with("rotate based on position of letter ")) {
            const unsigned p1 = input.find(line.at(35));
            std::rotate(input.rbegin(), std::next(input.rbegin(), 1), input.rend());
            std::rotate(input.rbegin(), std::next(input.rbegin(), p1), input.rend());
            if (p1 >= 4) {
                std::rotate(input.rbegin(), std::next(input.rbegin(), 1), input.rend());
            }
        } else {
            assert(false);
        }
    }
    fmt::print("1: {}\n", input);
    return 0;
}
