#include "point2d.h"

#include <cassert>
#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <memory>
#include <set>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

using Coord = std::int64_t;
using Point = Gfx_2d::Point<Coord>;

static constexpr Coord MIN_X = 0;
static constexpr Coord MAX_X = 8;

using Mapa = std::unordered_map<Point, unsigned char>;

class Game;

class Brick
{
public:
    Brick(Game* g)
        : game {g}
    {
        assert(game);
    }

    virtual ~Brick() = default;

    virtual bool move_down();
    virtual bool move_wind(unsigned char c);
    virtual void to_map() const;

protected:
    virtual bool move_it(Gfx_2d::Direction const& d);

    std::vector<Point> points;
    Game* game {};
};

class Line : public Brick
{
public:
    Line(Coord base_y, Game* g)
        : Brick(g)
    {
        points.push_back({3, base_y});
        points.push_back({4, base_y});
        points.push_back({5, base_y});
        points.push_back({6, base_y});
    }
};

class Plus : public Brick
{
public:
    Plus(Coord base_y, Game* g)
        : Brick(g)
    {
        points.push_back({4, base_y});
        points.push_back({4, base_y + 1});
        points.push_back({4, base_y + 2});
        points.push_back({3, base_y + 1});
        points.push_back({5, base_y + 1});
    }
};

class Lima : public Brick
{
public:
    Lima(Coord base_y, Game* g)
        : Brick(g)
    {
        points.push_back({3, base_y});
        points.push_back({4, base_y});
        points.push_back({5, base_y});
        points.push_back({5, base_y + 1});
        points.push_back({5, base_y + 2});
    }
};

class Snake : public Brick
{
public:
    Snake(Coord base_y, Game* g)
        : Brick(g)
    {
        points.push_back({3, base_y});
        points.push_back({3, base_y + 1});
        points.push_back({3, base_y + 2});
        points.push_back({3, base_y + 3});
    }
};

class Box : public Brick
{
public:
    Box(Coord base_y, Game* g)
        : Brick(g)
    {
        points.push_back({3, base_y});
        points.push_back({4, base_y});
        points.push_back({3, base_y + 1});
        points.push_back({4, base_y + 1});
    }
};


class Game
{
public:
    void setWind(std::string w) noexcept
    {
        wind = std::move(w);
        wind_pos = 0;
    }

    bool simulate_piece();

    void clear_states() noexcept
    {
        states.clear();
        // mapa.clear();
    }

    Coord get_max_y() const noexcept
    {
        Coord max_y = 0;
        for (auto const& [px, _] : mapa) {
            max_y = std::max(max_y, px.y);
        }
        return max_y;
    }

    std::string top_line() const noexcept
    {
        std::stringstream ss;
        Coord max_y = get_max_y();
        for (Coord y = max_y; y >= std::max(max_y - 100, (Coord)0); --y) {
            for (Coord x = MIN_X; x <= MAX_X; ++x) {
                const Point p {x, y};
                if (const auto it = mapa.find(p); it != mapa.end()) {
                    ss << it->second;
                } else {
                    ss << ".";
                }
            }
            ss << "\n";
        }
        return ss.str();
    }

    std::string top_line2() const noexcept
    {
        std::stringstream ss;
        Coord max_y = get_max_y();
        for (Coord x = MIN_X + 1; x < MAX_X; ++x) {
            Coord deep = 0;
            for (Coord y = max_y; y > 0; --y) {
                if (mapa.contains(Point{x, y})) {
                    ss << deep << ',';
                    break;
                }
                ++deep;
            }
        }
        return ss.str();
    }

    void dump() const noexcept
    {
        Coord max_y = get_max_y();
        for (Coord y = max_y; y >= 0; --y) {
            for (Coord x = MIN_X + 1; x < MAX_X; ++x) {
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
        fmt::print("dims: {}-{} x {}-{}\n", MIN_X, MAX_X, 0, max_y);
        std::cout.flush();
    }

    Mapa const& get_map() const noexcept { return mapa; }
    Mapa& get_map() noexcept { return mapa; }

    unsigned char next_wind();

private:
    std::string wind;
    size_t wind_pos = 0;

    unsigned piece_id = 0;

    Mapa mapa;

    std::set<size_t> states;
};


bool Brick::move_down()
{
    return move_it(Gfx_2d::Up);
}

bool Brick::move_wind(unsigned char c)
{
    Gfx_2d::Direction dir;
    if (c == '<') {
        dir = Gfx_2d::Left;
        // fmt::print("left\n");
    } else if (c == '>') {
        dir = Gfx_2d::Right;
        // fmt::print("right\n");
    } else if (c == '>') {
    } else {
        assert(false);
    }
    return move_it(dir);
}

bool Brick::move_it(Gfx_2d::Direction const& d)
{
    for (auto& p : points) {
        p += d;
    }

    bool collision = false;
    for (auto const& p : points) {
        if (p.y == 0 || p.x == MIN_X || p.x == MAX_X || game->get_map().contains(p)) {
            // fmt::print("collision: {},{},{},{}\n", p.y == 0, p.x == MIN_X, p.x == MAX_X, game->get_map().contains(p));
            collision = true;
            break;
        }
    }

    if (collision) {
        for (auto& p : points) {
            p -= d;
        }
    }

    return collision;
}

void Brick::to_map() const
{
    for (auto& p : points) {
        game->get_map().insert({p, '#'});
    }
}

unsigned char Game::next_wind()
{
    unsigned char c = wind.at(wind_pos++);
    wind_pos = wind_pos % wind.size();
    return c;
}

bool Game::simulate_piece()
{
    {
        size_t h = 13;
        boost::hash_combine(h, piece_id);
        boost::hash_combine(h, wind_pos);
        boost::hash_combine(h, top_line2());

        if (states.insert(h).second == false) {
            // already known state
            return false;
        }
    }

    std::shared_ptr<Brick> brick;

    switch (Coord base_y = get_max_y() + 4; piece_id) {
    case 0: brick.reset(new Line(base_y, this)); break;
    case 1: brick.reset(new Plus(base_y, this)); break;
    case 2: brick.reset(new Lima(base_y, this)); break;
    case 3: brick.reset(new Snake(base_y, this)); break;
    case 4: brick.reset(new Box(base_y, this)); break;
    default: assert(false);
    }

    assert(brick);

    for (;;) {
        brick->move_wind(wind.at(wind_pos));
        wind_pos = (wind_pos + 1) % wind.size();
        if (brick->move_down()) {
            brick->to_map();
            break;
        }
    }

    piece_id = (piece_id + 1) % 5;

    return true;
}

struct Repeated {
    Coord iter {}, diff {};
};

void part(Game game, Coord iterations)
{
    Coord computed_y = 0;
    std::vector<Repeated> diffs;

    for (Coord iter = 0; iter < iterations; ++iter) {
        if (!game.simulate_piece()) {
            diffs.push_back({iter, game.get_max_y()});
            break;
        }
    }
    assert(diffs.size() == 1);

    // for (auto const& d : diffs) {
    //     fmt::print("iter: {}, height: {}\n", d.iter, d.diff);
    // }
    // fmt::print("\n");
    // std::cout.flush();

    iterations -= diffs.at(0).iter;
    computed_y = diffs.at(0).diff;

    game.clear_states();

    for (Coord iter = 0; iter < iterations; ++iter) {
        if (!game.simulate_piece()) {
            diffs.push_back({iter, game.get_max_y() - diffs.at(0).diff});
            break;
        }
    }

    if (diffs.size() == 1) {
        diffs.push_back({iterations, game.get_max_y() - diffs.at(0).diff});
    }

    assert(diffs.size() == 2);

    // for (auto const& d : diffs) {
    //     fmt::print("iter: {}, height: {}\n", d.iter, d.diff);
    // }
    // fmt::print("\n");
    // std::cout.flush();

    computed_y += (iterations / diffs.at(1).iter) * diffs.at(1).diff;

    // the rest
    iterations = iterations % diffs.at(1).iter;

    if (iterations) {
        game.clear_states();

        for (Coord iter = 0; iter < iterations; ++iter) {
            if (!game.simulate_piece()) {
                assert(false);
                break;
            }
        }
        diffs.push_back({iterations, game.get_max_y() - diffs.at(0).diff - diffs.at(1).diff});
        assert(diffs.size() == 3);

        // for (auto const& d : diffs) {
        //     fmt::print("iter: {}, height: {}\n", d.iter, d.diff);
        // }
        // fmt::print("\n");
        // std::cout.flush();

        computed_y += diffs.at(2).diff;
    }

    fmt::print("{}\n", computed_y);
}

int main()
{
    Game game;

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        game.setWind(std::move(line));
    }

    part(game, 2022);
    part(game, 1000000000000);

    return 0;
}
