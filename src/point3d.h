#include <boost/container_hash/hash.hpp>
#include <cstdint>
#include <tuple>

namespace Gfx_3d {


struct Direction
{
    int dx = 0, dy = 0, dz = 0;
};

constexpr Direction Up{0, -1, 0};
constexpr Direction Down{0,  1, 0};
constexpr Direction Left{-1, 0, 0};
constexpr Direction Right{ 1, 0, 0};
constexpr Direction Closer{ 0, 0, -1};
constexpr Direction Further{0, 0, 1};

template<typename Coord>
struct Point
{
    Coord x {}, y {}, z{};

    Point() noexcept = default;
    Point(Coord x_, Coord y_, Coord z_) noexcept
        : x {x_}
        , y {y_}
        , z {z_}
    { }

    bool operator==(Point const& o) const noexcept { return x == o.x && y == o.y && z == o.z; }

    Point& operator+=(Direction const& o) noexcept
    {
        x += o.dx;
        y += o.dy;
        z += o.dz;
        return *this;
    }

    friend Point operator+(Point lhs, Direction const& o) noexcept
    {
        lhs += o;
        return lhs;
    }

    Point& operator-=(Direction const& o) noexcept
    {
        x -= o.dx;
        y -= o.dy;
        z -= o.dz;
        return *this;
    }

    friend Point operator-(Point lhs, Direction const& o) noexcept
    {
        lhs -= o;
        return lhs;
    }

    Direction operator-(Point const& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }

    bool operator<(Point const& o) const noexcept { return std::tie(x, y, z) < std::tie(o.x, o.y, o.z); }

    Coord manhattan_dist(Point const& o) const noexcept
    {
        return std::abs(x - o.x) + std::abs(y - o.y) + std::abs(z - o.z);
    }
};

}  // namespace Gfx_3d

namespace std {

template<typename Coord>
struct hash<Gfx_3d::Point<Coord>>
{
    size_t operator()(Gfx_3d::Point<Coord> const& p) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, p.x);
        boost::hash_combine(seed, p.y);
        boost::hash_combine(seed, p.z);
        return seed;
    }
};

}  // namespace std
