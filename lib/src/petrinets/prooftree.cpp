#include <iostream>
#include "prooftree.h"

ProofTree::ProofTree(Multiset first, Multiset second, PetriNet* net) : petri_net_(net) {
    root_ = std::make_unique<Node>(std::move(first), std::move(second), petri_net_->transitions,
                                   nullptr, nullptr, 0);
}

bool ProofTree::CheckBisimilarity() {
    return Expand(root_.get(), 0);
}

bool ProofTree::Expand(Node* node, int depth) {
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
            all_children = all_children && Reduce(child.get(), depth + 1);
        }
        if (all_children) {
            return true;
        }
    }
}

bool ProofTree::Reduce(Node* node, int depth) {
    size_t place_num = petri_net_->GetPlaceNum();
    bool reduced = true;
    while (reduced) {
        if (node->first == node->second) {
            return true;
        }
        Node* parent = node->parent;
        reduced = false;
        while (parent != nullptr) {
            Multiset intersect_r0(place_num), intersect_r0_tick(place_num), rem_r1(place_num),
                rem_r1_tick(place_num), rem_s1(place_num), rem_s1_tick(place_num);
            // r', s', r0, r0', r1, s1, r1', s1'
            bool res_rs = node->PartialOrder(&parent->first, &parent->second, &intersect_r0,
                                             &intersect_r0_tick, &rem_r1, &rem_s1, &rem_r1_tick,
                                             &rem_s1_tick);
            bool res_sr;
            if (!res_rs) {
                intersect_r0 = Multiset(place_num), intersect_r0_tick = Multiset(place_num),
                rem_r1 = Multiset(place_num), rem_r1_tick = Multiset(place_num),
                rem_s1 = Multiset(place_num), rem_s1_tick = Multiset(place_num);
                res_sr = node->PartialOrder(&parent->second, &parent->first, &intersect_r0,
                                            &intersect_r0_tick, &rem_r1, &rem_s1, &rem_r1_tick,
                                            &rem_s1_tick);
            }
            if (res_rs || res_sr) {
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
    return Expand(node, depth + 1);
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

// r', s', r0, r0', r1, s1, r1', s1'
bool ProofTree::Node::PartialOrder(Multiset* other_first, Multiset* other_second,
                                   Multiset* this_intersect, Multiset* other_intersect,
                                   Multiset* this_first_rem, Multiset* this_second_rem,
                                   Multiset* other_first_rem, Multiset* other_second_rem) const {
    if (first == second) {
        if (other_first == other_second) {
            return other_first->SubsetOf(first);
        }
        return false;
    }
    if (other_first == other_second) {
        return false;
    }
    *this_intersect = Multiset::SplitIntersection(&first, &second, this_first_rem, this_second_rem);
    *other_intersect =
        Multiset::SplitIntersection(other_first, other_second, other_first_rem, other_second_rem);
    return other_intersect->SubsetOf(*this_intersect) &&
           other_first_rem->SubsetOf(*this_first_rem) &&
           other_second_rem->SubsetOf(*this_second_rem);
}
