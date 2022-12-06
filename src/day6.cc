#include <array>
#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <string>

template<unsigned int SEGMENT>
void part(std::string const& input)
{
    std::array<unsigned int, 'z' - 'a' + 1> freq_data {};

    auto freq_unique = [&freq_data]() -> bool {
        for (auto const& f : freq_data) {
            if (f > 1) {
                return false;
            }
        }
        return true;
    };

    auto freq_index = [](unsigned char c) -> unsigned int {
        return c - 'a';
    };

    for (unsigned int i = 0; i < SEGMENT; ++i) {
        freq_data.at(freq_index(input.at(i))) += 1;
    }

    for (size_t from = 0; (from + SEGMENT) < input.size(); ++from) {
        if (freq_unique()) {
            fmt::print("1: {}\n", from + SEGMENT);
            break;
        }
        freq_data.at(freq_index(input.at(from))) -= 1;
        freq_data.at(freq_index(input.at(from + SEGMENT))) += 1;
    }
}

int main()
{
    std::string line;

    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        part<4>(line);
        part<14>(line);
    }

    return 0;
}
