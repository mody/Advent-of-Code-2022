#include "point2d.h"

#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

using Coord = std::int32_t;
using Point = Gfx_2d::Point<Coord>;
using Direction = Gfx_2d::Direction;

struct Step
{
    Direction direction {};
    unsigned count = 0;
};

using Steps = std::vector<Step>;

void dump(Point const& head, Point const& tail)
{
    Coord min_x = std::numeric_limits<Coord>::max();
    Coord min_y = std::numeric_limits<Coord>::max();
    Coord max_x = std::numeric_limits<Coord>::min();
    Coord max_y = std::numeric_limits<Coord>::min();

    for (auto const& px : {head, tail}) {
        min_x = std::min(min_x, px.x);
        min_y = std::min(min_y, px.y);
        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
    }

    for (Coord y = min_y; y <= max_y; ++y) {
        for (Coord x = min_x; x <= max_x; ++x) {
            const Point p {x, y};
            if (p == tail) {
                fmt::print("T");
            } else if (p == head) {
                fmt::print("H");
            } else {
                fmt::print(".");
            }
        }
        fmt::print("\n");
    }
    fmt::print("\n");
}

void part1(Steps const& steps)
{
    Point head{}, tail{}, last_head{};

    std::unordered_set<Point> positions{};
    positions.insert(tail);

    for (auto const& step : steps) {
        for (unsigned i = 0; i < step.count; ++i) {
            last_head = head;
            head += step.direction;

            auto dist = head - tail;
            if (std::abs(dist.dx) > 1 || std::abs(dist.dy) > 1) {
                tail = last_head;
            }
            positions.insert(tail);

            dist = head - tail;

            assert(std::abs(dist.dx) <= 1);
            assert(std::abs(dist.dy) <= 1);
        }
    }

    fmt::print("1: {}\n", positions.size());
}


int main()
{
    Steps steps{};

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        assert(line.size() >= 3);
        Step s {};
        switch (line.at(0)) {
        case 'U': s.direction = Gfx_2d::North; break;
        case 'D': s.direction = Gfx_2d::South; break;
        case 'L': s.direction = Gfx_2d::West; break;
        case 'R': s.direction = Gfx_2d::East; break;
        }
        s.count = std::stoul(&line[2]);
        steps.emplace_back(std::move(s));
    }

    part1(steps);

    return 0;
}
