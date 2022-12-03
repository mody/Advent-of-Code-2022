#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <string>
#include <vector>

using StringVect = std::vector<std::string>;

unsigned score(unsigned char c)
{
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 1;
    }
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 27;
    }
    assert(false);
}


void part1(StringVect const& data)
{
    uint64_t total_score = 0;

    for (auto const& line : data) {
        std::string p1 = line.substr(0, line.size() / 2);
        std::string p2 = line.substr(line.size() / 2);
        assert(p1.size() == p2.size());

        std::sort(p1.begin(), p1.end());
        auto p1_end = std::unique(p1.begin(), p1.end());
        std::sort(p2.begin(), p2.end());
        const auto p2_end = std::unique(p2.begin(), p2.end());

        std::string common;
        std::set_intersection(p1.begin(), p1_end, p2.begin(), p2_end, std::back_inserter(common));

        for (auto const& c : common) {
            total_score += score(c);
        }
    }

    fmt::print("1: {}\n", total_score);
}


void part2(StringVect const& data)
{
    uint64_t total_score = 0;

    std::map<unsigned char, unsigned int> freq;

    for (auto line : data) {
        std::sort(line.begin(), line.end());
        const auto line_end = std::unique(line.begin(), line.end());
        for (auto it = line.begin(); it != line_end; ++it) {
            freq.insert({*it, 0}).first->second += 1;
        }
        for (auto const& [c, cnt] : freq) {
            if (cnt == 3) {
                total_score += score(c);
                freq.clear();
                break;
            }
        }
    }

    fmt::print("2: {}\n", total_score);
}

int main()
{
    StringVect data;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        data.push_back(std::move(line));
    }

    part1(data);
    part2(data);

    return 0;
}
