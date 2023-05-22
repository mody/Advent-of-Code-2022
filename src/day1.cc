#include <cassert>
#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <set>
#include <string>

int main()
{
    uint64_t val = 0, sum = 0;

    std::set<uint64_t, std::greater<uint64_t>> top;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            top.insert(sum);
            sum = 0;
        }
        else
        {
            val = stoull(line);
            sum += val;
        }
    }
    top.insert(sum);

    auto it = top.begin();
    fmt::print("1: {}\n", *it);

    assert(top.size() > 2);
    sum = *it++;
    sum += *it++;
    sum += *it++;
    fmt::print("2: {}\n", sum);

    return 0;
}
