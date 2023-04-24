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
#include <unordered_map>
#include <utility>
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

enum class Action
{
    MOVE,
    OPEN
};

struct Player
{
    Action action = Action::MOVE;
    Index position = 0;

    bool operator==(Player const& oth) const noexcept
    {
        return action == oth.action && position == oth.position;
    }
};

struct Players
{
    Player p1, p2;
    unsigned minutes = 0;

    bool operator==(Players const& oth) const noexcept
    {
        return p1 == oth.p1 && p2 == oth.p2 && minutes == oth.minutes;
    }
};

struct State2
{
    State2(Data d)
        : data {std::move(d)}
    { }

    Data data;

    Players players;

    unsigned valves = 0;
    unsigned minutes = 1;
    unsigned total_score = 0;

    bool operator< (State2 const & oth) const noexcept {
        return !(operator>(oth));
    }

    bool operator> (State2 const & oth) const noexcept {
        const unsigned predicted = total_score + ((26 - minutes) * valves);
        const unsigned oth_predicted = oth.total_score + ((26 - oth.minutes) * oth.valves);
        return predicted > oth_predicted;
    }
};

using StateMap2 = std::unordered_map<Players, State2>;

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

template<>
struct hash<Player>
{
    size_t operator()(Player const& p) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, p.action);
        boost::hash_combine(seed, p.position);
        return seed;
    }
};

template<>
struct hash<Players>
{
    size_t operator()(Players const& p) const noexcept
    {
        size_t seed = 0;
        boost::hash_combine(seed, std::hash<Player>()(p.p1));
        boost::hash_combine(seed, std::hash<Player>()(p.p2));
        boost::hash_combine(seed, p.minutes);
        return seed;
    }
};

}  // namespace std


namespace {

[[maybe_unused]] Index key_to_index(Data const& base_data, const Data::key_type& k)
{
    return std::distance(base_data.begin(), base_data.find(k));
};

[[maybe_unused]] Data::key_type index_to_key(Data const& base_data, const Index& i)
{
    return std::next(base_data.begin(), i)->first;
};

[[maybe_unused]] std::string to_string(KeySet const& ks)
{
    std::stringstream ss;
    for (auto const& k : ks) {
        if (ss.tellp() > 0) {
            ss << ",";
        }
        ss << k;
    }
    return ss.str();
}

[[maybe_unused]] std::string to_string(State const& s)
{
    return fmt::format(
        "(currrent: {}, valves: {}, minutes: {}, total_score: {}, moves: {})",
        s.current,
        s.valves,
        s.minutes,
        s.total_score,
        to_string(s.available_moves));
}

[[maybe_unused]] std::string to_string(State2 const& s)
{
    return fmt::format(
        "(currrent: {}/{}, valves: {}, minutes: {}, total_score: {})",
        index_to_key(s.data, s.players.p1.position),
        index_to_key(s.data, s.players.p2.position),
        s.valves,
        s.minutes,
        s.total_score);
}

[[maybe_unused]] void print(std::ostream& os, const Dist_Type& x)
{
    os << x;
}


template<typename Array>
void print(std::ostream& os, const Array& A)
{
    typename Array::const_iterator i;

    os << "[";
    for (i = A.begin(); i != A.end(); ++i) {
        print(os, *i);
        if (boost::next(i) != A.end()) os << ',';
    }
    os << "]\n";
}


Matrix calculate_distances(Data const& base_data)
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
    for (auto const& [k, p] : base_data) {
        const Index from = key_to_index(base_data, k);
        // fmt::print("{} at {} leads to ", k, from);
        for (auto const& dst : p.destinations) {
            const Index to = key_to_index(base_data, dst);
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

    return distances;
}

};  // namespace

void part1(Data const& base_data, Matrix const& distances)
{
    StateMap state_map;
    {
        auto& state = state_map[{"AA"}];
        for (auto const& [k, p] : base_data) {
            if (p.rate < 1 || k == "AA") continue;
            state.available_moves.insert(k);
        }
        state.current = key_to_index(base_data, "AA");
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
            auto dist = distances[state.current][key_to_index(base_data, dst)];
            // fmt::print("distance: {}\n", dist);

            KeySet dst_key = from;
            dst_key.insert(dst);

            // fmt::print("new key {}\n", to_string(dst_key));

            State new_state = state;
            new_state.current = key_to_index(base_data, dst);
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


void part2(Data const& base_data, Matrix const& distances)
{
    StateMap2 state_map;
    state_map.insert(std::make_pair(Players {}, State2 {base_data}));  // initial state at position 0 -> AA

    std::deque<Players> q;
    q.push_back({});  // initial state at position 0 -> AA

    auto next_player_action = [](State2 const& state, Player const& player) {
        std::vector<Player> result;

        Pipe const& pipe = state.data.at(index_to_key(state.data, player.position));

        if (pipe.rate > 0) {
            // open current valve
            result.push_back({Action::OPEN, player.position});
        }

        // add valves near
        for (auto const& pos : pipe.destinations) {
            result.push_back({Action::MOVE, key_to_index(state.data, pos)});
        }

        return result;
    };


    while (!q.empty()) {
        Players current = q.front();
        q.pop_front();

        State2 const& current_state = state_map.at(current);

        for (auto p1 : next_player_action(current_state, current_state.players.p1)) {
            for (auto p2 : next_player_action(current_state, current_state.players.p2)) {
                State2 new_state = current_state;
                ++new_state.minutes;
                new_state.players = {p1, p2, new_state.minutes};

                if (new_state.minutes >= 27) {
                    continue;
                }

                if (p1.action == Action::OPEN) {
                    // open valve
                    Pipe& pipe = new_state.data.at(index_to_key(new_state.data, p1.position));
                    if (pipe.rate > 0) {
                        new_state.valves += pipe.rate;
                        pipe.rate = 0;
                    }
                }
                if (p2.action == Action::OPEN) {
                    // open valve
                    Pipe& pipe = new_state.data.at(index_to_key(new_state.data, p2.position));
                    if (pipe.rate > 0) {
                        new_state.valves += pipe.rate;
                        pipe.rate = 0;
                    }
                }
                new_state.total_score += new_state.valves;

                // fmt::print("cur: {} new: {}\n", to_string(current_state), to_string(new_state));

                auto it = state_map.find(new_state.players);
                if (it == state_map.end()) {
                    state_map.insert({new_state.players, new_state});
                    q.push_back(new_state.players);
                } else {
                    auto& old_state = it->second;
                    if (new_state > old_state) {
                        q.push_back(new_state.players);
                        old_state = std::move(new_state);
                    }
                }
            }
        }
    }

    for (auto& [k, s] : state_map) {
        s.total_score += (26 - s.minutes) * s.valves;
        s.minutes += (26 - s.minutes);
    }

    State2 s_max {base_data};
    for (auto const& [k, s] : state_map) {
        s_max = std::max((const State2)s_max, s);
    }

    fmt::print("2: {}\n", s_max.total_score);
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

    const Matrix distances = calculate_distances(data);

    part1(data, distances);
    part2(data, distances);

    return 0;
}
