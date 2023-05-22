#include "point2d.h"

#include <fmt/core.h>

#include <algorithm>
#include <array>
#include <iostream>
#include <map>
#include <string>
#include <unordered_map>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;

struct Tile
{
    char c = {};
    Point next = {};
};

struct World {
    std::unordered_map<Point, Tile> mapa;
    mutable Coord min_x {}, min_y {};
    mutable Coord max_x {}, max_y {};

    std::array<char, 4> directions = {'N', 'S', 'W', 'E'};

    void rotate_directions() noexcept
    {
        std::rotate(directions.begin(), std::next(directions.begin()), directions.end());
    }

    void min_max() const noexcept {
        min_x = std::numeric_limits<Coord>::max();
        min_y = std::numeric_limits<Coord>::max();
        max_x = std::numeric_limits<Coord>::min();
        max_y = std::numeric_limits<Coord>::min();

        for (auto const& [px, _] : mapa) {
            min_x = std::min(min_x, px.x);
            min_y = std::min(min_y, px.y);
            max_x = std::max(max_x, px.x);
            max_y = std::max(max_y, px.y);
        }
    }

    void dump() const noexcept
    {
        min_max();
        for (Coord y = min_y; y <= max_y; ++y) {
            for (Coord x = min_x; x <= max_x; ++x) {
                const Point p {x, y};
                const auto it = mapa.find(p);
                if (it != mapa.end()) {
                    fmt::print("{:c}", it->second.c);
                } else {
                    fmt::print(".");
                }
            }
            fmt::print("\n");
        }
        fmt::print("\n");
    }
};

template <typename T> class TD;

void part1(World world)
{
    enum
    {
        North = 1 << 0,
        South = 1 << 1,
        West = 1 << 2,
        East = 1 << 3,
        NW = 1 << 4,
        NE = 1 << 5,
        SW = 1 << 6,
        SE = 1 << 7
    };
    constexpr int UP = North|NW|NE;
    constexpr int DOWN = South|SW|SE;
    constexpr int LEFT = West|NW|SW;
    constexpr int RIGHT = East|NE|SE;

    std::map<int, Gfx_2d::Direction> DIRECTIONS = {
        {North, Gfx_2d::North},
        {South, Gfx_2d::South},
        {West, Gfx_2d::West},
        {East, Gfx_2d::East},
        {NW, Gfx_2d::NW},
        {NE, Gfx_2d::NE},
        {SW, Gfx_2d::SW},
        {SE, Gfx_2d::SE}};

    for (unsigned round = 0; round < 10; ++round) {
        unsigned unchanged = 0;
        for (auto& [px, tile] : world.mapa) {
            unsigned char neighs = 0;
            for (auto const& [dir1, dir2] : DIRECTIONS) {
                auto it = world.mapa.find(px + dir2);
                if (it != world.mapa.end() && it->second.c == '#') {
                    neighs |= dir1;
                }
            }

            // fmt::print("{},{} neighs {:#0b} -> ", px.x, px.y, neighs);
            if (neighs == 0) {
                // not moving
                tile.next = px;
                ++unchanged;
            } else {
                for (auto const& d : world.directions) {
                    if (d == 'N') {
                        if ((neighs & UP) == 0) {
                            tile.next = px + Gfx_2d::North;
                            break;
                        }
                    } else if (d == 'S') {
                        if ((neighs & DOWN) == 0) {
                            tile.next = px + Gfx_2d::South;
                            break;
                        }
                    } else if (d == 'W') {
                        if ((neighs & LEFT) == 0) {
                            tile.next = px + Gfx_2d::West;
                            break;
                        }
                    } else if (d == 'E') {
                        if ((neighs & RIGHT) == 0) {
                            tile.next = px + Gfx_2d::East;
                            break;
                        }
                    } else {
                        assert(false);
                    }
                }
            }
            // fmt::print("{},{}\n", tile.next.x, tile.next.y);
        }

        if (unchanged == world.mapa.size()) {
            break;
        }

        std::unordered_map<Point, unsigned> counts;
        for (auto const& [_, tile] : world.mapa) {
            counts[tile.next]++;
        }

        World w2;
        for (auto const& [px, tile] : world.mapa) {
            // fmt::print("{},{} count {}\n", tile.next.x, tile.next.y, counts.at(tile.next));
            if (counts.at(tile.next) == 1) {
                w2.mapa[tile.next] = {.c = tile.c};
            } else {
                w2.mapa[px] = {.c = tile.c};
            }
        }

        w2.directions = world.directions;
        w2.rotate_directions();
        // w2.dump();
        std::swap(world, w2);
    }

    world.min_max();
    unsigned unused = 0;
    for (Coord y = world.min_y; y <= world.max_y; ++y) {
        for (Coord x = world.min_x; x <= world.max_x; ++x) {
            if (!world.mapa.contains({x, y})) {
                ++unused;
            }
        }
    }
    fmt::print("1: {}\n", unused);
}

int main()
{
    World world;

    {
        Point px {0, 0};
        std::string line;
        while (std::getline(std::cin, line)) {
            if (line.empty()) {
                break;
            }
            for (auto c : line) {
                if (c != '.') {
                    world.mapa.insert({px, {c, px}});
                }
                ++px.x;
            }
            ++px.y;
            px.x = 0;
        }
    }
    // world.dump();

    part1(world);
}
