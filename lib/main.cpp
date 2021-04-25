// todo: delete file
#include <iostream>
#include "src/petrinets/prooftree.h"

using std::string;
using std::vector;

int main() {
    //    Multiset first({0, 1}), second({3, 0});
    Multiset first({2, 0}), second({5, 2});
    auto x = std::make_tuple("1", "a", std::vector<int>({1, 0}), std::vector<int>({0, 1}));
    auto y = std::make_tuple("2", "a", std::vector<int>({0, 1}), std::vector<int>({1, 0}));
    PetriNet net({x, y});
    ProofTree tree(first, second, &net);
    std::cout << tree.CheckBisimilarity();
    //    tree.PrintTree("here.txt");
    tree.PrintTree("here_normal.txt");
}