#include <algorithm>
#include <cassert>
#include <deque>
#include <iostream>
#include <iterator>
#include <list>
#include <string>
#include <vector>

#include <fmt/core.h>

struct Item
{
    int64_t value = 0;
    int posititon = 0;
};

using Storage = std::list<Item>;

[[maybe_unused]] void print(Storage const& values) noexcept
{
    for (auto const& v : values) {
        fmt::print("{}({})\n", v.value, v.posititon);
    }
}

Storage process(Storage values, int repeat = 1)
{
    const long values_size = values.size();

    for (int j = 0; j < repeat; ++j) {
        for (int i = 0; i < values_size; ++i) {
            // fmt::print("\n");
            // print(values);
            auto pos = std::find_if(
                values.begin(), values.end(), [&i](Item const& item) { return item.posititon == i; });
            if (pos == values.end()) {
                break;
            }

            const auto val = *pos;
            const int current_pos = std::distance(values.begin(), pos);
            int dest = (val.value + current_pos) % (values_size - 1);
            if (dest <= 0) {
                dest += values_size - 1;
            }

            // fmt::print("Moving {} from {} to {}\n", val, current_pos, dest);
            pos = values.erase(pos);
            values.insert(std::next(values.begin(), dest), val);
            // print(values);
        }
    }

    return values;
}

void print_result(Storage const& values)
{
    const long values_size = values.size();

    std::vector<Item> v2(values.begin(), values.end());

    auto it = std::find_if(v2.begin(), v2.end(), [](Item const& item) { return item.value == 0; });
    assert(it != v2.end());
    int dist = std::distance(v2.begin(), it);

    fmt::print("1000th = {}\n", v2.at((dist+1000)%values_size).value);
    fmt::print("2000th = {}\n", v2.at((dist+2000)%values_size).value);
    fmt::print("3000th = {}\n", v2.at((dist+3000)%values_size).value);
    fmt::print(
        "{}\n",
        v2.at((dist + 1000) % values_size).value + v2.at((dist + 2000) % values_size).value
            + v2.at((dist + 3000) % values_size).value);
}

void part1(Storage values)
{
    print_result(process(std::move(values)));
    // 16533 CORRECT
}

void part2(Storage values)
{
    for (auto& v : values) {
        v.value *= 811589153;
    }

    print_result(process(std::move(values), 10));

    // 4789999181006 CORRECT
}

int main()
{
    Storage values;

    int i = 0;
    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        values.push_back({stoi(line), i++});
    }

    part1(values);
    part2(values);

    return 0;
}
