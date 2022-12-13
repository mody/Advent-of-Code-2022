#include <algorithm>
#include <bit>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>

using Pair = std::pair<std::string, std::string>;
using Data = std::vector<Pair>;

void make_list(std::string& s, size_t pos)
{
    assert(pos < s.size());
    assert(s.at(pos) >= '0');
    assert(s.at(pos) <= '9');

    s.insert(pos, 1, '[');
    for (++pos; pos < s.size() && s.at(pos) >= '0' && s.at(pos) <= '9'; ++pos) {
    }
    s.insert(pos, 1, ']');
}

bool compare_less(std::string lhs, std::string rhs)
{
    size_t pos1 = 0, pos2 = 0;

    for (;;) {
        if (pos1 >= lhs.size()) {
            return true;
        }
        if (pos2 >= rhs.size()) {
            return false;
        }

        char c1 = lhs.at(pos1);
        char c2 = rhs.at(pos2);

        if (c1 == '[' && c2 == '[') {
            ++pos1;
            ++pos2;
        } else if (c1 == ']' && c2 == ']') {
            ++pos1;
            ++pos2;
        } else if (c1 == ',' && c2 == ',') {
            ++pos1;
            ++pos2;
        } else if (c1 >= '0' && c1 <= '9' && c2 >= '0' && c2 <= '9') {
            size_t next1 = 0, next2 = 0;
            auto n1 = std::stoi(&lhs[pos1], &next1);
            auto n2 = std::stoi(&rhs[pos2], &next2);
            if (n1 > n2) {
                return false;
            } else if (n1 < n2) {
                return true;
            }
            pos1 += next1;
            pos2 += next2;
        } else if (c1 == '[' && c2 >= '0' && c2 <= '9') {
            make_list(rhs, pos2);
        } else if (c1 >= '0' && c1 <= '9' && c2 == '[') {
            make_list(lhs, pos1);
        } else if (c2 == ']') {
            return false;
        } else if (c1 == ']') {
            return true;
        } else {
            assert(false);
        }
    }
}

void part1(Data const& data)
{
    unsigned score = 0;
    unsigned idx = 0;

    for (auto [lhs, rhs] : data) {
        ++idx;
        if (compare_less(std::move(lhs), std::move(rhs))) {
            score += idx;
        }
    }

    fmt::print("1: {}\n", score);
}

void part2(Data const& data)
{
    std::vector<std::string> v;
    v.push_back("[[2]]");
    v.push_back("[[6]]");

    for (auto [lhs, rhs] : data) {
        v.push_back(std::move(lhs));
        v.push_back(std::move(rhs));
    }

    std::sort(v.begin(), v.end(), [](const std::string& lhs, const std::string& rhs) {
        return compare_less(lhs, rhs);
    });

    auto i1 = std::find(v.begin(), v.end(), "[[2]]");
    auto i2 = std::find(v.begin(), v.end(), "[[6]]");
    fmt::print("2: {}\n", ((1 + std::distance(v.begin(), i1)) * (1 + std::distance(v.begin(), i2))));
}

int main()
{
    Data data;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        Pair p{};
        p.first = std::move(line);

        std::getline(std::cin, line);
        if (line.empty()) {
            break;
        }

        p.second = std::move(line);
        data.push_back(std::move(p));

        std::getline(std::cin, line);
    }

    part1(data);
    part2(data);

    return 0;
}
