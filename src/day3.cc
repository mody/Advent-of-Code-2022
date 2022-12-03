#include <algorithm>
#include <boost/algorithm/string/join.hpp>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include <vector>

using StringVect = std::vector<std::string>;

void part1(StringVect const& data)
{
    auto score = [](unsigned char c) -> unsigned {
        if (c >= 'a' && c <= 'z') {
            return c - 'a' + 1;
        }
        if (c >= 'A' && c <= 'Z') {
            return c - 'A' + 27;
        }
        assert(false);
    };

    uint64_t total_score = 0;

    for (auto const& line : data) {
        std::string p1 = line.substr(0, line.size() / 2);
        std::string p2 = line.substr(line.size() / 2);
        assert(p1.size() == p2.size());

        std::sort(p1.begin(), p1.end());
        auto p1_end = std::unique(p1.begin(), p1.end());
        std::sort(p2.begin(), p2.end());
        auto p2_end = std::unique(p2.begin(), p2.end());

        std::string common;
        std::set_intersection(p1.begin(), p1_end, p2.begin(), p2_end, std::back_inserter(common));

        for (auto const& c : common) {
            total_score += score(c);
        }
    }

    fmt::print("1: {}\n", total_score);
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

    return 0;
}
