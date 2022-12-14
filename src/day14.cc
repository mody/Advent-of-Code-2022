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

    Coord min_x {}, min_y {}, max_x {}, max_y {};
    Mapa mapa;
public:

    auto insert(Mapa::value_type v) {
        mapa.insert(std::move(v));
    }

    void min_max()
    {
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
        fmt::print("\n");
    }


    bool drop(Point drop)
    {
        for(;;) {
            drop += Gfx_2d::Down;

            if (drop.x < min_x || drop.x > max_x || drop.y < min_y || drop.y > max_y) {
                // fall through
                return false;
            }

            auto it = mapa.find(drop);
            if (it == mapa.end()) {
                // no obstacle
                continue;
            }

            drop += Gfx_2d::Left;
            it = mapa.find(drop);
            if (it == mapa.end()) {
                continue;
            }

            drop += Gfx_2d::Right;
            drop += Gfx_2d::Right;

            it = mapa.find(drop);
            if (it == mapa.end()) {
                continue;
            }

            drop += Gfx_2d::Left;
            drop += Gfx_2d::Up;
            mapa.insert({drop, 'o'});
            break;
        }
        return true;
    }
};


void part1(World world)
{
    unsigned score = 0;
    while (world.drop({500, 0})) {
        ++score;
    }
    // world.dump();

    fmt::print("1: {}\n", score);
}


int main()
{
    World world;

    world.insert({{500, 0}, '+'});

    fmt::print("Start\n");
    std::cout.flush();

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
    world.min_max();

    part1(world);

    return 0;
}
