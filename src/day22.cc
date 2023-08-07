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

void part2_4(World const& world, const Coord tile_size, std::string instructions, Point& position, Gfx_2d::Direction& dir)
{
#if 0
        1111
        1111
        1111
        1111
222233334444
222233334444
222233334444
222233334444
        55556666
        55556666
        55556666
        55556666
#endif

    auto point_to_tile = [&tile_size](Point const& p) -> int {
        static const std::array<std::array<int, 4>, 4> mapping = {{
            { 5, 3, 1, 6},
            { 2, 3, 4, 6},
            { 1, 3, 5, 6},
        }};
        const int _x = p.x / tile_size;
        const int _y = p.y / tile_size;
        return mapping.at(_y).at(_x);
    };


    // find start
    while(true) {
        auto it = world.find(position);
        if (it != world.end()) {
            if (it->second == '.') {
                break;
            }
        }
        position.x += 1;
    }


    auto next_point = [&world, &point_to_tile, &tile_size](
                          Point const& p0,
                          Gfx_2d::Direction const& d0) -> std::optional<std::pair<Point, Gfx_2d::Direction>> {
        const int tile0 = point_to_tile(p0);
        Point p1 = p0 + d0;
        Gfx_2d::Direction d1 = d0;
        const int inv_x_offset = (tile_size - (p0.x % tile_size) - 1);
        const int inv_y_offset = (tile_size - (p0.y % tile_size) - 1);
        // fmt::print("1 candidate point: {},{} dir: {}\n", p1.x, p1.y, direction_to_value(d0));
        auto it = world.find(p1);
        if (it == world.end()) {
            if (tile0 == 4) { // going to tile 6
                assert(d0 == Gfx_2d::Right);
                p1.x = 3 * tile_size + inv_y_offset;
                p1.y = 2 * tile_size;
                d1 = Gfx_2d::Down;
            } else if (tile0 == 5) {
                if (d0 == Gfx_2d::Down) { // goint to tile 2
                    p1.x = 0 * tile_size + inv_x_offset;
                    p1.y = 2 * tile_size - 1;
                } else if (d0 == Gfx_2d::Left) { // goint to tile 3
                    p1.x = 2 * tile_size - 1 - inv_y_offset;
                    p1.y = 2 * tile_size - 1;
                } else {
                    assert(false);
                }
                d1 = Gfx_2d::Up;
            } else if (tile0 == 3) {
                if (d0 == Gfx_2d::Up) { // goint to tile 1
                    p1.x = 2 * tile_size;
                    p1.y = 1 * tile_size - 1 - inv_x_offset;
                } else if (d0 == Gfx_2d::Down) { // goint to tile 5
                    p1.x = 2 * tile_size;
                    p1.y = 2 * tile_size + inv_x_offset;
                } else {
                    assert(false);
                }
                d1 = Gfx_2d::Right;
            } else if (tile0 == 1) {
                if (d0 == Gfx_2d::Left) { // goint to tile 3
                    p1.x = 1 * tile_size + inv_y_offset;
                    p1.y = 1 * tile_size;
                    d1 = Gfx_2d::Down;
                } else if (d0 == Gfx_2d::Up) { // goint to tile 2
                    p1.x = 0 * tile_size + inv_x_offset;
                    p1.y = 1 * tile_size;
                    d1 = Gfx_2d::Down;
                } else if (d0 == Gfx_2d::Right) { // goint to tile 6
                    p1.x = 4 * tile_size - 1;
                    p1.y = 2 * tile_size + inv_y_offset;
                    d1 = Gfx_2d::Left;
                } else {
                    assert(false);
                }
            } else if (tile0 == 2) {
                if (d0 == Gfx_2d::Left) { // goint to tile 6
                    p1.x = 3 * tile_size + inv_y_offset;
                    p1.y = 3 * tile_size - 1;
                    d1 = Gfx_2d::Up;
                } else if (d0 == Gfx_2d::Up) { // goint to tile 1
                    p1.x = 2 * tile_size + inv_x_offset;
                    p1.y = 0 * tile_size;
                    d1 = Gfx_2d::Down;
                } else if (d0 == Gfx_2d::Down) { // goint to tile 5
                    p1.x = 2 * tile_size + inv_x_offset;
                    p1.y = 3 * tile_size - 1;
                    d1 = Gfx_2d::Up;
                } else {
                    assert(false);
                }
            } else if (tile0 == 6) {
                if (d0 == Gfx_2d::Up) { // goint to tile 4
                    p1.x = 3 * tile_size - 1;
                    p1.y = 2 * tile_size - 1 - inv_x_offset;
                    d1 = Gfx_2d::Left;
                } else if (d0 == Gfx_2d::Right) { // goint to tile 1
                    p1.x = 3 * tile_size - 1;
                    p1.y = 0 * tile_size + inv_y_offset;
                    d1 = Gfx_2d::Left;
                } else if (d0 == Gfx_2d::Down) { // goint to tile 2
                    p1.x = 0 * tile_size;
                    p1.y = 1 * tile_size + inv_x_offset;
                    d1 = Gfx_2d::Right;
                } else {
                    assert(false);
                }
            }
            // fmt::print("2 candidate point: {},{}\n", p1.x, p1.y);
            it = world.find(p1);
            assert(it != world.end());

        }
        if (it->second == '#') {
            return std::nullopt;
        }
        // fmt::print("next point: {},{}\n", p1.x, p1.y);
        return {{p1, d1}};
    };

    // fmt::print(
    //     "position: {},{} dir: {} tile: {}\n",
    //     position.x,
    //     position.y,
    //     direction_to_value(dir),
    //     point_to_tile(position));

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
                auto pn = next_point(position, dir);
                if (!pn) {
                    break;
                }
                position = pn.value().first;
                dir = pn.value().second;
                // fmt::print(
                //     "position: {},{} dir: {} tile: {}\n",
                //     position.x,
                //     position.y,
                //     direction_to_value(dir),
                //     point_to_tile(position));
            }
        }
        instructions = instructions.substr(skip);
    }
}

void part2_50(World const& world, const Coord tile_size, std::string instructions, Point& position, Gfx_2d::Direction& dir)
{
#if 0
    11116666
    11116666
    11116666
    11116666
    2222
    2222
    2222
    2222
44443333
44443333
44443333
44443333
5555
5555
5555
5555
#endif

    auto point_to_tile = [&tile_size](Point const& p) -> int {
        static const std::array<std::array<int, 4>, 4> mapping = {{
            { 0, 1, 6, 0},
            { 0, 2, 0, 0},
            { 4, 3, 0, 0},
            { 5, 0, 0, 0},
        }};
        const int _x = p.x / tile_size;
        const int _y = p.y / tile_size;
        return mapping.at(_y).at(_x);
    };


    // find start
    while(true) {
        auto it = world.find(position);
        if (it != world.end()) {
            if (it->second == '.') {
                break;
            }
        }
        position.x += 1;
    }


    auto next_point = [&world, &point_to_tile, &tile_size](
                          Point const& p0,
                          Gfx_2d::Direction const& d0) -> std::optional<std::pair<Point, Gfx_2d::Direction>> {
        const int tile0 = point_to_tile(p0);
        const int x_offset = p0.x % tile_size;
        const int y_offset = p0.y % tile_size;
        // const int inv_x_offset = (tile_size - x_offset - 1);
        const int inv_y_offset = (tile_size - y_offset - 1);
        // fmt::print("1 p0: {},{} dir: {}, tile0: {}\n", p0.x, p0.y, direction_to_value(d0), tile0);
        // fmt::print("1 p0: x_offset={}, y_offset={}\n", x_offset, y_offset);
        // fmt::print("1 p0: inv_x_offset={}, inv_y_offset={}\n", inv_x_offset, inv_y_offset);

        Point p1 = p0 + d0;
        Gfx_2d::Direction d1 = d0;
        // fmt::print("1 candidate point: {},{} dir: {}\n", p1.x, p1.y, direction_to_value(d0));

        auto it = world.find(p1);
        if (it == world.end()) {
            if (tile0 == 1) {
                if (d0 == Gfx_2d::Up) { // goint to tile 5
                    p1.x = 0 * tile_size;
                    p1.y = 3 * tile_size + x_offset;
                    d1 = Gfx_2d::Right;
                } else if (d0 == Gfx_2d::Left) { // goint to tile 4
                    p1.x = 0 * tile_size;
                    p1.y = 2 * tile_size + inv_y_offset;
                    d1 = Gfx_2d::Right;
                } else {
                    assert(false);
                }
            } else if (tile0 == 4) {
                if (d0 == Gfx_2d::Left) { // goint to tile 1
                    p1.x = 1 * tile_size;
                    p1.y = 0 * tile_size + inv_y_offset;
                    d1 = Gfx_2d::Right;
                } else if (d0 == Gfx_2d::Up) { // goint to tile 2
                    p1.x = 1 * tile_size;
                    p1.y = 1 * tile_size + x_offset;
                    d1 = Gfx_2d::Right;
                } else {
                    assert(false);
                }
            } else if (tile0 == 3) {
                if (d0 == Gfx_2d::Down) { // goint to tile 5
                    p1.x = 1 * tile_size - 1;
                    p1.y = 3 * tile_size + x_offset;
                    d1 = Gfx_2d::Left;
                } else if (d0 == Gfx_2d::Right) { // goint to tile 6
                    p1.x = 3 * tile_size - 1;
                    p1.y = 0 * tile_size + inv_y_offset;
                    d1 = Gfx_2d::Left;
                } else {
                    assert(false);
                }
            } else if (tile0 == 5) {
                if (d0 == Gfx_2d::Right ) { // goint to tile 3
                    p1.x = 1 * tile_size + y_offset;
                    p1.y = 3 * tile_size - 1;
                    d1 = Gfx_2d::Up;
                } else if (d0 == Gfx_2d::Left ) { // goint to tile 1
                    p1.x = 1 * tile_size + y_offset;
                    p1.y = 0 * tile_size;
                    d1 = Gfx_2d::Down;
                } else if (d0 == Gfx_2d::Down ) { // goint to tile 6
                    p1.x = 2 * tile_size + x_offset;
                    p1.y = 0 * tile_size;
                    d1 = Gfx_2d::Down;
                } else {
                    assert(false);
                }
            } else if (tile0 == 6) {
                if (d0 == Gfx_2d::Right ) { // goint to tile 3
                    p1.x = 2 * tile_size - 1;
                    p1.y = 2 * tile_size + inv_y_offset;
                    d1 = Gfx_2d::Left;
                } else if (d0 == Gfx_2d::Down ) { // goint to tile 2
                    p1.x = 2 * tile_size - 1;
                    p1.y = 1 * tile_size + x_offset;
                    d1 = Gfx_2d::Left;
                } else if (d0 == Gfx_2d::Up ) { // goint to tile 5
                    p1.x = 0 * tile_size + x_offset;
                    p1.y = 4 * tile_size - 1;
                    d1 = Gfx_2d::Up;
                } else {
                    assert(false);
                }
            } else if (tile0 == 2) {
                if (d0 == Gfx_2d::Left) { // goint to tile 4
                    p1.x = 0 * tile_size + y_offset;
                    p1.y = 2 * tile_size;
                    d1 = Gfx_2d::Down;
                } else if (d0 == Gfx_2d::Right) { // goint to tile 6
                    p1.x = 2 * tile_size + y_offset;
                    p1.y = 1 * tile_size - 1;
                    d1 = Gfx_2d::Up;
                } else {
                    assert(false);
                }
            }
            // fmt::print("2 candidate point: {},{}\n", p1.x, p1.y);
            it = world.find(p1);
            assert(it != world.end());

        }

        // is candidate a wall?
        if (it->second == '#') {
            // fmt::print("candidate is a wall\n");
            return std::nullopt;
        }
        // fmt::print("next point: {},{}\n", p1.x, p1.y);
        return {{p1, d1}};
    };

    // fmt::print(
    //     "position: {},{} dir: {} tile: {}\n",
    //     position.x,
    //     position.y,
    //     direction_to_value(dir),
    //     point_to_tile(position));

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
                auto pn = next_point(position, dir);
                if (!pn) {
                    break;
                }
                position = pn.value().first;
                dir = pn.value().second;
                // fmt::print(
                //     "position: {},{} dir: {} tile: {}\n",
                //     position.x,
                //     position.y,
                //     direction_to_value(dir),
                //     point_to_tile(position));
            }
        }
        instructions = instructions.substr(skip);
    }
}

void part2(StringVec input, std::string instructions)
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


    const auto [max_x, max_y, tile_size] = [&input]() {
        const Coord my = input.size();
        const Coord mx =
            std::max_element(input.begin(), input.end(), [](std::string const& lhs, std::string const& rhs) {
                return lhs.size() < rhs.size();
            })->size();
        // fmt::print("max_x={}, max_y={}", mx, my);
        if (mx > my) {
            assert(mx / 4 == my / 3);
            return std::make_tuple(mx, my, mx / 4);
        } else if (mx < my) {
            assert(mx / 3 == my / 4);
            return std::make_tuple(mx, my, mx / 3);
        }
        assert(false);
    }();
    // fmt::print(", tile_size={}\n", tile_size);

    Point position = {0, 0};
    Gfx_2d::Direction dir = Gfx_2d::Right;

    if (tile_size == 4) {
        // test data
        part2_4(world, tile_size, std::move(instructions), position, dir);
    } else if (tile_size == 50) {
        part2_50(world, tile_size, std::move(instructions), position, dir);
        // 148391 That's not the right answer; your answer is too high
    }


    position.x++;
    position.y++;
    const int dir_value = direction_to_value(dir);

    fmt::print("final position: {},{}\nfinal direction: {}\n", position.x, position.y, dir_value);

    fmt::print("2: {}\n", 1000*position.y + 4*position.x + dir_value);
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
    part2(input, line);

    return 0;
}
