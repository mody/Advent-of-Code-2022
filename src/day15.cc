#include "point2d.h"

#include <boost/regex.hpp>
#include <boost/regex/v5/regex.hpp>
#include <boost/regex/v5/regex_fwd.hpp>
#include <boost/regex/v5/regex_match.hpp>
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
    Coord dist{};
};

using Mapa = std::map<Point, Sensor>;


void part1(Mapa mapa)
{
    constexpr Coord LEVEL_Y = 2000000;
    Coord min_x = std::numeric_limits<Coord>::max(), max_x = std::numeric_limits<Coord>::min();

    for (auto const& [px, sensor] : mapa) {
        min_x = std::min(min_x, px.x - sensor.dist);
        max_x = std::max(max_x, px.x + sensor.dist);
    }
    max_x++; // one after to make it end iterator

    Coord score = 0;

    for (Point px {min_x, LEVEL_Y}; px != Point {max_x, LEVEL_Y}; px += Gfx_2d::Right) {
        for (auto const& [sensor_px, sensor] : mapa) {
            if (px != sensor_px && px != sensor.beacon && px.manhattan_dist(sensor_px) <= sensor.dist) {
                ++score;
                break;
            }
        }
    }

    fmt::print("1: {}\n", score);
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
        sensor.dist = px.manhattan_dist(sensor.beacon);
        mapa.insert({px, sensor});
    }

    part1(mapa);

    return 0;
}
