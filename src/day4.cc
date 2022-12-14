#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using StringVect = std::vector<std::string>;

struct Group
{
    unsigned int start1 {};
    unsigned int end1 {};
    unsigned int start2 {};
    unsigned int end2 {};

    unsigned int size1() const noexcept { return end1 - start1 + 1; }
    unsigned int size2() const noexcept { return end2 - start2 + 1; }
};

using Groups = std::vector<Group>;

void part1(Groups const& groups)
{
    unsigned int cnt = 0;

    for (auto const& g : groups) {
        if (g.size1() >= g.size2()) {
            if (g.start1 <= g.start2 && g.end1 >= g.end2) {
                ++cnt;
            }
        } else if (g.size1() < g.size2()) {
            if (g.start2 <= g.start1 && g.end2 >= g.end1) {
                ++cnt;
            }
        }
    }

    fmt::print("1: {}\n", cnt);
}

void part2(Groups const& groups)
{
    unsigned int cnt = 0;

    for (auto const& g : groups) {
        if (g.start1 <= g.start2) {
            if (g.start2 <= g.end1) {
                ++cnt;
            }
        } else {
            if (g.start1 <= g.end2) {
                ++cnt;
            }
        }

#ifdef SLOW_AND_NAIVE
        std::map<unsigned int, char> freq;
        for (unsigned int i = g.start1; i <= g.end1; ++i) {
            freq.insert({i, 0}).first->second += 1;
        }
        for (unsigned int i = g.start2; i <= g.end2; ++i) {
            freq.insert({i, 0}).first->second += 1;
        }
        for (auto const& [_, count] : freq) {
            if (count > 1) {
                ++cnt;
                break;
            }
        }
#endif
    }

    fmt::print("2: {}\n", cnt);
}


int main()
{
    Groups groups;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        StringVect parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(",-"));
        assert(parts.size() == 4);
        groups.push_back(
            {static_cast<unsigned int>(std::stoi(parts.at(0))),
             static_cast<unsigned int>(std::stoi(parts.at(1))),
             static_cast<unsigned int>(std::stoi(parts.at(2))),
             static_cast<unsigned int>(std::stoi(parts.at(3)))});
    }

    part1(groups);
    part2(groups);

    return 0;
}
