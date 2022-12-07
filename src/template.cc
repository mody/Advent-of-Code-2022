#include <cassert>
#include <fmt/core.h>
#include <iostream>
#include <string>

int main()
{
    std::string line;

    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
    }

    return 0;
}
