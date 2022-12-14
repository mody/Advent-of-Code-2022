#include "point2d.h"

#include <bit>
#include <boost/regex.hpp>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

using Coord = int32_t;
using Point = Gfx_2d::Point<Coord>;

static const boost::regex rx{"((\\d+),(\\d+)(?: -> )?)+?", boost::regex::perl};

class World
{
    using Mapa = std::map<Point, unsigned char>;

    Mapa mapa;
    Coord min_x = std::numeric_limits<Coord>::max(), min_y = std::numeric_limits<Coord>::max();

public:
    Coord max_x = std::numeric_limits<Coord>::min(), max_y = std::numeric_limits<Coord>::min();

    auto insert(Mapa::value_type v) {
        min_x = std::min(min_x, v.first.x);
        min_y = std::min(min_y, v.first.y);
        max_x = std::max(max_x, v.first.x);
        max_y = std::max(max_y, v.first.y);
        return mapa.insert_or_assign(v.first, v.second);
    }

    void dump() const noexcept
    {
        for (Coord y = min_y; y <= max_y; ++y) {
            for (Coord x = min_x; x <= max_x; ++x) {
                const Point p {x, y};
                const auto it = mapa.find(p);
                if (it != mapa.end()) {
                    fmt::print("{:c}", it->second);
                } else {
                    fmt::print(".");
                }
            }
            fmt::print("\n");
        }
        fmt::print("dims: {}-{} x {}-{}\n", min_x, max_x, min_y, max_y);
        std::cout.flush();
    }


    bool drop1(Point drop)
    {
        for(;;) {
            drop += Gfx_2d::Down;

            if (drop.x > max_x || drop.y > max_y) {
                // fall through
                return false;
            }

            if (!mapa.contains(drop)) {
                // no obstacle
                continue;
            }

            drop += Gfx_2d::Left;
            if (!mapa.contains(drop)) {
                continue;
            }

            drop += Gfx_2d::Right;
            drop += Gfx_2d::Right;

            if (!mapa.contains(drop)) {
                continue;
            }

            drop += Gfx_2d::Left;
            drop += Gfx_2d::Up;

            return insert({drop, 'o'}).second;
        }
    }

    bool drop2(Point drop, Coord limit_y)
    {
        for(;;) {
            drop += Gfx_2d::Down;

            if (drop.y < limit_y) {
                if (!mapa.contains(drop)) {
                    // no obstacle
                    continue;
                }

                drop += Gfx_2d::Left;
                if (!mapa.contains(drop)) {
                    continue;
                }

                drop += Gfx_2d::Right;
                drop += Gfx_2d::Right;

                if (!mapa.contains(drop)) {
                    continue;
                }

                drop += Gfx_2d::Left;
                drop += Gfx_2d::Up;
            } else {
                drop += Gfx_2d::Up;
            }

            return insert({drop, 'o'}).second;
        }
    }
};


void part1(World world)
{
    unsigned score = 0;
    while (world.drop1({500, 0})) {
        ++score;
    }

    fmt::print("1: {}\n", score);
    std::cout.flush();
}


void part2(World world)
{
    unsigned score = 0;
    Coord limit_y = world.max_y + 2;

    while (world.drop2({500, 0}, limit_y)) {
        ++score;
    }

    fmt::print("2: {}\n", score);
    std::cout.flush();
}


int main()
{
    World world;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

        std::vector<Point> points;
        for(;;) {
            boost::smatch res;
            if (!boost::regex_search(line, res, rx)) {
                break;
            }
            if (res.empty()) {
                fmt::print("No more match\n");
                break;
            }

            points.push_back({std::stoi(res[2].str()), std::stoi(res[3].str())});
            line = res.suffix();
        }

        auto it = points.begin();
        for (Point p1 = *it++; it != points.end();) {
            Point p2 = *it++;
            auto dist = p1 - p2;
            if (dist.dx == 0) {
                // vertical
                if (dist.dy > 0) {
                    dist = Gfx_2d::Up;
                } else {
                    dist = Gfx_2d::Down;
                }
            } else if (dist.dy == 0) {
                // horizontal
                if (dist.dx > 0) {
                    dist = Gfx_2d::Left;
                } else {
                    dist = Gfx_2d::Right;
                }
            } else {
                assert(false);
            }
            for(;;) {
                world.insert({p1, '#'});
                if (p1 == p2) {
                    break;
                }
                p1 += dist;
            }
            p1 = p2;
        }
    }

    part1(world);
    part2(world);

    return 0;
}
