#include "point2d.h"

#include <boost/regex.hpp>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <limits>
#include <string>

using Coord = std::int64_t;
using Point = Gfx_2d::Point<Coord>;

static const boost::regex rx{"^Sensor at x=([0-9-]+), y=([0-9-]+): closest beacon is at x=([0-9-]+), y=([0-9-]+)$"};

struct Sensor {
    Point beacon{};
    Coord radius{};
};

using Mapa = std::map<Point, Sensor>;

// static constexpr Coord LEVEL_Y = 10;
static constexpr Coord LEVEL_Y = 2000000;
// static constexpr Coord LIMIT = 20;
static constexpr Coord LIMIT = 4000000;

void part1(Mapa mapa)
{
    Coord min_x = std::numeric_limits<Coord>::max(), max_x = std::numeric_limits<Coord>::min();

    for (auto const& [px, sensor] : mapa) {
        min_x = std::min(min_x, px.x - sensor.radius);
        max_x = std::max(max_x, px.x + sensor.radius);
    }
    max_x++; // one after to make it end iterator

    Coord score = 0;

    for (Point px {min_x, LEVEL_Y}; px != Point {max_x, LEVEL_Y}; px += Gfx_2d::Right) {
        for (auto const& [sensor_px, sensor] : mapa) {
            if (px != sensor_px && px != sensor.beacon && px.manhattan_dist(sensor_px) <= sensor.radius) {
                ++score;
                break;
            }
        }
    }

    fmt::print("1: {}\n", score);
}

// void part2_1(Mapa mapa)
// {
//     for (Coord y = 0; y <= LIMIT; ++y) {
//         for (Coord x = 0; x <= LIMIT; ++x) {
//             const Point px {x, y};
//             bool covered = false;
//             for (auto const& [sensor_px, sensor] : mapa) {
//                 if (px == sensor_px || px == sensor.beacon || px.manhattan_dist(sensor_px) <= sensor.dist) {
//                     covered = true;
//                     break;
//                 }
//             }
//             if (!covered) {
//                 fmt::print("2: {}\n", 4000000 * px.x + px.y);
//                 return;
//             }
//         }
//     }
// }

// void part2_2(Mapa mapa)
// {
//     tbb::parallel_for(tbb::blocked_range<Coord>(0, LIMIT + 1), [=](tbb::blocked_range<Coord> const& r) {
//         for (Coord y = r.begin(); y != r.end(); ++y) {
//             for (Coord x = 0; x <= LIMIT; ++x) {
//                 const Point px {x, y};
//                 bool covered = false;
//                 for (auto const& [sensor_px, sensor] : mapa) {
//                     if (px == sensor_px || px == sensor.beacon || px.manhattan_dist(sensor_px) <= sensor.dist)
//                     {
//                         covered = true;
//                         break;
//                     }
//                 }
//                 if (!covered) {
//                     fmt::print("2: {}\n", 4000000 * px.x + px.y);
//                     return;
//                 }
//             }
//         }
//     });
// }


void part2_3(Mapa mapa)
{
    for (Coord y = 0; y <= LIMIT; ++y) {
        for (Coord x = 0; x <= LIMIT; ++x) {
            const Point px {x, y};
            bool covered = false;
            for (auto const& [sensor_px, sensor] : mapa) {
                if (px == sensor_px || px == sensor.beacon || px.manhattan_dist(sensor_px) <= sensor.radius) {
                    covered = true;
                    const auto my_dist = sensor.radius - sensor_px.manhattan_dist(px);
                    assert(my_dist >= 0);
                    x += my_dist;
                    break;
                }
            }
            if (!covered) {
                fmt::print("2: {}\n", 4000000 * px.x + px.y);
                return;
            }

        }
    }
    fmt::print("2: none\n");
}


int main()
{
    Mapa mapa;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        boost::smatch r;
        if (!boost::regex_match(line, r, rx)) {
            fmt::print("Error parsing {}\n", line);
            return 1;
        }
        assert(r.size() == 5);

        Point px {};
        Sensor sensor {};
        px.x = std::stoll(r[1].str());
        px.y = std::stoll(r[2].str());
        sensor.beacon.x = std::stoll(r[3].str());
        sensor.beacon.y = std::stoll(r[4].str());
        sensor.radius = px.manhattan_dist(sensor.beacon);
        mapa.insert({px, sensor});
    }

    part1(mapa);
    part2_3(mapa);

    return 0;
}
