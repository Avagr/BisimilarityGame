// todo: delete
#pragma once
#include <iostream>
#include "src/petrinets/prooftree.h"

using std::string;
using std::vector;

int main() {
    Multiset first({1, 0}), second({0, 1});
    auto x = std::make_tuple("1", "a", std::vector<int>({1, 0}), std::vector<int>({0, 1}));
    auto y = std::make_tuple("2", "a", std::vector<int>({0, 1}), std::vector<int>({1, 0}));
    PetriNet net({x, y});
    ProofTree tree(first, second, &net);
    std::cout << tree.CheckBisimilarity();
}