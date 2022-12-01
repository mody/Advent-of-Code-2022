#include <fmt/core.h>
#include <iostream>
#include <string>

int main()
{
    uint64_t top = 0, val = 0, sum = 0;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            top = std::max(top, sum);
            sum = 0;
        }
        else
        {
            val = stoull(line);
            sum += val;
        }
    }
    top = std::max(top, sum);

    fmt::print("1: {}\n", top);

    return 0;
}
