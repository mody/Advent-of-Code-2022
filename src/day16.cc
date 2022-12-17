#include <boost/algorithm/string/classification.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/multi_array.hpp>
#include <boost/regex.hpp>
#include <cassert>
#include <cstring>
#include <deque>
#include <fmt/core.h>
#include <iostream>
#include <iterator>
#include <limits>
#include <map>
#include <numeric>
#include <sstream>
#include <string>
#include <vector>

using StringVect = std::vector<std::string>;

using Dist_Type = std::uint32_t;
using Matrix = boost::multi_array<Dist_Type, 2>;
using Index = Matrix::index;
constexpr Dist_Type MAX_DIST = 99999;

struct Pipe
{
    unsigned int rate = 0;
    StringVect destinations {};
};

using Data = std::map<std::string, Pipe>;
using KeySet = std::set<std::string>;

struct State {
    unsigned current = 0;
    KeySet available_moves;
    unsigned valves = 0;
    unsigned minutes = 0;
    unsigned total_score = 0;

    bool operator< (State const & oth) const noexcept {
        return !(operator>(oth));
    }

    bool operator> (State const & oth) const noexcept {
        const unsigned predicted = total_score + ((30 - minutes) * valves);
        const unsigned oth_predicted = oth.total_score + ((30 - oth.minutes) * oth.valves);
        return predicted > oth_predicted;
    }
};

using StateMap = std::map<KeySet, State>;

std::string to_string(KeySet const& ks) {
    std::stringstream ss;
    for (auto const& k : ks) {
        if (ss.tellp() > 0) {
            ss << ",";
        }
        ss << k;
    }
    return ss.str();
}

std::string to_string(State const& s) {
    return fmt::format(
        "(currrent: {}, valves: {}, minutes: {}, total_score: {}, moves: {})",
        s.current,
        s.valves,
        s.minutes,
        s.total_score,
        to_string(s.available_moves));
}

namespace std {

template<>
struct hash<Pipe>
{
    size_t operator()(Pipe const& p) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, p.rate);
        for (auto const& d : p.destinations) {
            boost::hash_combine(seed, d);
        }
        return seed;
    }
};

}  // namespace std


namespace {

// void print(std::ostream& os, const Dist_Type& x)
// {
//     os << x;
// }

// template<typename Array>
// void print(std::ostream& os, const Array& A)
// {
//     typename Array::const_iterator i;

//     os << "[";
//     for (i = A.begin(); i != A.end(); ++i) {
//         print(os, *i);
//         if (boost::next(i) != A.end()) os << ',';
//     }
//     os << "]\n";
// }

};

void part1(Data const& base_data)
{
    const unsigned node_count = base_data.size();
    Matrix distances{boost::extents[node_count][node_count]};

    // default is infinite
    for (Index i = 0; i < node_count; ++i) {
        for (Index j = 0; j < node_count; ++j) {
            distances[i][j] = MAX_DIST;
        }
    }

    // fill in direct distances
    auto key_to_index = [&base_data](const Data::key_type& k) -> Index {
        return std::distance(base_data.begin(), base_data.find(k));
    };
    // auto index_to_key = [&base_data](const Index& i) -> Data::key_type {
    //     return std::next(base_data.begin(), i)->first;
    // };

    for (auto const& [k, p] : base_data) {
        const Index from = key_to_index(k);
        // fmt::print("{} at {} leads to ", k, from);
        for (auto const& dst : p.destinations) {
            const Index to = key_to_index(dst);
            // fmt::print("{} at {},", dst, to);
            distances[from][to] = 1;
            distances[to][from] = 1;
        }
        // fmt::print("\n");
    }

    // clear diagonal
    for (Index i = 0; i < node_count; ++i) {
        distances[i][i] = 0;
    }

    // Floyd–Warshall algorithm
    for (Index k = 0; k < node_count; ++k) {
        for (Index i = 0; i < node_count; ++i) {
            for (Index j = 0; j < node_count; ++j) {
                if (i == j) continue;
                distances[i][j] = std::min(distances[i][j], distances[i][k] + distances[k][j]);
            }
        }
    }

    // print(std::cout, distances);

    StateMap state_map;
    {
        auto& state = state_map[{"AA"}];
        for (auto const& [k, p] : base_data) {
            if (p.rate < 1 || k == "AA") continue;
            state.available_moves.insert(k);
        }
        state.current = key_to_index("AA");
    }

    std::deque<KeySet> q;
    q.push_front({"AA"});

    while(!q.empty()) {
        auto const from = q.front();
        q.pop_front();

        // fmt::print("Starting to process {}\n", to_string(from));

        State const& state = state_map.at(from);

        if (state.minutes >= 30) {
            assert(state.minutes == 30);
            // fmt::print("State finished\n", to_string(state));
            continue;
        }

        // fmt::print("From state {}\n", to_string(state));

        for (auto const& dst : state.available_moves) {
            // fmt::print("Moving from {} to {}\n", index_to_key(state.current), dst);
            auto dist = distances[state.current][key_to_index(dst)];
            // fmt::print("distance: {}\n", dist);

            KeySet dst_key = from;
            dst_key.insert(dst);

            // fmt::print("new key {}\n", to_string(dst_key));

            State new_state = state;
            new_state.current = key_to_index(dst);
            new_state.available_moves.erase(dst);

            new_state.minutes += dist;
            if (new_state.minutes > 30) {
                // fmt::print("Destination too far {}\n", to_string(dst_key));
                continue;
            }
            new_state.total_score += new_state.valves * dist;

            if (new_state.minutes < 30) {
                ++new_state.minutes;
                new_state.total_score += new_state.valves;
                new_state.valves += base_data.at(dst).rate;
            }

            // fmt::print("new state {}\n", to_string(new_state));

            auto it = state_map.find(dst_key);
            if (it != state_map.end()) {
                auto& old_state = it->second;
                // fmt::print("old state {}\n", to_string(old_state));
                if (new_state > old_state) {
                    old_state = new_state;
                    // fmt::print("overvritten\n");
                    q.push_back(std::move(dst_key));
                    // fmt::print("queue new key {}\n", to_string(dst_key));
                }
            } else {
                state_map.insert({dst_key, std::move(new_state)});
                // fmt::print("add new state\n");
                q.push_back(std::move(dst_key));
                // fmt::print("queue new key {}\n", to_string(dst_key));
            }
        }
        // fmt::print("\n");
    }

    // fmt::print("States1\n");
    // for (auto const& [k, s] : state_map) {
    //     fmt::print("{} - {}\n", to_string(k), to_string(s));
    // }

    for (auto& [k, s] : state_map) {
        s.total_score += (30 - s.minutes) * s.valves;
        s.minutes += (30 - s.minutes);
    }

    // fmt::print("States2\n");
    // for (auto const& [k, s] : state_map) {
    //     fmt::print("{} - {}\n", to_string(k), to_string(s));
    // }

    State s_max{};
    for (auto const& [k, s] : state_map) {
        s_max = std::max((const State)s_max, s);
    }

    fmt::print("1: {}\n", s_max.total_score);

    // 1790 is too low
    // 1891 is too low
    // 1916 is too low
    // 2087 !!
}


int main()
{
    Data data {};

    std::string line;
    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }

        static const boost::regex rx {"Valve (..) has flow rate=(\\d+); tunnels? leads? to valves? (.*)"};
        boost::smatch r;
        if (!boost::regex_match(line, r, rx)) {
            fmt::print("Error parsing {}\n", line);
            return 1;
        }
        assert(r.size() == 4);

        // for (size_t i=0; i < r.size(); ++i) {
        //     fmt::print("{}: {}\n", i, r[i].str());
        // }

        Pipe p {};
        p.rate = std::stoi(r[2]);
        boost::split(
            p.destinations,
            r[3].str(),
            boost::algorithm::is_any_of(", "),
            boost::algorithm::token_compress_on);

        data.insert({r[1], std::move(p)});
    }

    // for (const auto& [k, p] : data) {
    //     fmt::print("{}, rate {}, dest: {}\n", k, p.rate, boost::join(p.destinations, ","));
    // }

    part1(data);

    return 0;
}
