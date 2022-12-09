#include "point2d.h"

#include <cassert>
#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <string>
#include <unordered_map>

using Coord = std::int32_t;
using Point = Gfx_2d::Point<Coord>;

struct World {
    std::unordered_map<Point, unsigned char> mapa;
    Coord min_x {}, min_y {};
    Coord max_x {}, max_y {};

    void min_max() {
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
};

bool is_visible(World const& world, const Point center)
{
    bool visible = false;

    auto check_direction = [&world, &center](const Coord dx, const Coord dy) {
        const unsigned char my_level = world.mapa.at(center);
        for (Point px = center;;) {
            px.x += dx;
            px.y += dy;
            auto it = world.mapa.find(px);
            if (it == world.mapa.end()) {
                return true;
            }
            if (my_level <= it->second) {
                return false;
            }
        }
        assert(false);
        return false;
    };

    visible |= check_direction(1, 0);
    visible |= check_direction(0, 1);
    visible |= check_direction(-1, 0);
    visible |= check_direction(0, -1);
    return visible;
}

void part1(World const& world)
{
    unsigned int visible = 0;
    for (auto const& [px, _] : world.mapa) {
        if (is_visible(world, px)) {
            ++visible;
        }
    }
    fmt::print("1: {}\n", visible);
}

std::uint64_t get_score(World const& world, const Point center)
{
    auto visible_distance = [&world, &center](const Coord dx, const Coord dy) -> unsigned int {
        const unsigned char my_level = world.mapa.at(center);
        unsigned int score = 0;
        for (Point px = center;;) {
            px.x += dx;
            px.y += dy;
            auto it = world.mapa.find(px);
            if (it == world.mapa.end()) {
                break;
            }
            ++score;
            if (my_level <= it->second) {
                break;
            }
        }
        return score;
    };

    std::uint64_t score = 1;
    score *= visible_distance(1, 0);
    score *= visible_distance(0, 1);
    score *= visible_distance(-1, 0);
    score *= visible_distance(0, -1);
    return score;
}

void part2(World const& world)
{
    std::uint64_t score = 0;
    for (auto const& [px, _] : world.mapa) {
        score = std::max(score, get_score(world, px));
    }
    fmt::print("2: {}\n", score);
}


int main()
{
    World world;

    std::string line;
    Coord y = 0;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        Coord x = 0;
        for (const char& c : line) {
            world.mapa.insert({{x, y}, c});
            ++x;
        }
        ++y;
    }
    world.min_max();

    part1(world);
    part2(world);

    return 0;
}
