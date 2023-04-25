#include <algorithm>
#include <array>
#include <deque>
#include <iostream>
#include <limits>
#include <memory>
#include <numeric>
#include <optional>
#include <set>
#include <string>
#include <map>
#include <vector>

#include <boost/functional/hash.hpp>
#include <boost/regex.hpp>
#include <fmt/core.h>

// #define DO_LOG

enum Material {
    ORE = 0,
    CLAY,
    OBSIDIAN,
    GEODE,
    MATERIAL_SIZE // always last
};

static const std::map<std::string, Material> Str2Material = {
    {"ore", Material::ORE},
    {"clay", Material::CLAY},
    {"obsidian", Material::OBSIDIAN},
    {"geode", Material::GEODE},
};
static const std::array<const char*, MATERIAL_SIZE> Material2Str = {"ore", "clay", "obsidian", "geode"};


using Materials = std::array<unsigned, MATERIAL_SIZE>;

struct Robot {
    Materials requirement = {0};

    unsigned max(unsigned mat) const noexcept
    {
        return requirement.at(mat);
    }
};

using Robots = std::array<Robot, MATERIAL_SIZE>;

struct Blueprint
{
    unsigned id = 0;
    Robots robots = {0};
    Materials maximums = {0};

    void calculate_maximums() noexcept
    {
        for (auto const& r : robots) {
            for (unsigned i = 0; i < r.requirement.size(); ++i) {
                maximums.at(i) = std::max(maximums.at(i), r.max(i));
            }
        }
    }

    unsigned max(unsigned mat) const noexcept
    {
        return maximums.at(mat);
    }

    bool empty() const noexcept
    {
        return robots.empty();
    }
};

using Blueprints = std::vector<Blueprint>;


struct Storage
{
    Materials storage = {0};
    Materials diff = {0};

    void mine() noexcept
    {
        for (unsigned i = 0; i < storage.size(); ++i) {
            storage.at(i) += diff.at(i);
        }
    }

    bool can_build_robot(Robot const& r) const noexcept
    {
        bool can_build = true;
        for (unsigned i = 0; i < r.requirement.size(); ++i) {
            if (storage.at(i) < r.requirement.at(i)) {
                // fmt::print("have {} of {}, need {}\n", storage.at(i), Material2Str.at(i), r.requirement.at(i));
                can_build = false;
                break;
            }
        }
        return can_build;
    }

    bool build_robot(Robot const& r) noexcept
    {
        if (can_build_robot(r)) {
            for (unsigned i = 0; i < r.requirement.size(); ++i) {
                storage.at(i) -= r.requirement.at(i);
            }
            return true;
        }
        return false;
    }

    void apply(unsigned product) noexcept
    {
        diff.at(product) += 1;
    }
};


struct State
{
    unsigned minute = 0;
    Storage storage;

    State() noexcept
    {
        storage.diff.at(Material::ORE) = 1;
    }
};


namespace boost
{

template<>
struct hash<Material>
{
    size_t operator()(Material const& m) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, Material2Str.at(m));
        return seed;
    }
};

template<>
struct hash<Materials>
{
    size_t operator()(Materials const& r) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, r);
        return seed;
    }
};

template<>
struct hash<Storage>
{
    size_t operator()(Storage const& s) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, s.storage);
        boost::hash_combine(seed, s.diff);
        return seed;
    }
};

// template<>
// struct hash<Robot>
// {
//     size_t operator()(Robot const& r) const noexcept
//     {
//         size_t seed = 0;
//         boost::hash_combine(seed, r.requirement);
//         return seed;
//     }
// };

template<>
struct hash<State>
{
    size_t operator()(State const& s) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, s.minute);
        boost::hash_combine(seed, s.storage);
        return seed;
    }
};

}  // namespace boost

#ifdef DO_LOG
[[maybe_unused]] void print(Materials const& r) noexcept
{
    for (unsigned i = 0; i < r.size(); ++i) {
        fmt::print("\t{} = {}\n", Material2Str.at(i), r.at(i));
    }
}

[[maybe_unused]] void print(Storage const& s) noexcept
{
    fmt::print("Storage:\n");
    print(s.storage);
    fmt::print("Diffs:\n");
    print(s.diff);
}

[[maybe_unused]] void print(unsigned product, Robot const& r) noexcept
{
    fmt::print("Robot:\n");
    fmt::print("\tProduct: {}\n", Material2Str.at(product));
    fmt::print("\tRequirements:\n");
    print(r.requirement);
    fmt::print("\n");
}

[[maybe_unused]] void print(State const& state) noexcept
{
    fmt::print("\nState at {}\n", state.minute);
    print(state.storage);
    fmt::print("\n");
}
#endif

void part1(Blueprints const& blueprints)
{
    constexpr int MINUTE_LIMIT = 24;
    unsigned quality_level = 0;

    for (auto const& blueprint : blueprints) {
        State initial;
        unsigned max_geodes = 0;

        std::deque<State> work;
        work.push_front(std::move(initial));

        while(!work.empty()) {
            const State state = work.front();
            work.pop_front();
#ifdef DO_LOG
            print(state);
#endif
            if (state.minute >= MINUTE_LIMIT) {
                max_geodes = std::max(max_geodes, state.storage.storage.at(Material::GEODE));
                continue;
            }

            {
                State next_state = state;
                next_state.minute++;

#ifdef DO_LOG
                fmt::print("Noop state\n");
#endif
                next_state.storage.mine();
                work.push_front(std::move(next_state));
            }

            // create robots
            for (unsigned mat : {Material::ORE, Material::CLAY, Material::OBSIDIAN}) {
                State next_state = state;
                next_state.minute++;

                // if (next_state.storage.diff.at(mat) >= blueprint.max(mat)
                //     && next_state.storage.storage.at(mat) >= blueprint.max(mat)
                //     )

                // if (next_state.storage.diff.at(mat) >= (2*blueprint.max(mat))
                //     || next_state.storage.storage.at(mat) >= (2*blueprint.max(mat))
                //     )

                if (next_state.storage.diff.at(mat) > blueprint.max(mat)
                    || next_state.storage.storage.at(mat) > blueprint.max(mat) + 1
                    )
                {
#ifdef DO_LOG
                    fmt::print("No need to build robot for {}\n", Material2Str.at(mat));
#endif
                    continue;
                }

                auto const& r = blueprint.robots.at(mat);
                if (!next_state.storage.build_robot(r)) {
#ifdef DO_LOG
                    fmt::print("Cant build robot for {}\n", Material2Str.at(mat));
#endif
                    continue;
                }
#ifdef DO_LOG
                fmt::print("Built robot for {}\n", Material2Str.at(mat));
#endif
                next_state.storage.mine();
                next_state.storage.apply(mat);
                work.push_front(std::move(next_state));
            }

            {
                State next_state = state;
                next_state.minute++;

                auto const& r = blueprint.robots.at(Material::GEODE);
                if (next_state.storage.build_robot(r)) {
#ifdef DO_LOG
                    fmt::print("Built robot for geode\n");
#endif
                    next_state.storage.mine();
                    next_state.storage.apply(Material::GEODE);
                    work.push_front(std::move(next_state));
                } else {
#ifdef DO_LOG
                    fmt::print("Cant build robot for geode\n");
#endif
                }
            }
        }
// #ifdef DO_LOG
        fmt::print("{}: {}\n", blueprint.id, max_geodes);
// #endif
        quality_level += blueprint.id * max_geodes;
    }
    fmt::print("1: {}\n", quality_level);
}


int main()
{
    Blueprints blueprints;

    std::string line;
    while (std::getline(std::cin, line))
    {
        if (line.empty())
        {
            break;
        }

        Blueprint b;

        b.id = strtol(&line[10], nullptr, 10);

        static boost::regex rx{"Each ([a-z]+) robot costs ([0-9]+) ([a-z]+)( and ([0-9]+) ([a-z]+))?\\."};
        auto rit = boost::make_regex_iterator(line, rx);
        std::for_each(rit, decltype(rit) {}, [&b](boost::smatch const& m) {
            // std::cout << "results: " << m.size() << std::endl;
            // for (unsigned i = 0; i < m.size(); ++i) {
            //     std::cout << i << ": " << m[i].str() << std::endl;
            // }
            assert(m.size() > 4);
            Robot& r = b.robots.at(Str2Material.at(m[1]));
            r.requirement.at(Str2Material.at(m[3])) = (unsigned)std::stoi(m[2]);

            if (m.size() == 7 && !m[4].str().empty()) {
                r.requirement.at(Str2Material.at(m[6])) = (unsigned)std::stoi(m[5]);
            }
#ifdef DO_LOG
            print(Str2Material.at(m[1]), r);
#endif
        });

        if (!b.empty()) {
            b.calculate_maximums();
            blueprints.push_back(std::move(b));
        }
        // std::cout << std::endl;
    }

    part1(blueprints);


    return 0;
}
