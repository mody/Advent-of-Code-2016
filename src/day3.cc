#include <fmt/core.h>

#include <array>
#include <cassert>
#include <iostream>
#include <set>
#include <vector>

using Triangle = std::array<int, 3>;
using Numbers = std::vector<Triangle>;

int main()
{
    Numbers by_rows, col1, col2, col3;

    int count = 0;
    int a = 0, b = 0, c = 0;
    Triangle t1, t2, t3;
    while(std::cin >> a >> b >> c)
    {
        if ((count % 3) == 0) {
            t1[0] = a;
            t2[0] = b;
            t3[0] = c;
        } else if ((count % 3) == 1) {
            t1[1] = a;
            t2[1] = b;
            t3[1] = c;
        } else if ((count % 3) == 2) {
            t1[2] = a;
            t2[2] = b;
            t3[2] = c;

            col1.push_back(t1);
            col2.push_back(t2);
            col3.push_back(t3);
        }
        by_rows.push_back({a, b, c});
        ++count;
    }

    auto count_valid = [](Numbers const& numbers) {
        unsigned cnt = 0;
        std::multiset<int> values;
        for (auto t : numbers) {
            values.clear();
            values.insert(t[0]);
            values.insert(t[1]);
            values.insert(t[2]);
            auto it = values.cbegin();
            t[0] = *it++;
            t[1] = *it++;
            t[2] = *it++;
            if ((t[0] + t[1]) > t[2]) {
                ++cnt;
            }
        }
        return cnt;
    };

    col1.insert(col1.end(), col2.cbegin(), col2.cend());
    col1.insert(col1.end(), col3.cbegin(), col3.cend());

    fmt::print("1: {}\n", count_valid(by_rows));
    fmt::print("2: {}\n", count_valid(col1));
    return 0;
}
