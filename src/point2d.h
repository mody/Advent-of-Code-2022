#include <boost/container_hash/hash.hpp>
#include <cstdint>
#include <tuple>

namespace Gfx_2d {


struct Direction
{
    int dx = 0, dy = 0;
};

constexpr Direction North{0, -1};
constexpr Direction South{0,  1};
constexpr Direction West{-1, 0};
constexpr Direction East{ 1, 0};

constexpr Direction Up{0, -1};
constexpr Direction Down{0,  1};
constexpr Direction Left{-1, 0};
constexpr Direction Right{ 1, 0};

template<typename Coord>
struct Point
{
    Coord x {}, y {};

    Point() noexcept = default;
    Point(Coord x_, Coord y_) noexcept
        : x {x_}
        , y {y_}
    { }

    bool operator==(Point const& o) const noexcept { return x == o.x && y == o.y; }

    Point& operator+=(Direction const& o) noexcept
    {
        x += o.dx;
        y += o.dy;
        return *this;
    }

    friend Point operator+(Point lhs, Direction const& o) noexcept
    {
        lhs += o;
        return lhs;
    }

    Direction operator-(Point const& o) const noexcept { return {x - o.x, y - o.y}; }

    bool operator<(Point const& o) const noexcept { return std::tie(x, y) < std::tie(o.x, o.y); }

    Coord manhattan_dist(Point const& o) const noexcept { return std::abs(x - o.x) + std::abs(y - o.y); }
};

}  // namespace Gfx_2d

namespace std {

template<typename Coord>
struct hash<Gfx_2d::Point<Coord>>
{
    size_t operator()(Gfx_2d::Point<Coord> const& p) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);
        return seed;
    }
};

}  // namespace std

