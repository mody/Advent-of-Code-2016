#include <algorithm>
#include <fmt/core.h>

void part1(unsigned const disk_size, std::string data)
{
    while(data.size() < disk_size) {
        std::string b = data;
        std::reverse(b.begin(), b.end());
        for (auto& c : b) {
            c = (c == '0') ? '1' : '0';
        }
        data += "0" + std::move(b);
    }
    data.resize(disk_size);

    while((data.size() & 0x1) == 0) {
        for (unsigned i = 0; i < data.size()/2; ++i) {
            data.at(i) = (data.at(i * 2) == data.at(i * 2 + 1)) ? '1' : '0';
        }
        data.resize(data.size() / 2);
    }

    fmt::print("data: {}\n", data);
}

int main()
{
    // part1(20, "10000");
    part1(272, "11110010111001001");

    return 0;
}
