#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

#include <fmt/core.h>

using Storage = std::deque<int>;

[[maybe_unused]] void print(Storage const& values) noexcept
{
    for (auto const& v : values) {
        fmt::print("{}\n", v);
    }
}


#if 0
template<typename It>
void move_element(const It& b, It it, const It& e, const int N)
{
    // fmt::print("move_element - N {} it={}\n", N, *it);
    It next = std::next(it, 1);
    for (int i = 0; i < N; ++i) {
        // fmt::print("\t{} of {}\n", i, N);
        if (next == e) {
            // fmt::print("\t{} next is end\n", i);
            next = b;
        }
        // fmt::print("\t{} swap {} and {}\n", i, *it, *next);
        std::iter_swap(it++, next++);
        if (it == e) {
            // fmt::print("\t{} it is end\n", i);
            it = b;
        }
    }
    // fmt::print("move_element - N {} - done\n", N);
}


void part1(Storage values)
{
    constexpr int BASELINE = 100000;

    for (auto& v : values) {
        v += BASELINE;
    }

    const long values_size = values.size();

    for (auto pos = values.begin();; pos = values.begin()) {
        // fmt::print("\n");
        // print(values);
        for (; pos != values.end() && *pos < (BASELINE/1000); ++pos)
            ;
        if(pos == values.end()) {
            break;
        }

        *pos -= BASELINE;

        int dest = *pos % values_size;
        // fmt::print("value {}, dest {}\n", *pos, dest);

        dest += std::distance(values.begin(), pos);
        // fmt::print("dest {}\n", dest);

        if (dest == 0) {
            dest = values_size - 1;
        } else if (dest < 0) {
            dest += values_size - 1;
        } else if (dest > values_size) {
            dest -= values_size - 1;
        }
        // fmt::print("dest {}\n", dest);

        dest -= std::distance(values.begin(), pos);
        // fmt::print("dest {}\n", dest);

        if (dest < 0) {
            move_element(values.rbegin(), --std::make_reverse_iterator(pos), values.rend(), std::abs(dest));
        }
        if (dest > 0) {
            move_element(values.begin(), pos, values.end(), dest);
        }

        // print(values);
        // fmt::print("\n");
    }

    auto it = std::find(values.begin(), values.end(), 0);
    assert(it != values.end());
    int dist = std::distance(values.begin(), it);

    fmt::print("1000th = {}\n", values.at((dist+1000)%values_size));
    fmt::print("2000th = {}\n", values.at((dist+2000)%values_size));
    fmt::print("3000th = {}\n", values.at((dist+3000)%values_size));
    fmt::print(
        "1: {}\n",
        values.at((dist + 1000) % values_size) + values.at((dist + 2000) % values_size)
            + values.at((dist + 3000) % values_size));

    // -12539 INCORRECT
}
#endif

#if 0
void part1(Storage values)
{
    constexpr int BASELINE = 100000;

    for (auto& v : values) {
        v += BASELINE;
    }

    const long values_size = values.size();

    for (auto pos = values.begin();; pos = values.begin()) {
        // fmt::print("\n");
        // print(values);
        for (; pos != values.end() && *pos < (BASELINE/1000); ++pos)
            ;
        if(pos == values.end()) {
            break;
        }

        const int val = *pos - BASELINE;
        const int current_pos = std::distance(values.begin(), pos);
        int dest = (val + current_pos) % (values_size - 1);
        if (dest <= 0) {
            dest += values_size - 1;
        }

        // fmt::print("Moving {} from {} to {}\n", val, current_pos, dest);
        pos = values.erase(pos);
        values.insert(std::next(values.begin(), dest), val);
        // print(values);
    }

    Storage v2(values.begin(), values.end());

    auto it = std::find(v2.begin(), v2.end(), 0);
    assert(it != v2.end());
    int dist = std::distance(v2.begin(), it);

    fmt::print("1000th = {}\n", v2.at((dist+1000)%values_size));
    fmt::print("2000th = {}\n", v2.at((dist+2000)%values_size));
    fmt::print("3000th = {}\n", v2.at((dist+3000)%values_size));
    fmt::print(
        "1: {}\n",
        v2.at((dist + 1000) % values_size) + v2.at((dist + 2000) % values_size)
            + v2.at((dist + 3000) % values_size));

    // -107839 INCORRECT
}
#endif

void part1(Storage values)
{
    constexpr int BASELINE = 100000;

    for (auto& v : values) {
        v += BASELINE;
    }

    const long values_size = values.size();

    for (auto pos = values.begin();; pos = values.begin()) {
        fmt::print("\n");
        print(values);
        for (; pos != values.end() && *pos < (BASELINE/1000); ++pos)
            ;
        if(pos == values.end()) {
            break;
        }

        auto rotate = [&values](int ammount) {
            if (ammount < 0) {
                fmt::print("negative rotate\n");
                ammount = std::abs(ammount);
                for (int i = 0; i < ammount; ++i) {
                    values.push_front(values.back());
                    values.pop_back();
                }
            } else if (ammount > 0) {
                fmt::print("positive rotate\n");
                for (int i = 0; i < ammount; ++i) {
                    values.push_back(values.front());
                    values.pop_front();
                }
            }
        };

        int val = (*pos - BASELINE) % values_size;

        const int initial_dist = std::distance(values.begin(), pos);

        rotate(initial_dist);
        fmt::print("after 1st rotate of {}\n", val);
        print(values);

        values.pop_front();

        fmt::print("after pop of {}\n", val);
        print(values);

        rotate(val);
        fmt::print("after 2nd rotate of {}\n", val);
        print(values);

        values.push_front(val);

        fmt::print("after push of {}\n", val);
        print(values);

        val = -val;
        rotate(val);

        fmt::print("after 3rd rotate of {}\n", val);
        print(values);

        if (val < 0 && initial_dist != 0) {
            rotate(-initial_dist);

            fmt::print("after 4rd rotate of {}\n", -initial_dist);
            print(values);
        }

        fmt::print("----------------------\n");
    }

    auto it = std::find(values.begin(), values.end(), 0);
    assert(it != values.end());
    int dist = std::distance(values.begin(), it);

    fmt::print("1000th = {}\n", values.at((dist+1000)%values_size));
    fmt::print("2000th = {}\n", values.at((dist+2000)%values_size));
    fmt::print("3000th = {}\n", values.at((dist+3000)%values_size));
    fmt::print(
        "1: {}\n",
        values.at((dist + 1000) % values_size) + values.at((dist + 2000) % values_size)
            + values.at((dist + 3000) % values_size));

    // -7160 INCORRECT
}


int main()
{
    Storage values;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        values.push_back(stoi(line));
    }

    part1(values);

    return 0;
}
