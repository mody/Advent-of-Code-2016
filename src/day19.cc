#include <cmath>
#include <fmt/core.h>

int main()
{
    const unsigned INPUT = 3004953;
    {
        const unsigned res = 1 + 2 * (INPUT - (unsigned)std::pow(2, (unsigned)std::log2(INPUT)));
        fmt::print("1: {}\n", res);
    }

    // see https://oeis.org/A334473
    auto highest_power_of_3 = [](unsigned n) -> unsigned {
        unsigned val = 0;
        for (; static_cast<unsigned>(std::pow(3, val)) <= n; ++val)
            ;
        return static_cast<unsigned>(std::pow(3, val - 1));
    };

    auto solve_shooters = [&highest_power_of_3](unsigned n) -> unsigned {
        const unsigned x = highest_power_of_3(n);
        if (x == n) {
            return x;
        } else {
            if (n < 2 * x) {
                return n % x;
            } else {
                return x + 2 * (n % x);
            }
        }
        return 0;
    };

    fmt::print("2: {}\n", solve_shooters(INPUT));

    return 0;
}

