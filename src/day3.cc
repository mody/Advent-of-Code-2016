#include <fmt/core.h>

#include <iostream>
#include <set>
#include <cassert>

int main()
{
    std::multiset<int> values;

    unsigned valid = 0;

    int a = 0, b = 0, c = 0;
    while(std::cin >> a >> b >> c)
    {
        values.clear();
        values.insert(a);
        values.insert(b);
        values.insert(c);
        assert(values.size() == 3);
        auto it = values.cbegin();
        a = *it++;
        b = *it++;
        c = *it++;
        if ((a+b) > c) {
            ++valid;
        }
    }
    fmt::print("1: {}\n", valid);

    return 0;
}
