#include <iostream>
#include "prooftree.h"

ProofTree::ProofTree(Multiset first, Multiset second, PetriNet* net) : petri_net_(net) {
    root_ = std::make_unique<Node>(std::move(first), std::move(second), petri_net_->transitions,
                                   nullptr, nullptr, 0);
}

bool ProofTree::CheckBisimilarity() {
    return Expand(root_.get());
}

bool ProofTree::Expand(Node* node) {
    if (node->first == node->second) {
        return true;
    }
    while (true) {
        node->children.clear();
        for (auto&& trans : petri_net_->transitions) {
            auto rs_child = DeltaChild(trans.get(), &node->first, &node->second,
                                       &node->rs_used[trans.get()], 1);
            if (rs_child == nullptr) {
                return false;
            }
            auto sr_child = DeltaChild(trans.get(), &node->second, &node->first,
                                       &node->sr_used[trans.get()], -1);
            if (sr_child == nullptr) {
                return false;
            }
            node->AddChild(std::move(rs_child));
            node->AddChild(std::move(sr_child));
        }
        bool all_children = true;
        for (auto&& child : node->children) {
            all_children = all_children && Reduce(child.get());
        }
        if (all_children) {
            return true;
        }
    }
}

bool ProofTree::Reduce(Node* node) {
    size_t place_num = petri_net_->GetPlaceNum();
    bool reduced = true;
    while (reduced) {
        if (node->first == node->second) {
            return true;
        }
        Node* parent = node->parent;
        reduced = false;
        while (parent != nullptr) {
            Multiset intersect_r0(place_num), rem_s1(place_num), rem_s1_tick(place_num),
                rem_r1_tick(place_num);
            if (parent->PartialOrder(node, &intersect_r0, &rem_s1, &rem_s1_tick, &rem_r1_tick)) {
                node->AddChild(std::make_unique<Node>(
                    node->first,
                    Multiset::ReduceChild(intersect_r0, rem_s1_tick, rem_s1, rem_r1_tick),
                    petri_net_->transitions, nullptr, nullptr, 0));
                reduced = true;
                node = node->children.back().get();
                break;
            }
            parent = parent->parent;
        }
    }
    return Expand(node);
}

inline unique_ptr<ProofTree::Node> ProofTree::DeltaChild(const Transition* delta,
                                                         const Multiset* first,
                                                         const Multiset* second, int* counter,
                                                         int rs_order) {
    auto candidates = &(petri_net_->label_map[delta->label]);
    unique_ptr<Node> res = nullptr;
    for (; *counter < candidates->size(); (*counter)++) {
        auto gamma = (*candidates)[*counter];
        Multiset r_set = Multiset::WeakTransition(first, delta);
        Multiset s_set(petri_net_->GetPlaceNum());
        if (!Multiset::MirrorTransition(second, first, delta, gamma, &s_set)) {
            continue;
        }
        res = std::make_unique<Node>(r_set, s_set, petri_net_->transitions, delta, gamma, rs_order);
        break;
    }
    return res;
}

ProofTree::Node::Node(Multiset first, Multiset second,
                      const std::vector<unique_ptr<Transition>>& trans, const Transition* delta,
                      const Transition* gamma, int rs_order)
    : first(std::move(first)),
      second(std::move(second)),
      delta_used(delta),
      gamma_used(gamma),
      order_used(rs_order) {
    for (auto&& t : trans) {
        rs_used[t.get()] = 0;
        sr_used[t.get()] = 0;
    }
}

void ProofTree::Node::AddChild(unique_ptr<Node> child) {
    child->parent = this;
    children.push_back(std::move(child));
}

// (r, s), r0, s1, s1', r1'
inline bool ProofTree::Node::PartialOrder(ProofTree::Node* other, Multiset* other_intersect,
                                          Multiset* other_second_rem, Multiset* this_second_rem,
                                          Multiset* this_first_rem) const {
    if (first == second) {
        if (other->first == other->second) {
            return first.SubsetOf(other->first);
        }
        return false;
    }
    if (other->first == other->second) {
        return false;
    }
    size_t n = this->first.Length();
    Multiset other_first_rem(n);
    Multiset this_intersect =
        Multiset::SplitIntersection(first, second, this_first_rem, this_second_rem);
    *other_intersect = Multiset::SplitIntersection(other->first, other->second, &other_first_rem,
                                                   other_second_rem);
    return this_intersect.SubsetOf(*other_intersect) && this_first_rem->SubsetOf(other_first_rem) &&
           this_second_rem->SubsetOf(*other_second_rem);
}
