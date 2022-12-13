#include <bit>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
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

void part1(Data const& data)
{
    unsigned score = 0;
    unsigned idx = 0;

    for (auto [lhs, rhs] : data) {
        ++idx;
        // fmt::print("{} - {} vs {}\n", idx, lhs, rhs);

        size_t pos1 = 0, pos2 = 0;

        for(;;) {
            // std::cout.flush();
            if (pos1 >= lhs.size()) {
                // fmt::print("lhs ends XXX\n");
                score += idx;
                break;
            }
            if (pos2 >= rhs.size()) {
                // fmt::print("rhs ends\n");
                break;
            }

            char c1 = lhs.at(pos1);
            char c2 = rhs.at(pos2);

            if (c1 == '[' && c2 == '[') {
                // fmt::print("same [\n");
                ++pos1;
                ++pos2;
            } else if (c1 == ']' && c2 == ']') {
                // fmt::print("same ]\n");
                ++pos1;
                ++pos2;
            } else if (c1 == ',' && c2 == ',') {
                // fmt::print("same ,\n");
                ++pos1;
                ++pos2;
            } else if (c1 >= '0' && c1 <= '9' && c2 >= '0' && c2 <= '9') {
                size_t next1 = 0, next2 = 0;
                auto n1 = std::stoi(&lhs[pos1], &next1);
                auto n2 = std::stoi(&rhs[pos2], &next2);
                if (n1 > n2) {
                    // fmt::print("{} > {}\n", n1, n2);
                    break;
                } else if (n1 < n2) {
                    // fmt::print("{} < {} XXX\n", n1, n2);
                    score += idx;
                    break;
                }
                // fmt::print("{} <= {}\n", n1, n2);
                // fmt::print("pos1: {}, next1: {}\n", pos1, next1);
                // fmt::print("pos2: {}, next2: {}\n", pos2, next2);
                pos1 += next1;
                pos2 += next2;
            } else if (c1 == '[' && c2 >= '0' && c2 <= '9') {
                make_list(rhs, pos2);
                // fmt::print("make_list rhs - {}\n", rhs);
            } else if (c1 >= '0' && c1 <= '9' && c2 == '[') {
                make_list(lhs, pos1);
                // fmt::print("make_list lhs - {}\n", lhs);
            } else if (c2 == ']') {
                // fmt::print("rhs out of items\n");
                break;
            } else if (c1 == ']') {
                // fmt::print("lhs out of items XXX\n");
                score += idx;
                break;
            } else {
                // fmt::print("lhs: {}, c1: {}, pos1: {}\n", lhs, c1, pos1);
                // fmt::print("rhs: {}, c2: {}, pos2: {}\n", rhs, c2, pos2);
                // std::cout.flush();
                assert(false);
            }
        }
    }

    // 5757 -- too low

    fmt::print("1: {}\n", score);
}

int main()
{
    // [1,1,3,1,1]
    // [1,1,5,1,1]

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

    return 0;
}
