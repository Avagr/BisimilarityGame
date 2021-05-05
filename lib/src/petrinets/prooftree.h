#pragma once

#include <memory>
#include <unordered_set>
#include <fstream>
#include <stack>
#include <unordered_map>
#include "petrinet.h"

using std::unique_ptr;

/**
 * Proof tree we are constructing
 */
class ProofTree {
public:
    ProofTree(Multiset first, Multiset second, PetriNet* net, bool record_basis);

    /**
     * Builds the proof tree
     * @return true if the tree is correct, false otherwise
     */
    bool CheckBisimilarity();

    void PrintTree(std::basic_ofstream<char> output);

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
         * @return true if this node is greater than the other, false otherwise
         */
        bool GreaterThan(Multiset* other_first, Multiset* other_second, Multiset* this_intersect,
                         Multiset* other_intersect, Multiset* this_first_rem,
                         Multiset* this_second_rem, Multiset* other_first_rem,
                         Multiset* other_second_rem) const;

        Node* parent = nullptr;
        Multiset first, second;
        std::vector<unique_ptr<Node>> children;
        std::unordered_map<Transition*, int> rs_used, sr_used;
        const Transition* delta_used = nullptr;
        const Transition* gamma_used = nullptr;
        int order_used = 0;  // 1 - rs, 0 - none, -1 - sr
        int id = -1;
        Node* reduced_parent = nullptr;
    };

    /**
     * Expand step of the algorithm
     * @param node node to perform the step on
     * @param depth depth of recursion
     * @return bisimilarity for the resulting subtree
     */
    bool Expand(Node* node, int depth);

    /**
     * Reduce step of the algorithm
     * @param node node to perform the step on
     * @param depth depth of recursion
     * @return bisimilarity for the resulting subtree
     */
    bool Reduce(Node* node, int depth);

    /**
     * Traverses the tree using Depth-First Search, recording nodes, edges and a basis, if required
     * @param nodes nodes to collect
     * @param edges edges to collect
     */
    void TreeTraversal(std::vector<Node*>* nodes, std::vector<std::pair<Node*, Node*>>* edges);

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
    bool record_basis_ = false;
    std::unordered_set<Node*> basis_;
    PetriNet* petri_net_;
    bool success = false;
};
