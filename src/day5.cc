#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cassert>
#include <deque>
#include <fmt/core.h>
#include <iostream>
#include <iterator>
#include <map>
#include <sstream>
#include <string>
#include <vector>

using StringVect = std::vector<std::string>;
using Stack = std::deque<unsigned char>;
using Stacks = std::map<unsigned int, Stack>;

struct Op {
    unsigned int from {};
    unsigned int to {};
    unsigned int amount {};
};

using Ops = std::vector<Op>;

void dump(Stacks const& stacks)
{
    fmt::print("DUMP\n");
    for (auto const& [i, stack] : stacks) {
        fmt::print("{} = ", i);
        for (auto const& c : stack) {
            fmt::print("[{:c}]", c);
        }
        fmt::print("\n");
    }

}


void part1(Stacks stacks, Ops const& ops)
{
    std::string result;

    for (auto const& op : ops) {
        Stack& from = stacks.at(op.from);
        Stack& to = stacks.at(op.to);

        for (auto i = 0; i < op.amount; ++i) {
            to.push_back(from.back());
            from.pop_back();
        }
    }

    for (auto const& [i, stack] : stacks) {
        result += stack.back();
    }

    fmt::print("1: {}\n", result);
}

int main()
{
    Stacks stacks;
    Ops ops;

    std::string line;
    std::getline(std::cin, line);
    assert(!line.empty());
    {
        std::stringstream ss(line);
        unsigned int i = 0;
        while (ss >> i) {
            stacks.insert({i, {}});
            // fmt::print("Added stack {}\n", i);
        }
    }

    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        for (auto& [i, stack] : stacks) {
            const unsigned int pos = (i - 1) * 4 + 1;
            if (pos >= line.size()) {
                break;
            }
            unsigned char c = line.at(pos);
            if (c >= 'A') {
                stack.push_front(c); // Reverse order!!
                // fmt::print("Added '{:c}' into stack {}\n", c, i);
            }
        }
    }

    while (std::getline(std::cin, line)) {
        if (line.empty()) break;
        StringVect parts;
        boost::algorithm::split(parts, line, boost::algorithm::is_any_of(" "));
        assert(parts.size() == 6);
        ops.push_back(
            {static_cast<unsigned int>(std::stoi(parts.at(3))),
             static_cast<unsigned int>(std::stoi(parts.at(5))),
             static_cast<unsigned int>(std::stoi(parts.at(1)))});
        // fmt::print("move {} from {} to {}\n", ops.back().amount, ops.back().from, ops.back().to);
    }

    part1(stacks, ops);

    return 0;
}
