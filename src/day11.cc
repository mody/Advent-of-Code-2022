#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/join.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/split.hpp>
#include <cassert>
#include <cstdint>
#include <deque>
#include <fmt/core.h>
#include <functional>
#include <iostream>
#include <optional>
#include <sstream>
#include <string>
#include <vector>

enum class Op
{
    NOOP = 0,
    PLUS,
    MUL,
    SQRT
};

struct Ape;
using Apes = std::vector<Ape>;
using StringVect = std::vector<std::string>;


struct Ape
{
    std::deque<std::uint64_t> items{};

    Op op {};
    int op_value = 0;
    size_t dst_true = 0, dst_false = 0;

    unsigned divisible = 0;

    std::uint64_t inspections = 0;

    bool process_one(Apes& apes, std::optional<std::uint64_t> global_div = std::nullopt)
    {
        if (items.empty()) {
            return false;
        }

        std::uint64_t v = items.front();
        items.pop_front();

        switch (op) {
        case Op::PLUS: v += op_value; break;
        case Op::MUL: v *= op_value; break;
        case Op::SQRT: v = v * v; break;
        default: assert(false);
        }

        if (!global_div) {
            v = v / 3u;
        } else {
            v = v % global_div.value();
        }

        auto new_val = v % divisible;
        if ((new_val) == 0) {
            apes.at(dst_true).items.push_back(v);
        } else {
            apes.at(dst_false).items.push_back(v);
        }

        ++inspections;

        return true;
    }
};

void part1(Apes apes)
{
    for (unsigned round = 0; round < 20; ++round) {
        for (auto& ape : apes) {
            while (ape.process_one(apes)) {
            };
        }
    }

    std::set<std::uint64_t, std::greater<std::uint64_t>> inspections;
    for (auto const& ape : apes) {
        inspections.insert(ape.inspections);
    }
    assert(inspections.size() > 2);

    fmt::print("1: {}\n", (*inspections.begin())*(*std::next(inspections.begin())));
}

void part2(Apes apes)
{
    uint64_t global_div = 1;
    for (auto& ape : apes) {
        global_div *= ape.divisible;
    }

    for (unsigned round = 0; round < 10000; ++round) {
        for (auto& ape : apes) {
            while (ape.process_one(apes, global_div)) {
            };
        }
    }

    std::set<std::uint64_t, std::greater<std::uint64_t>> inspections;
    for (auto const& ape : apes) {
        inspections.insert(ape.inspections);
    }
    assert(inspections.size() > 2);

    fmt::print("2: {}\n", (*inspections.begin())*(*std::next(inspections.begin())));
}

int main()
{
    Apes apes;

    unsigned ape_id = 0;
    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        assert(boost::starts_with(line, "Monkey"));
        assert(ape_id == std::stoi(&line[7]));

        Ape ape{};

        {
            std::getline(std::cin, line);
            assert(boost::starts_with(line, "  Starting items:"));
            StringVect values;
            boost::algorithm::split(
                values, line.substr(18), boost::algorithm::is_any_of(", "), boost::algorithm::token_compress_on);
            for (auto const& v : values) {
                ape.items.push_back(std::stoi(v));
            }
        }

        {
            std::getline(std::cin, line);
            assert(boost::starts_with(line, "  Operation: new = old "));
            if (boost::starts_with(&line[25], "old")) {
                ape.op = Op::SQRT;
            } else {
                ape.op_value = std::stoi(&line[25]);
                const char op = line.at(23);
                if (op == '+') {
                    ape.op = Op::PLUS;
                } else if (op == '*') {
                    ape.op = Op::MUL;
                } else {
                    assert(false);
                }
            }
        }

        {
            std::getline(std::cin, line);
            assert(boost::starts_with(line, "  Test: divisible by "));
            ape.divisible = std::stoi(&line[21]);
        }

        {
            std::getline(std::cin, line);
            assert(boost::starts_with(line, "    If true: throw to monkey "));
            ape.dst_true = std::stoi(&line[29]);
        }

        {
            std::getline(std::cin, line);
            assert(boost::starts_with(line, "    If false: throw to monkey "));
            ape.dst_false = std::stoi(&line[30]);
        }

        // empty line
        std::getline(std::cin, line);

        apes.push_back(std::move(ape));

        ++ape_id;
    }

    part1(apes);
    part2(apes);

    return 0;
}
