#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <string>

void part1(std::string const& input)
{
    size_t from = 0;
    for (; (from + 4) < input.size(); ++from) {
        if (input.at(from) != input.at(from + 1)
            && input.at(from) != input.at(from + 2)
            && input.at(from) != input.at(from + 3)
            && input.at(from + 1) != input.at(from + 2)
            && input.at(from + 1) != input.at(from + 3)
            && input.at(from + 2) != input.at(from + 3))
        {
            fmt::print("1: {}\n", from + 4);
            break;
        }
    }
}

int main()
{
    std::string line;

    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        part1(line);
    }

    return 0;
}
