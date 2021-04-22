#pragma once

#include <memory>
#include <set>
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
         * @param other other node to check the ordering
         * @return true if the node is less or equals to the other, false if the node is bigger or
         * incomparable
         */
        bool PartialOrder(Node* other, Multiset* other_intersect, Multiset* other_second_rem,
                          Multiset* this_second_rem, Multiset* this_first_rem) const;

        Node* parent = nullptr;
        Multiset first, second;
        std::vector<unique_ptr<Node>> children;
        std::map<Transition*, int> rs_used, sr_used;
        const Transition* delta_used = nullptr;
        const Transition* gamma_used = nullptr;
        int order_used = 0;  // 1 - rs, 0 - none, -1 - sr
    };

    bool Expand(Node* node);

    bool Reduce(Node* node);

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
