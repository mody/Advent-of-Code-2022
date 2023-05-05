#include <cassert>
#include <iostream>
#include <map>
#include <string>
#include <variant>

#include <fmt/core.h>

enum class Op
{
    None = 0,
    Mul,
    Div,
    Add,
    Sub
};

struct Node
{
    std::string left, right;
    Op op = Op::None;
    int64_t val = 0;
};

using Tree = std::map<std::string, Node>;


void part1(Tree tree)
{
    while(tree.at("root").op != Op::None) {
        for (auto& [key, node] : tree) {
            if (node.op == Op::None) {
                continue;
            }
            if (tree.at(node.left).op == Op::None && tree.at(node.right).op == Op::None) {
                int64_t a = tree.at(node.left).val;
                int64_t b = tree.at(node.right).val;
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
        if (std::isdigit(line.at(6))) {
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

    return 0;
}
