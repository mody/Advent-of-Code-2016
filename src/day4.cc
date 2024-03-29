#include <fmt/core.h>

#include <algorithm>
#include <iostream>
#include <map>
#include <vector>


std::string get_checksum(std::string const& name)
{
    std::map<unsigned char, unsigned> freq;
    for (auto c : name) {
        freq[c] += 1;
    }
    std::vector<std::pair<unsigned char, unsigned>> freq2;
    for (auto i : freq) {
        if (i.first != '-') {
            freq2.push_back(std::move(i));
        }
    }
    std::sort(freq2.begin(), freq2.end(), [](auto const& lhs, auto const& rhs) {
        return (lhs.second == rhs.second) ? (lhs.first < rhs.first) : (lhs.second > rhs.second);
    });
    if (freq2.size() > 5) {
        freq2.resize(5);
    }
    std::string my_check;
    for (auto const& it : freq2) {
        my_check += it.first;
    }
    return my_check;
}


std::string decode_name(std::string name, unsigned count)
{
    for (auto& c : name) {
        if (c == '-') {
            c = ' ';
            continue;
        }
        unsigned x = c - 'a';
        x = (x + count) % 26;
        c = 'a' + x;
    }

    return name;
}

int main()
{
    unsigned sum = 0;
    unsigned target = 0;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

        const auto pos = line.find_last_of('-');
        const std::string name = line.substr(0, pos);
        const unsigned int sector_id = std::stoul(&line.at(pos + 1));
        const std::string checksum = line.substr(line.find_first_of('[') + 1, 5);

        if (checksum == get_checksum(name)) {
            sum += sector_id;
        }

        if ("northpole object storage" == decode_name(name, sector_id)) {
            target = sector_id;
        }
    }

    fmt::print("1: {}\n", sum);
    fmt::print("2: {}\n", target);
    return 0;
}
