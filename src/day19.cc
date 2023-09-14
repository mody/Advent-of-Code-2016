#include <cmath>
#include <fmt/core.h>

int main()
{
    // for (unsigned i = 2; i < 17; ++i) {
    //     unsigned res = 1 + 2 * (i - (unsigned)std::exp2((unsigned)std::log2(i)));
    //     fmt::print("{} -> {}\n", i, res);
    // }

    const unsigned i = 3004953;
    const unsigned res = 1 + 2 * (i - (unsigned)std::exp2((unsigned)std::log2(i)));
    fmt::print("1: {}\n", res);

    return 0;
}

#if 0

1 x (2)

x x 3 (3)

1 x x x (4)

x x 3 x x (5)

x x x x 5 x (6)

x x x x x x 7 (7)

1 x x x x x x x (8)

x x 3 x x x x x x (9)

x x x x 5 x x x x x (10)

x x x x x x 7 x x x x (11)

x x x x x x x x 9 x x x (12)

x x x x x x x x x x b x x (13)

x x x x x x x x x x x x d x (14)

x x x x x x x x x x x x x x f (15)

1 x x x x x x x x x x x x x x x (16)

#endif
