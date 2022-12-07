#include <cassert>
#include <concepts>
#include <cstdint>
#include <deque>
#include <fmt/core.h>
#include <iostream>
#include <iterator>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <variant>
#include <vector>

struct Dir;
using DirPtr = std::shared_ptr<Dir>;

struct File
{
    std::string name {};
    std::uint64_t size {};
};

struct Dir
{
    Dir(std::string n)
        : name {std::move(n)}
    { }

    std::string name {};
    std::weak_ptr<Dir> parent;
    std::vector<DirPtr> children;
    std::vector<File> files;

    std::uint64_t size {};
};

std::uint64_t set_dir_size(DirPtr d)
{
    std::uint64_t size = 0;

    for (auto const& f : d->files) {
        size += f.size;
    }

    for (auto& sub : d->children) {
        size += set_dir_size(sub);
    }

    d->size = size;
    return size;
}

std::uint64_t count_sub_100000(DirPtr d)
{
    std::uint64_t size = 0;

    if (d->size < 100000) {
        size += d->size;
    }

    for (auto& sub : d->children) {
        size += count_sub_100000(sub);
    }

    return size;
}

void part1(DirPtr root)
{
    set_dir_size(root);
    fmt::print("1: {}\n", count_sub_100000(root));
}

void part2(DirPtr root)
{
    const std::uint64_t current_space = 70000000 - root->size;
    std::uint64_t smallest = root->size;

    std::deque<DirPtr> q;
    q.push_front(root);

    for (; !q.empty();) {
        DirPtr d = q.front();
        q.pop_front();

        if (d->size + current_space > 30000000) {
            smallest = std::min(smallest, d->size);
        }

        for (auto sub : d->children) {
            q.push_back(sub);
        }
    }

    fmt::print("2: {}\n", smallest);
}

int main()
{
    std::string line;

    DirPtr root = std::make_shared<Dir>("/");
    DirPtr current{};

    while (std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        if (line.at(0) == '$') {
            // command
            if (line.at(2) == 'c') {
                // cd
                const std::string_view dir_name(std::next(line.cbegin(), 5), line.cend());
                if (dir_name == "/") {
                    // cd /
                    current = root;
                } else if (dir_name == "..") {
                    // cd ..
                    current = current->parent.lock();
                } else {
                    // cd xxx
                    for (auto &sub : current->children) {
                        if (sub->name == dir_name) {
                            current = sub;
                            break;
                        }
                    }
                    assert(current->name == dir_name);
                }
            } else if (line.at(2) == 'l') {
                // ls
            } else {
                std::cerr << "unknow line: " << line << std::endl;
                assert(false);
            }
        } else if (line.at(0) == 'd') {
            // dir
            DirPtr sub_dir = std::make_shared<Dir>(line.substr(4));
            sub_dir->parent = current;
            current->children.push_back(sub_dir);
        } else {
            // file
            File f{};
            std::stringstream ss(line);
            ss >> f.size >> f.name;
            current->files.emplace_back(std::move(f));
        }
    }

    part1(root);
    part2(root);

    return 0;
}
