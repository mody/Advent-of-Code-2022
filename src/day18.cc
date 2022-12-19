#include "point3d.h"

#include <cassert>
#include <cstdlib>
#include <deque>
#include <fmt/core.h>
#include <iostream>
#include <sstream>
#include <string>
#include <unordered_set>

using Coord = std::int32_t;
using Point = Gfx_3d::Point<Coord>;
using World = std::unordered_set<Point>;


void dump(World const& world)
{
    fmt::print("World has {} points\n", world.size());
    for (auto const& px : world) {
        fmt::print("{},{},{}\n", px.x, px.y, px.z);
    }
}

void part1(World world)
{
    World covered;

    for (auto const& px : world)
    {
        Point oth = px + Gfx_3d::Up;
        if (world.contains(oth)) {
            oth = px + Gfx_3d::Down;
            if (world.contains(oth)) {
                oth = px + Gfx_3d::Left;
                if (world.contains(oth)) {
                    oth = px + Gfx_3d::Right;
                    if (world.contains(oth)) {
                        oth = px + Gfx_3d::Further;
                        if (world.contains(oth)) {
                            oth = px + Gfx_3d::Closer;
                            if (world.contains(oth)) {
                                // covered
                                covered.insert(px);
                            }
                        }
                    }
                }
            }
        }
    }

    unsigned int sides = 0;

    for (auto const& px : world)
    {
        if (covered.contains(px)) {
            continue;
        }

        Point oth = px + Gfx_3d::Up;
        if (!world.contains(oth)) {
            ++sides;
        }
        oth = px + Gfx_3d::Down;
        if (!world.contains(oth)) {
            ++sides;
        }
        oth = px + Gfx_3d::Left;
        if (!world.contains(oth)) {
            ++sides;
        }
        oth = px + Gfx_3d::Right;
        if (!world.contains(oth)) {
            ++sides;
        }
        oth = px + Gfx_3d::Further;
        if (!world.contains(oth)) {
            ++sides;
        }
        oth = px + Gfx_3d::Closer;
        if (!world.contains(oth)) {
            ++sides;
        }
    }

    fmt::print("1: {}\n", sides);
}

int main()
{
    World world;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        std::stringstream ss(line);
        unsigned char c;

        Point px{};
        ss >> px.x >> c >> px.y >> c >> px.z;
        world.insert(std::move(px));
    }
    // dump(world);

    part1(world);

    return 0;
}
