#pragma once

#include <memory>
#include <set>
#include <fstream>
#include <stack>
#include <queue>
#include "petrinet.h"

using std::unique_ptr;

/**
 * Proof tree we are constructing
 */
class ProofTree {
public:
    ProofTree(Multiset first, Multiset second, PetriNet* net);

    /**
     * Builds the proof tree
     * @return true if the tree is correct, false otherwise
     */
    bool CheckBisimilarity();

    void PrintTree(const std::string& path) const {
        std::ofstream out{path};
        std::stack<std::pair<Node*, int>> pipe;
        //        std::queue<std::pair<Node*, int>> pipe;
        pipe.emplace(root_.get(), 0);
        while (!pipe.empty()) {
            auto x = pipe.top();
            //            auto x = pipe.front();
            pipe.pop();
            for (int i = 0; i < x.second; ++i) {
                out << '\t';
            }
            out << x.first->first.ToString() << ' ' << x.first->second.ToString() << '\n';
            for (auto&& child : x.first->children) {
                pipe.emplace(child.get(), x.second + 1);
            }
        }
        out.close();
    }

private:
    /**
     * Node of the proof tree
     */
    struct Node {
        Node(Multiset first, Multiset second, const std::vector<unique_ptr<Transition>>& trans,
             const Transition* delta, const Transition* gamma, int rs_order);

        /**
         * Adds a child to the node
         * @param child child to add
         */
        void AddChild(unique_ptr<Node> child);

        /**
         * Computes partial ordering for REDUCE
         * @return true if the node is greater than the other, false if the node is lesser or
         * incomparable
         */
        bool PartialOrder(Multiset* other_first, Multiset* other_second, Multiset* this_intersect,
                          Multiset* other_intersect, Multiset* this_first_rem,
                          Multiset* this_second_rem, Multiset* other_first_rem,
                          Multiset* other_second_rem) const;

        Node* parent = nullptr;
        Multiset first, second;
        std::vector<unique_ptr<Node>> children;
        std::map<Transition*, int> rs_used, sr_used;
        const Transition* delta_used = nullptr;
        const Transition* gamma_used = nullptr;
        int order_used = 0;  // 1 - rs, 0 - none, -1 - sr
    };

    bool Expand(Node* node, int depth);

    bool Reduce(Node* node, int depth);

    /**
     * Finds a random delta0child of node (first, second)
     * @param delta transition
     * @param first left state of the node
     * @param second right state of the node
     * @return nullptr if the set of children is empty, the child node otherwise
     */
    unique_ptr<Node> DeltaChild(const Transition* delta, const Multiset* first,
                                const Multiset* second, int* counter, int rs_order);

    unique_ptr<Node> root_;

    PetriNet* petri_net_;
};
