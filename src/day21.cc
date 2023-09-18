#include <algorithm>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include <vector>

using Commands = std::vector<std::string>;

void part1(Commands const& commands, std::string input)
{
    for (std::string const& line : commands) {
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
}

void part2(Commands const& commands, std::string input)
{
    for (auto it = commands.crbegin(); it != commands.crend(); ++it) {
        std::string const& line = *it;

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
            const unsigned p1 = std::stoi(&line.at(28));
            const unsigned p2 = std::stoi(&line.at(14));
            unsigned char c = input.at(p1);
            input.erase(p1, 1);
            input.insert(std::next(input.begin(), p2), c);
        } else if (line.starts_with("rotate left ")) {
            const unsigned n1 = std::stoi(&line.at(12));
            std::rotate(input.rbegin(), std::next(input.rbegin(), n1), input.rend());
        } else if (line.starts_with("rotate right ")) {
            const unsigned n1 = std::stoi(&line.at(13));
            std::rotate(input.begin(), std::next(input.begin(), n1), input.end());
        } else if (line.starts_with("rotate based on position of letter ")) {
            unsigned p1 = input.find(line.at(35));
            bool left = true;
            if (p1 == 0) {
                // left once
                p1 = 1;
            } else if (p1 == 1) {
                // left once
            } else if (p1 == 2) {
                // right twice
                left = false;
            } else if (p1 == 3) {
                // left twice
                p1 = 2;
            } else if (p1 == 4) {
                // right once
                p1 = 1;
                left = false;
            } else if (p1 == 5) {
                // left three times
                p1 = 3;
            } else if (p1 == 6) {
                // no move
                p1 = 0;
            } else if (p1 == 7) {
                // left four times
                p1 = 4;
            }
            if (left) {
                std::rotate(input.begin(), std::next(input.begin(), p1), input.end());
            } else {
                std::rotate(input.rbegin(), std::next(input.rbegin(), p1), input.rend());
            }
        } else {
            assert(false);
        }
    }
    fmt::print("2: {}\n", input);
}

int main()
{
    Commands commands;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) break;
        commands.push_back(std::move(line));
    }

    part1(commands, "abcdefgh"); // -> gfdhebac
    part2(commands, "fbgdceah");

    return 0;
}
