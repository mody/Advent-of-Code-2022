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

void part2(World world)
{
    Coord min_x = std::numeric_limits<Coord>::max(),
          min_y = std::numeric_limits<Coord>::max(),
          min_z = std::numeric_limits<Coord>::max(),
          max_x = std::numeric_limits<Coord>::min(),
          max_y = std::numeric_limits<Coord>::min(),
          max_z = std::numeric_limits<Coord>::min();
    for (auto const& px : world) {
        min_x = std::min(min_x, px.x);
        min_y = std::min(min_y, px.y);
        min_z = std::min(min_z, px.z);
        max_x = std::max(max_x, px.x);
        max_y = std::max(max_y, px.y);
        max_z = std::max(max_z, px.z);
    }

    --min_x;
    --min_y;
    --min_z;
    ++max_x;
    ++max_y;
    ++max_z;

    World air;

    // expand the air
    std::deque<Point> q;
    q.push_front({min_x, min_y, min_z});

    while(!q.empty()) {
        Point px = q.front();
        q.pop_front();
        if(air.contains(px)) {
            continue;
        }

        Point oth = px;
        oth += Gfx_3d::Left;
        if (oth.x >= min_x) {
            if (!air.contains(oth) && !world.contains(oth)) {
                q.push_back(std::move(oth));
            }
        }

        oth = px;
        oth += Gfx_3d::Right;
        if (oth.x <= max_x) {
            if (!air.contains(oth) && !world.contains(oth)) {
                q.push_back(std::move(oth));
            }
        }

        oth = px;
        oth += Gfx_3d::Up;
        if (oth.y >= min_y) {
            if (!air.contains(oth) && !world.contains(oth)) {
                q.push_back(std::move(oth));
            }
        }

        oth = px;
        oth += Gfx_3d::Down;
        if (oth.y <= max_y) {
            if (!air.contains(oth) && !world.contains(oth)) {
                q.push_back(std::move(oth));
            }
        }

        oth = px;
        oth += Gfx_3d::Closer;
        if (oth.z >= min_z) {
            if (!air.contains(oth) && !world.contains(oth)) {
                q.push_back(std::move(oth));
            }
        }

        oth = px;
        oth += Gfx_3d::Further;
        if (oth.z <= max_z) {
            if (!air.contains(oth) && !world.contains(oth)) {
                q.push_back(std::move(oth));
            }
        }

        air.insert(std::move(px));
    }

    unsigned int sides = 0;

    for (Point const& px : air) {
        Point oth = px;
        oth += Gfx_3d::Left;
        if (world.contains(oth)) {
            ++sides;
        }

        oth = px;
        oth += Gfx_3d::Right;
        if (world.contains(oth)) {
            ++sides;
        }

        oth = px;
        oth += Gfx_3d::Up;
        if (world.contains(oth)) {
            ++sides;
        }

        oth = px;
        oth += Gfx_3d::Down;
        if (world.contains(oth)) {
            ++sides;
        }

        oth = px;
        oth += Gfx_3d::Closer;
        if (world.contains(oth)) {
            ++sides;
        }

        oth = px;
        oth += Gfx_3d::Further;
        if (world.contains(oth)) {
            ++sides;
        }
    }

    fmt::print("2: {}\n", sides);
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
    part2(world);

    return 0;
}
