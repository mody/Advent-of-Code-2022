#include <boost/container_hash/hash.hpp>

#include <concepts>
#include <type_traits>

namespace Gfx_2d {

struct Direction
{
    int dx = 0, dy = 0;

    template<std::integral T>
    constexpr Direction operator*(const T& num) const noexcept
    {
        Direction d {dx, dy};
        d *= num;
        return d;
    }

    template<std::integral T>
    constexpr Direction& operator*=(const T& num) noexcept
    {
        static_assert(std::numeric_limits<T>::is_integer);
        dx *= num;
        dy *= num;
        return *this;
    }

    constexpr bool operator==(Direction const& o) const noexcept { return dx == o.dx && dy == o.dy; }
};

constexpr Direction North{0, -1};
constexpr Direction South{0,  1};
constexpr Direction West{-1, 0};
constexpr Direction East{ 1, 0};

constexpr Direction NW{-1, -1};
constexpr Direction NE{ 1, -1};
constexpr Direction SW{-1,  1};
constexpr Direction SE{ 1,  1};

constexpr Direction Up{0, -1};
constexpr Direction Down{0,  1};
constexpr Direction Left{-1, 0};
constexpr Direction Right{ 1, 0};

template<std::integral Coord>
struct Point
{
    Coord x {}, y {};

    constexpr Point() noexcept = default;
    constexpr Point(Coord x_, Coord y_) noexcept
        : x {x_}
        , y {y_}
    { }

    constexpr Point(Point const&) noexcept = default;
    constexpr Point(Point&&) noexcept = default;

    constexpr Point& operator=(Point const&) noexcept = default;
    constexpr Point& operator=(Point&&) noexcept = default;

    constexpr bool operator==(Point const& o) const noexcept { return x == o.x && y == o.y; }

    constexpr Point& operator+=(Direction const& o) noexcept
    {
        x += o.dx;
        y += o.dy;
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
        return *this;
    }

    constexpr friend Point operator-(Point lhs, Direction const& o) noexcept
    {
        lhs -= o;
        return lhs;
    }

    constexpr Direction operator-(Point const& o) const noexcept { return {x - o.x, y - o.y}; }

    constexpr bool operator<(Point const& o) const noexcept { return std::tie(x, y) < std::tie(o.x, o.y); }

    constexpr Coord manhattan_dist(Point const& o) const noexcept
    {
        return std::abs(std::make_signed_t<Coord>(x) - std::make_signed_t<Coord>(o.x))
            + std::abs(std::make_signed_t<Coord>(y) - std::make_signed_t<Coord>(o.y));
    }
};

}  // namespace Gfx_2d

namespace std {

template<typename Coord>
struct hash<Gfx_2d::Point<Coord>>
{
    size_t operator()(Gfx_2d::Point<Coord> const& p) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, std::hash<Coord>{}(p.x));
        boost::hash_combine(seed, std::hash<Coord>{}(p.y));
        return seed;
    }
};

}  // namespace std

