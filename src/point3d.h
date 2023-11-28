#include <boost/container_hash/hash.hpp>

#include <concepts>
#include <type_traits>

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

template<std::integral Coord>
struct Point
{
    Coord x {}, y {}, z{};

    constexpr Point() noexcept = default;
    constexpr Point(Coord x_, Coord y_, Coord z_) noexcept
        : x {x_}
        , y {y_}
        , z {z_}
    { }

    constexpr Point(Point const&) noexcept = default;
    constexpr Point(Point&&) noexcept = default;

    constexpr Point& operator=(Point const&) noexcept = default;
    constexpr Point& operator=(Point&&) noexcept = default;

    constexpr bool operator==(Point const& o) const noexcept { return x == o.x && y == o.y && z == o.z; }

    constexpr Point& operator+=(Direction const& o) noexcept
    {
        x += o.dx;
        y += o.dy;
        z += o.dz;
        return *this;
    }

    constexpr friend Point operator+(Point lhs, Direction const& o) noexcept
    {
        lhs += o;
        return lhs;
    }

    constexpr Point& operator-=(Direction const& o) noexcept
    {
        x -= o.dx;
        y -= o.dy;
        z -= o.dz;
        return *this;
    }

    constexpr friend Point operator-(Point lhs, Direction const& o) noexcept
    {
        lhs -= o;
        return lhs;
    }

    constexpr Direction operator-(Point const& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }

    constexpr bool operator<(Point const& o) const noexcept { return std::tie(x, y, z) < std::tie(o.x, o.y, o.z); }

    constexpr Coord manhattan_dist(Point const& o) const noexcept
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
        boost::hash_combine(seed, std::hash<Coord>{}(p.x));
        boost::hash_combine(seed, std::hash<Coord>{}(p.y));
        boost::hash_combine(seed, std::hash<Coord>{}(p.z));
        return seed;
    }
};

}  // namespace std
