#include <algorithm>
#include <array>
#include <iostream>
#include <vector>
#include <cstdint>

using Num = std::int64_t;

std::string toSnafu(Num in)
{
    static constexpr std::array<std::pair<char, Num>, 5> MAP = {{
        {'0', 0},
        {'1', 0},
        {'2', 0},
        {'=', 1},
        {'-', 1}
    }};

    std::vector<char> out;
    out.reserve(10);

    for (;;) {
        const Num r = in % 5;
        const auto& [c, over] = MAP.at(r);
        out.push_back(c);
        if (in < 5 && over == 0)  {
            break;
        }
        in = in / 5 + over;
    }

    return std::string(out.rbegin(), out.rend());
}

Num toNum(std::string const& in)
{
    Num out = {};

    Num base = 1;
    for (auto it = in.rbegin(); it != in.rend(); ++it) {
        switch(*it) {
        case '0': break;
        case '1': out += 1 * base; break;
        case '2': out += 2 * base; break;
        case '=': out += -2 * base; break;
        case '-': out += -1 * base; break;
        }
        base = base * 5;
    }

    return out;
}

int main()
{
    // for (unsigned i = 0; i <= 20; ++i) {
    //     const auto s = toSnafu(i);
    //     std::cout << i << " ... " << s << " ... " << toNum(s) << "\n";
    // }
    // for (unsigned i : {2022u, 12345u, 314159265u}) {
    //     const auto s = toSnafu(i);
    //     std::cout << i << " ... " << s << " ... " << toNum(s) << "\n";
    // }

    Num sum = {};

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            break;
        }
        sum += toNum(line);
    }

    std::cout << "1: " << sum << " ... " << toSnafu(sum) << "\n";

    return 0;
}
