#include "point2d.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <deque>
#include <iostream>
#include <map>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <fmt/core.h>

using StringVec = std::vector<std::string>;
using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using World = std::map<Point, char>;

constexpr unsigned direction_to_value(Gfx_2d::Direction const& dir) noexcept
{
    unsigned dir_value = 0;
    if (dir == Gfx_2d::Right) {
        dir_value = 0;
    } else if (dir == Gfx_2d::Down) {
        dir_value = 1;
    } else if (dir == Gfx_2d::Left) {
        dir_value = 2;
    } else if (dir == Gfx_2d::Up) {
        dir_value = 3;
    } else {
        assert(false);
    }
    return dir_value;
}


void part1(StringVec const& input, std::string instructions)
{
    World world;

    {
        Coord y = 0;
        for (auto const& line : input) {
            Coord x = 0;
            for (auto const& c : line) {
                if (c == '.' || c == '#') {
                    world[{x, y}] = c;
                }
                ++x;
            }
            ++y;
        }
    }

    Point position = {0, 0};
    while(true) {
        auto it = world.find(position);
        if (it != world.end()) {
            if (it->second == '.') {
                break;
            }
        }
        position.x += 1;
    }

    // fmt::print("position: {},{}\n", position.x, position.y);

    Gfx_2d::Direction dir = Gfx_2d::Right;

    auto next_point = [&world, &dir](Point p) -> std::optional<Point> {
        p += dir;
        auto it = world.find(p);
        if (it == world.end()) {
            // wrap around
            Gfx_2d::Direction opposite;
            if (dir == Gfx_2d::Up) {
                opposite = Gfx_2d::Down;
            } else if (dir == Gfx_2d::Right) {
                opposite = Gfx_2d::Left;
            } else if (dir == Gfx_2d::Down) {
                opposite = Gfx_2d::Up;
            } else if (dir == Gfx_2d::Left) {
                opposite = Gfx_2d::Right;
            }
            for(;;) {
                p += opposite;
                it = world.find(p);
                if (it == world.end()) {
                    break;
                }
            }
            p += dir;
            it = world.find(p);
        }

        if (it->second == '#') {
            return std::nullopt;
        }
        // fmt::print("next point: {},{}\n", p.x, p.y);
        return {p};
    };

    for(;!instructions.empty();) {
        std::size_t skip = 0;
        if (std::isalpha(instructions.front())) {
            char c = instructions.front();
            skip = 1;
            // fmt::print("{}\n", c);
            if (c == 'R') {
                if (dir == Gfx_2d::Up) {
                    dir = Gfx_2d::Right;
                } else if (dir == Gfx_2d::Right) {
                    dir = Gfx_2d::Down;
                } else if (dir == Gfx_2d::Down) {
                    dir = Gfx_2d::Left;
                } else if (dir == Gfx_2d::Left) {
                    dir = Gfx_2d::Up;
                }
            } else if (c == 'L') {
                if (dir == Gfx_2d::Up) {
                    dir = Gfx_2d::Left;
                } else if (dir == Gfx_2d::Left) {
                    dir = Gfx_2d::Down;
                } else if (dir == Gfx_2d::Down) {
                    dir = Gfx_2d::Right;
                } else if (dir == Gfx_2d::Right) {
                    dir = Gfx_2d::Up;
                }
            } else {
                assert(false);
            }
        } else {
            int steps = std::stoi(instructions, &skip);
            // fmt::print("{}\n", steps);
            for(; steps > 0; --steps) {
                auto pn = next_point(position);
                if (!pn) {
                    break;
                }
                position = pn.value();
            }
        }
        instructions = instructions.substr(skip);
    }

    const int dir_value = direction_to_value(dir);
    position.x += 1;
    position.y += 1;

    fmt::print("final position: {},{}\nfinal direction: {}\n", position.x, position.y, dir_value);
    fmt::print("1: {}\n", 1000*position.y + 4*position.x + dir_value);
    // 70118 too low
}

int main()
{
    StringVec input;
    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        input.push_back(std::move(line));
    }

    if(!std::getline(std::cin, line)) {
        fmt::print("Missing instructions!\n");
        exit(1);
    }
    assert(!line.empty());

    part1(input, line);

    return 0;
}
