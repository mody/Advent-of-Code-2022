#include "point2d.h"

#include <boost/graph/adjacency_list.hpp>
#include <boost/graph/compressed_sparse_row_graph.hpp>
#include <boost/graph/dijkstra_shortest_paths.hpp>
#include <boost/graph/graph_selectors.hpp>
#include <boost/graph/graph_traits.hpp>

#include <cassert>
#include <cstring>
#include <deque>
#include <fmt/core.h>
#include <iostream>
#include <map>
#include <string>

using Point = Gfx_2d::Point<int>;


class World
{
    std::string data;
    int32_t max_x = 0, max_y = 0;

    int32_t start = -1;
    int32_t end = -1;

public:
    void append(std::string input) noexcept
    {
        max_x = input.size();

        int32_t pos = input.find('S');
        if (pos != std::string::npos) {
            start = data.size() + pos;
            input.at(pos) = 'a';
        }
        pos = input.find('E');
        if (pos != std::string::npos) {
            end = data.size() + pos;
            input.at(pos) = 'z';
        }

        data.append(std::move(input));
        ++max_y;
    }

    int32_t get_size() const noexcept
    {
        assert(data.size() == (max_x * max_y));
        return data.size();
    }

    int32_t xy_to_index(Point const& px) const noexcept {
        return (max_x * px.y) + px.x;
    }

    Point index_to_xy(int32_t index) const noexcept {
        return {index % max_x, index / max_x};
    }

    bool has(Point const& px) const noexcept {
        return px.x >= 0 && px.x < max_x && px.y >= 0 && px.y < max_y;
    }

    bool has(int32_t index) const noexcept {
        return index >= 0 && index < data.size();
    }

    unsigned char get(Point const& px) const { return data.at(xy_to_index(px)); }
    unsigned char get(int32_t index) const { return data.at(index); }

    int32_t get_start() const noexcept { return start; }
    int32_t get_end() const noexcept { return end; }

    void dump() const noexcept
    {
        std::string line;
        line.reserve(max_x);
        for (int32_t y = 0; y < max_y; ++y) {
            for (int32_t x = 0; x < max_x; ++x) {
                line += get({x, y});
            }
            std::cout << line << "\n";
            line.clear();
        }
    }

    void part1();
    void part2();
};


void World::part1()
{
    dump();
    assert(get_start() >= 0);
    assert(get_start() < get_size());
    assert(get_end() >= 0);
    assert(get_end() < get_size());

    using EdgeWeightProp = boost::property<boost::edge_weight_t, unsigned>;
    using Graph = boost::adjacency_list<
        boost::vecS,
        boost::vecS,
        boost::directedS,
        boost::property<boost::vertex_distance_t, int32_t>,
        EdgeWeightProp>;
    using Vertex = boost::graph_traits<Graph>::vertex_descriptor;

    Graph g;
    std::vector<Vertex> vMap;

    for (int32_t idx = 0; idx < get_size(); ++idx) {
        vMap.push_back(boost::add_vertex(g));
    }

    for (int32_t idx = 0; idx < get_size(); ++idx) {
        auto const& pxFrom = index_to_xy(idx);

        for (auto const& dir : {Gfx_2d::North, Gfx_2d::South, Gfx_2d::East, Gfx_2d::West}) {
            const Point pxTo = pxFrom + dir;
            if (!has(pxTo)) {
                continue;
            }
            const int c1 = get(pxFrom);
            const int c2 = get(pxTo);
            if (c2 - c1 > 1) {
                continue;
            }
            boost::add_edge(vMap.at(xy_to_index(pxFrom)), vMap.at(xy_to_index(pxTo)), 1, g);
        }
    }

    boost::dijkstra_shortest_paths(
        g, vMap.at(get_start()), boost::distance_map(boost::get(boost::vertex_distance, g)));

    auto const& dist = boost::get(boost::vertex_distance, g);
    const int32_t d = dist[vMap.at(get_end())];

    fmt::print("1: {}\n", d);
}

int main()
{
    World world;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        world.append(line);
    }

    world.part1();

    return 0;
}
