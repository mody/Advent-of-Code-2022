#include "point2d.h"

#include <array>
#include <cassert>
#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <iterator>
#include <string>
#include <type_traits>
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

template<unsigned int KNOTS>
void part(Steps const& steps)
{
    std::array<Point, KNOTS> points;

    std::unordered_set<Point> positions{};
    positions.insert(points.at(0));

    for (auto const& step : steps) {
        for (unsigned i = 0; i < step.count; ++i) {
            points.at(0) += step.direction;

            for (unsigned j = 1; j < KNOTS; ++j) {
                Point const& head = points.at(j - 1);
                Point & tail = points.at(j);

                auto dist = head - tail;
                if (!(std::abs(dist.dx) > 1 || std::abs(dist.dy) > 1)) {
                    continue;
                }

                if (dist.dx == 0) {
                    tail.y += (dist.dy > 0) ? 1 : -1;
                } else if (dist.dy == 0) {
                    tail.x += (dist.dx > 0) ? 1 : -1;
                } else {
                    // diagonal
                    tail.x += (dist.dx > 0) ? 1 : -1;
                    tail.y += (dist.dy > 0) ? 1 : -1;
                }

                dist = head - tail;
                assert(std::abs(dist.dx) <= 1);
                assert(std::abs(dist.dy) <= 1);
            }

            positions.insert(points.back());
        }
    }

    fmt::print("{}: {}\n", KNOTS, positions.size());
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

    part<2>(steps);
    part<10>(steps);

    return 0;
}
