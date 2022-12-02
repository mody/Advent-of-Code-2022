#include <algorithm>
#include <array>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

#if 0
A X ... rock
B Y ... paper
C Z ... scissors
#endif

using Steps = std::vector<std::pair<unsigned char, unsigned char>>;

using Points1 = std::map<unsigned char, int>;
using Points2 = std::map<std::string, int>;
using Mapping = std::array<unsigned char, 3>;

const Points1 POINTS1 = {{'A', 1}, {'B', 2}, {'C', 3}};
const Points2 POINTS2 = {
    {"AA", 3},
    {"BB", 3},
    {"CC", 3},
    {"AB", 6},
    {"CA", 6},
    {"BC", 6},
    {"BA", 0},
    {"AC", 0},
    {"CB", 0},
};

uint64_t play1(const Steps& steps)
{
    static const Mapping mapping = {'A', 'B', 'C'};

    uint64_t score = 0;
    for (auto const& s : steps) {
        const auto p1 = s.first;
        auto p2 = s.second;
        p2 = mapping.at(p2 - 'X');
        score += POINTS1.at(p2);
        score += POINTS2.at(std::string(1, p1) + std::string(1, p2));
    }
    return score;
}

void day1(Steps const& steps)
{
    fmt::print("1: {}\n", play1(steps));
}

int main()
{
    Steps steps;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        assert(line.size() == 3);
        steps.push_back({line[0], line[2]});
    }

    day1(steps);

    return 0;
}
