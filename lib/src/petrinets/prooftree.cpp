#include <iostream>
#include "prooftree.h"

ProofTree::ProofTree(Multiset first, Multiset second, PetriNet* net, bool record_basis)
    : petri_net_(net), record_basis_(record_basis) {
    root_ = std::make_unique<Node>(std::move(first), std::move(second), petri_net_->transitions,
                                   nullptr, nullptr, 0);
}

bool ProofTree::CheckBisimilarity() {
    success = Expand(root_.get(), 0);
    return success;
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
    Multiset intersect_r0(place_num), intersect_r0_tick(place_num), rem_r1(place_num),
        rem_r1_tick(place_num), rem_s1(place_num), rem_s1_tick(place_num);
    bool reduced = true;
    while (reduced) {
        if (node->first == node->second) {
            return true;
        }
        Node* parent = node->parent;
        reduced = false;
        while (parent != nullptr) {
            intersect_r0.Reset(), intersect_r0_tick.Reset(), rem_r1.Reset(), rem_r1_tick.Reset(),
                rem_s1.Reset(), rem_s1_tick.Reset();
            bool res_rs =  // r', s', r0, r0', r1, s1, r1', s1'
                node->GreaterThan(&parent->first, &parent->second, &intersect_r0,
                                  &intersect_r0_tick, &rem_r1, &rem_s1, &rem_r1_tick, &rem_s1_tick);
            bool res_sr;
            if (!res_rs) {
                intersect_r0.Reset(), intersect_r0_tick.Reset(), rem_r1.Reset(),
                    rem_r1_tick.Reset(), rem_s1.Reset(), rem_s1_tick.Reset();
                res_sr = node->GreaterThan(&parent->second, &parent->first, &intersect_r0,
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
                node->reduced_parent = parent;
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

void ProofTree::PrintTree(std::basic_ofstream<char> output) {
    std::vector<Node*> nodes;
    std::vector<std::pair<Node*, Node*>> edges;
    TreeTraversal(&nodes, &edges);
    output
        << R""""(<?xml version="1.0" encoding="UTF-8"?><graphml xmlns="http://graphml.graphdrawing.org/xmlns"><graph id="GameTree" edgedefault="directed">)"""";
    for (int i = 0; i < nodes.size(); ++i) {
        Node* node = nodes[i];
        output << "<node id=\"" << i << "\" first=\"" << node->first.ToString() << "\" second=\""
               << node->second.ToString() << "\"";
        if (node->order_used != 0) {
            output << " delta=\"" << node->delta_used->ToString() << "\" gamma=\""
                   << node->gamma_used->ToString() << "\" order=\""
                   << (node->order_used > 0 ? "direct" : "reverse") << "\"";
        } else if (node->reduced_parent != nullptr) {
            output << " reduced=\"" << node->reduced_parent->id << "\"";
        }
        if (node->children.empty()) {
            output << " terminal=\"" << (node->first == node->second ? "SUCCESS" : "FAIL") << "\"";
        }
        output << "/>";
        node->id = i;
    }
    for (auto&& edge : edges) {
        output << "<edge source=\"" << edge.first->id << "\" target=\"" << edge.second->id
               << "\"/>";
    }
    output << "</graph>";
    if (success && record_basis_) {
        output << "<basis>";
        for (auto&& node : basis_) {
            if (!(node->first == node->second)) {
                output << "<pair first=\"" << node->first.ToString() << "\" second=\""
                       << node->second.ToString() << "\"/>";
            }
        }
        output << "</basis>";
    }
    output << "</graphml>";
}

void ProofTree::TreeTraversal(std::vector<Node*>* nodes,
                              std::vector<std::pair<Node*, Node*>>* edges) {
    std::stack<Node*> stack;
    std::unordered_set<Node*> to_remove;
    size_t n = petri_net_->GetPlaceNum();
    Multiset m1(n), m2(n), m3(n), m4(n), m5(n), m6(n);
    stack.emplace(root_.get());
    while (!stack.empty()) {
        auto current = stack.top();
        nodes->push_back(current);
        if (record_basis_) {
            to_remove.clear();
            bool insert = true;
            for (auto&& el : basis_) {
                m1.Reset(), m2.Reset(), m3.Reset(), m4.Reset(), m5.Reset(), m6.Reset();
                bool direct = el->GreaterThan(&current->first, &current->second, &m1, &m2, &m3, &m4,
                                              &m5, &m6);
                bool reverse;
                if (!direct) {
                    m1.Reset(), m2.Reset(), m3.Reset(), m4.Reset(), m5.Reset(), m6.Reset();
                    reverse = el->GreaterThan(&current->second, &current->first, &m1, &m2, &m3, &m4,
                                              &m5, &m6);
                }
                if (direct || reverse) {
                    to_remove.insert(el);
                } else {
                    m1.Reset(), m2.Reset(), m3.Reset(), m4.Reset(), m5.Reset(), m6.Reset();
                    direct =
                        current->GreaterThan(&el->first, &el->second, &m1, &m2, &m3, &m4, &m5, &m6);
                    if (direct) {
                        insert = false;
                    }
                }
            }
            for (auto&& el : to_remove) {
                basis_.erase(el);
            }
            if (insert) {
                basis_.insert(current);
            }
        }
        stack.pop();
        for (auto&& child : current->children) {
            stack.emplace(child.get());
            edges->emplace_back(current, child.get());
        }
    }
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
bool ProofTree::Node::GreaterThan(Multiset* other_first, Multiset* other_second,
                                  Multiset* this_intersect, Multiset* other_intersect,
                                  Multiset* this_first_rem, Multiset* this_second_rem,
                                  Multiset* other_first_rem, Multiset* other_second_rem) const {
    if (first == second) {
        if (*other_first == *other_second) {
            return other_first->SubsetOf(first);
        }
        return false;
    }
    if (*other_first == *other_second) {
        return false;
    }
    *this_intersect = Multiset::SplitIntersection(&first, &second, this_first_rem, this_second_rem);
    *other_intersect =
        Multiset::SplitIntersection(other_first, other_second, other_first_rem, other_second_rem);
    return other_intersect->SubsetOf(*this_intersect) &&
           other_first_rem->SubsetOf(*this_first_rem) &&
           other_second_rem->SubsetOf(*this_second_rem);
}
