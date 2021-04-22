#pragma once
#include <iostream>
#include "src/petrinets/prooftree.h"

int main() {
    Multiset first(2), second(2);
    first.arr_ = {1, 0};
    second.arr_ = {0, 1};
    Multiset one_before(2), two_before(2), one_after(2), two_after(2);
    one_before.arr_ = {1, 0};
    one_after.arr_ = {0, 1};
    two_before.arr_ = {0, 1};
    two_after.arr_ = {1, 0};
    PetriNet net;
    std::unique_ptr<Transition> trans1 = std::make_unique<Transition>("a", one_before, one_after);
    std::unique_ptr<Transition> trans2 = std::make_unique<Transition>("a", two_before, two_after);
    net.transitions.push_back(std::move(trans1));
    net.transitions.push_back(std::move(trans2));
    net.label_map["a"] =
        std::vector<Transition*>{net.transitions[0].get(), net.transitions[1].get()};
    net.place_num_ = 2;
    ProofTree tree(first, second, &net);
    std::cout << tree.CheckBisimilarity();
};