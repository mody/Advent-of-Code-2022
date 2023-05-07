#include <cassert>
#include <cmath>
#include <cstring>
#include <deque>
#include <iostream>
#include <limits>
#include <map>
#include <optional>
#include <sstream>
#include <string>
#include <unordered_map>
#include <variant>

#include <fmt/core.h>

using ValType = int64_t;

enum class Op
{
    None = 0,
    Mul,
    Div,
    Add,
    Sub,
    Eq
};

struct Node
{
    std::string left, right;
    Op op = Op::None;
    ValType val = 0;
};

using Tree = std::map<std::string, Node>;

[[maybe_unused]] void print(Tree const& tree) {
    fmt::print("Tree\n");
    for (auto const& [key, node] : tree) {
        fmt::print("{}: ", key);
        if (node.op == Op::None) {
            fmt::print("{}\n", node.val);
        } else {
            fmt::print("{} ", node.left);
            switch (node.op) {
            case Op::Mul: fmt::print("*"); break;
            case Op::Div: fmt::print("/"); break;
            case Op::Add: fmt::print("+"); break;
            case Op::Sub: fmt::print("-"); break;
            case Op::Eq:  fmt::print("="); break;
            default: assert(false);
            }
            fmt::print(" {}\n", node.right);
        }
    }
}

[[maybe_unused]] std::string print_recursive(Tree const& tree, std::string const& next)
{
    auto const& node = tree.at(next);
    auto const& lhs = tree.at(node.left);
    auto const& rhs = tree.at(node.right);

    std::stringstream ss;
    ss << "(";

    if (node.left == "humn") {
        ss << "humn";
    } else if (lhs.op == Op::None) {
        ss << lhs.val;
    } else {
        ss << print_recursive(tree, node.left);
    }

    switch (node.op) {
    case Op::Mul: ss << " * "; break;
    case Op::Div: ss << " / "; break;
    case Op::Add: ss << " + "; break;
    case Op::Sub: ss << " - "; break;
    case Op::Eq:  ss << " = "; break;
    default: assert(false);
    }

    if (node.right == "humn") {
        ss << "humn";
    } else if (rhs.op == Op::None) {
        ss << rhs.val;
    } else {
        ss << print_recursive(tree, node.right);
    }
    ss << ")";

    return ss.str();
}

[[maybe_unused]] void print2(Tree const& tree) {
    fmt::print("Tree\n{}\n",
            print_recursive(tree, "root"));
}

void part1(Tree tree)
{
    while(tree.at("root").op != Op::None) {
        for (auto& [key, node] : tree) {
            if (node.op == Op::None) {
                continue;
            }
            if (tree.at(node.left).op == Op::None && tree.at(node.right).op == Op::None) {
                ValType a = tree.at(node.left).val;
                ValType b = tree.at(node.right).val;
                switch (node.op) {
                case Op::Mul: node.val = a * b; break;
                case Op::Div: node.val = a / b; break;
                case Op::Add: node.val = a + b; break;
                case Op::Sub: node.val = a - b; break;
                default: assert(false);
                }
                node.op = Op::None;
            }
        }
    }
    fmt::print("1: {}\n", tree.at("root").val);
}

Tree optimize(Tree tree)
{
    for(bool change = true; change == true;) {
        change = false;
        for (auto& [key, node] : tree) {
            if (node.op == Op::Eq) {
                continue;
            }
            if (key == "humn") {
                continue;
            }
            if (node.op == Op::None) {
                bool del = true;
                for (auto& [_, node2] : tree) {
                    if (node2.left == key || node2.right == key) {
                        del = false;
                        break;
                    }
                }
                if (del) {
                    change = true;
                    tree.erase(key);
                    break;
                }
                continue;
            }
            Node const& lnode = tree.at(node.left);
            Node const& rnode = tree.at(node.right);
            if (lnode.op == Op::None && rnode.op == Op::None) {
                ValType a = lnode.val;
                ValType b = rnode.val;
                switch (node.op) {
                case Op::Mul: node.val = a * b; break;
                case Op::Div: node.val = a / b; break;
                case Op::Add: node.val = a + b; break;
                case Op::Sub: node.val = a - b; break;
                default: assert(false);
                }
                node.op = Op::None;
                node.left.clear();
                node.right.clear();

                change = true;
                break;
            }
        }
    }
    return tree;
}

std::optional<ValType> reverse(Tree const& tree)
{
    std::string next = "root";
    ValType val = 0;

    for(;;) {
        assert(!next.empty());
        if (next == "humn") {
            return val;
        }
        auto const& node = tree.at(next);
        auto const& lhs = tree.at(node.left);
        auto const& rhs = tree.at(node.right);

        ValType oth = 0;

        if (lhs.op == Op::None && rhs.op == Op::None) {
            assert(false);
        } else if (lhs.op != Op::None && rhs.op != Op::None) {
            assert(false);
        } else if (lhs.op == Op::None && rhs.op != Op::None) {
            // left is a value
            oth = lhs.val;
            next = node.right;
            switch (node.op) {
            case Op::Mul: val = val / oth; break;
            case Op::Div: val = val * oth; break;
            case Op::Add: val = val - oth; break;
            case Op::Sub: val = oth - val; break;
            case Op::Eq: val = oth; break;
            default: assert(false);
            }
        } else if (lhs.op != Op::None && rhs.op == Op::None) {
            // right is a value
            oth = rhs.val;
            next = node.left;
            switch (node.op) {
            case Op::Mul: val = val / oth; break;
            case Op::Div: val = val * oth; break;
            case Op::Add: val = val - oth; break;
            case Op::Sub: val = val + oth; break;
            case Op::Eq: val = oth; break;
            default: assert(false);
            }
        } else {
            assert(false);
        }
    }

    return std::nullopt;
}

void part2(Tree tree)
{
    std::unordered_map<std::string, std::map<ValType, ValType>> counts;

    tree.at("root").op = Op::Eq;

    auto process = [&tree, &counts](ValType from, ValType to, ValType step = 1) -> std::optional<ValType> {
        for (ValType i = from; i < to; i += step) {
            Tree t2 = tree;
            {
                Node& humn = t2.at("humn");
                humn.left.clear();
                humn.right.clear();
                humn.op = Op::None;
                humn.val = i;
            }
            Node const& root = t2.at("root");
            while (root.op != Op::None) {
                for (auto& [key, node] : t2) {
                    if (node.op == Op::None) {
                        continue;
                    }
                    if (t2.at(node.left).op == Op::None && t2.at(node.right).op == Op::None) {
                        ValType a = t2.at(node.left).val;
                        ValType b = t2.at(node.right).val;
                        switch (node.op) {
                        case Op::Mul: node.val = a * b; break;
                        case Op::Div: node.val = a / b; break;
                        case Op::Add: node.val = a + b; break;
                        case Op::Sub: node.val = a - b; break;
                        case Op::Eq: node.val = (a == b); break;
                        default: assert(false);
                        }
                        node.left.clear();
                        node.right.clear();
                        if (node.op == Op::Eq) {
                            node.op = Op::None;
                            break;
                        }
                        node.op = Op::None;
                        counts[key][node.val] += 1;
                    }
                }
            }
            if (root.val) {
                return {i};
            }
        }
        return std::nullopt;
    };

    auto result = process(0, 300);

    for (auto const& [key, cs] : counts)
    {
        if (cs.size() == 1) {
            const auto val = cs.begin()->first;
            tree[key] = Node{.val = val};
        }
    }

    // fake "humn"
    {
        Node& humn = tree.at("humn");
        humn.left = "humn";
        humn.right = "humn";
        humn.op = Op::Eq;
    }

    tree = optimize(std::move(tree));

    result = reverse(tree);

    // try found humn
    if (result) {
        Node& humn = tree.at("humn");
        humn.left.clear();
        humn.right.clear();
        humn.op = Op::None;
        humn.val = result.value();

        result = process(result.value(), result.value() + 1);

        fmt::print("\n2: {}\n", result.value_or(-1));
    } else {
        fmt::print("2: NOT FOUND\n");
    }
}

int main()
{
    Tree tree;

    std::string line;
    while(std::getline(std::cin, line)) {
        if (line.empty()) {
            break;
        }
        std::string key = line.substr(0, 4);
        Node n;
        if (std::isdigit(line.at(6)) || line.at(6) == '-') {
            n.val = std::stoll(line.substr(6));
        } else {
            n.left = line.substr(6, 4);
            n.right = line.substr(13, 4);
            switch(line.at(11)) {
            case '+': n.op = Op::Add; break;
            case '-': n.op = Op::Sub; break;
            case '*': n.op = Op::Mul; break;
            case '/': n.op = Op::Div; break;
            default: assert(false);
            }
        }
        tree.insert({std::move(key), std::move(n)});
    }

    part1(tree);
    part2(tree);

    return 0;
}
