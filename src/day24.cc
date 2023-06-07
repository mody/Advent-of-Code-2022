#include "point2d.h"

#include <deque>
#include <fmt/core.h>
#include <boost/functional/hash.hpp>

#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>

using Coord = int;
using Point = Gfx_2d::Point<Coord>;
using Positions = std::unordered_set<Point>;

struct Mapa
{
    Coord max_x = 0, max_y = 0;
    Coord dx = 0, dy = 0;
    std::unordered_map<Point, Gfx_2d::Direction> data;

    void set_max(Coord mx, Coord my) noexcept {
        max_x = std::max(max_x, mx);
        max_y = std::max(max_y, my);
        dx = max_x - 2; // substract two walls
        dy = max_y - 2; // substract two walls
    }

    Positions at(const unsigned minutes) const {
        const unsigned x_minutes = minutes % dx;
        const unsigned y_minutes = minutes % dy;

        std::unordered_set<Point> result;
        for (auto const& [p0, dir] : data) {
            Point px = p0;
            if (minutes) {
                if (dir == Gfx_2d::Down) {
                    px.y = ((y_minutes + p0.y - 1) % dy) + 1;
                } else if (dir == Gfx_2d::Up) {
                    px.y = max_y - 2 - p0.y;
                    px.y = ((y_minutes + px.y) % dy) + 1;
                    px.y = max_y - px.y - 1;
                } else if (dir == Gfx_2d::Right) {
                    px.x = ((x_minutes + p0.x - 1) % dx) + 1;
                } else if (dir == Gfx_2d::Left) {
                    px.x = max_x - 2 - p0.x;
                    px.x = ((x_minutes + px.x) % dx) + 1;
                    px.x = max_x - px.x - 1;
                }
            }
            result.insert(std::move(px));
        }
        return result;
    }
};

struct Moment
{
    unsigned minute = 0;
    Point position;

    bool operator==(const Moment& oth) const noexcept
    {
        return minute == oth.minute && position == oth.position;
    }
};

namespace std {

template<>
struct hash<Moment>
{
    size_t operator()(Moment const& m) const noexcept {
        size_t seed = std::hash<Point>()(m.position);
        boost::hash_combine(seed, m.minute);
        return seed;
    }
};

}  // namespace std

unsigned process(Mapa const& mapa, Moment start, Point const dest)
{
    std::deque<Moment> work_queue;
    work_queue.push_front(std::move(start)); // hard-coded start

    // const Point dest = {mapa.max_x - 2, mapa.max_y - 1};

    std::unordered_set<Moment> visited;
    std::map<unsigned, Positions> PositionsCache;

    while(!work_queue.empty()) {
        const Moment now = work_queue.front();
        work_queue.pop_front();

        auto [it, inserted] = PositionsCache.insert({now.minute + 1, {}});
        if (inserted) {
            it->second = mapa.at(now.minute + 1);
        }
        auto const& positions = it->second;

        for (const auto dir : {Gfx_2d::Down, Gfx_2d::Up, Gfx_2d::Left, Gfx_2d::Right, Gfx_2d::Direction{}})
        {
            Moment next = now;
            next.minute++;
            next.position += dir;

            if (next.position == dest) {
                return next.minute;
            }

            if (next.position.x < 0 || next.position.y < 0 || next.position.x > mapa.max_x
                || next.position.y > mapa.max_y)
            {
                // outside of the map
                continue;
            }

            if (visited.contains(next)) {
                // already was there
                continue;
            }

            if (positions.contains(next.position)) {
                // position occupied (tornado or wall)
                continue;
            }

            visited.insert(next);
            work_queue.push_back(std::move(next));
        }
    }
    return 0;
}

void part1(Mapa const& mapa)
{
    unsigned minutes = process(mapa, Moment{0, {1, 0}}, {mapa.max_x - 2, mapa.max_y - 1});
    fmt::print("1: {}\n", minutes);
}

void part2(Mapa const& mapa)
{
    const Point start{1, 0};
    const Point end{mapa.max_x - 2, mapa.max_y - 1};
    unsigned minutes = process(mapa, Moment {0, start}, end);
    minutes = process(mapa, Moment {minutes, end}, start);
    minutes = process(mapa, Moment {minutes, start}, end);
    fmt::print("2: {}\n", minutes);
}

int main()
{
    Mapa mapa;

    {
        std::string line;
        Coord y = 0;
        while(std::getline(std::cin, line))
        {
            if (line.empty()) {
                break;
            }
            Coord x = 0;
            for (auto const& c : line) {
                // fmt::print("{}", c);
                if (c == '>') {
                    mapa.data[{x,y}] = Gfx_2d::Right;
                } else if (c == 'v') {
                    mapa.data[{x,y}] = Gfx_2d::Down;
                } else if (c == '<') {
                    mapa.data[{x,y}] = Gfx_2d::Left;
                } else if (c == '^') {
                    mapa.data[{x,y}] = Gfx_2d::Up;
                } else if (c == '#') {
                    mapa.data[{x,y}] = {};
                }
                ++x;
            }
            if (mapa.max_x) {
                assert(mapa.max_x == x);
            }
            mapa.set_max(x, y);

            x = 0;
            ++y;
            // fmt::print("\n");
        }
        mapa.set_max(0, y);
    }

    fmt::print("max_x={}, max_y={}\n", mapa.max_x, mapa.max_y);

    // for(unsigned i = 0; i < 7; ++i) {
    //     for (auto const& px : mapa.at(i)) {
    //         fmt::print("@{} - {},{}\n", i, px.x, px.y);
    //     }
    // }

    part1(mapa);
    part2(mapa);
}
