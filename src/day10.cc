#include <cassert>
#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

struct Op
{
    std::string op {};
    int val {};
};

using Ops = std::vector<Op>;

class CPU
{
    Ops ops;

    Ops::iterator ip {};
    std::int64_t x = 1;
    unsigned op_ticks = 0;
    std::int64_t ip_ticks = 0;

public:
    CPU(Ops o)
        : ops {std::move(o)}
    { }

    std::int64_t get_x() const noexcept { return x; }
    std::int64_t get_ticks() const noexcept { return ip_ticks; }

    void init() noexcept
    {
        ip = ops.begin();
        x = 1;
        op_ticks = 0;
        ip_ticks = 0;
    }

    template<typename CallBack>
    bool tick(CallBack&& cb)
    {
        if (op_ticks == 0) {
            if (ip->op == "noop") {
                op_ticks = 1;
            } else if (ip->op == "addx") {
                op_ticks = 2;
            }
        }

        --op_ticks;

        cb(this);

        if (op_ticks == 0) {
            if (ip->op == "addx") {
                x += ip->val;
            }
            ++ip;
        }

        ++ip_ticks;

        if (ip == ops.end()) {
            return false;
        }

        return true;
    }
};


void part1(CPU cpu)
{
    std::int64_t ticks = 1, signal = 0;
    cpu.init();

    auto cb = [&ticks, &signal](CPU* cpu) {
        if (((ticks - 20) % 40) == 0) {
            signal += cpu->get_x() * ticks;
            // fmt::print("{}\t - signal: {}\n", ticks, cpu->get_x() * ticks);
        }
        ++ticks;
    };

    for (bool stop = false; !stop;) {
        stop = !cpu.tick(cb);
    }

    fmt::print("1: {}\n", signal);
}


int main()
{
    Ops ops;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        Op op {};
        std::stringstream ss(line);
        ss >> op.op >> op.val;
        ops.push_back(std::move(op));
    }

    CPU cpu(ops);

    part1(cpu);

    return 0;
}
