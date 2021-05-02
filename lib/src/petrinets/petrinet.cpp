#include "petrinet.h"
#include <utility>

Multiset::Multiset(size_t place_num) : arr_(place_num) {
}

Multiset Multiset::WeakTransition(const Multiset* init, const Transition* delta) {
    Multiset res(init->Length());
    for (int i = 0; i < res.Length(); ++i) {
        res.arr_[i] = std::max(init->arr_[i], delta->before.arr_[i]) - delta->before.arr_[i] +
                      delta->after.arr_[i];
        res.power_ += res.arr_[i];
    }
    return res;
}

bool Multiset::MirrorTransition(const Multiset* init, const Multiset* prev, const Transition* delta,
                                const Transition* gamma, Multiset* res) {
    for (int i = 0; i < res->Length(); ++i) {
        int tmp = std::max(delta->before.arr_[i] - prev->arr_[i], 0) + init->arr_[i] -
                  gamma->before.arr_[i];
        if (tmp < 0) {
            return false;
        }
        res->arr_[i] = tmp + gamma->after.arr_[i];
        res->power_ += res->arr_[i];
    }
    return true;
}

bool Multiset::operator==(const Multiset& other) const {
    if (power_ != other.power_) {
        return false;
    }
    for (int i = 0; i < Length(); ++i) {
        if (arr_[i] != other.arr_[i]) {
            return false;
        }
    }
    return true;
}

Multiset Multiset::SplitIntersection(const Multiset* left, const Multiset* right,
                                     Multiset* left_rem, Multiset* right_rem) {
    Multiset intersect(left->Length());
    for (int i = 0; i < left->Length(); ++i) {
        intersect.arr_[i] = std::min(left->arr_[i], right->arr_[i]);
        intersect.power_ += intersect.arr_[i];
        left_rem->arr_[i] = left->arr_[i] - intersect.arr_[i];
        left_rem->power_ += left_rem->arr_[i];
        right_rem->arr_[i] = right->arr_[i] - intersect.arr_[i];
        right_rem->power_ += right_rem->arr_[i];
    }
    return intersect;
}

size_t Multiset::Length() const {
    return arr_.size();
}

Multiset Multiset::ReduceChild(const Multiset& intersect, const Multiset& other_second_rem,
                               const Multiset& second_rem, const Multiset& first_rem) {
    Multiset res(intersect.Length());
    for (int i = 0; i < intersect.Length(); ++i) {
        res.arr_[i] = intersect.arr_[i] +
                      std::max(0, other_second_rem.arr_[i] - second_rem.arr_[i]) +
                      first_rem.arr_[i];
        res.power_ += res.arr_[i];
    }
    return res;
}
Multiset::Multiset(std::vector<int> arr) : arr_(std::move(arr)) {
    power_ = 0;
    for (int x : arr_) {
        power_ += x;
    }
}
std::string Multiset::ToString() const {
    std::stringstream builder;
    builder << '[';
    for (int i = 0; i < arr_.size(); ++i) {
        builder << arr_[i];
        if (i != arr_.size() - 1) {
            builder << ", ";
        }
    }
    builder << ']';
    return builder.str();
}
Multiset Multiset::Difference(const Multiset& other) const {
    auto res = Multiset(arr_.size());
    for (int i = 0; i < arr_.size(); ++i) {
        res.arr_[i] = arr_[i] - other.arr_[i];
    }
    return res;
}

Transition::Transition(std::string id, std::string label, Multiset before, Multiset after)
    : label(std::move(label)),
      id(std::move(id)),
      before(std::move(before)),
      after(std::move(after)) {
}
std::string Transition::ToString() const {
    std::stringstream builder;
    builder << id << ", " << label << ", " << after.Difference(before).ToString();
    return builder.str();
}

PetriNet::PetriNet(
    const std::vector<std::tuple<std::string, std::string, std::vector<int>, std::vector<int>>>&
        trans_list) {
    for (auto&& trans : trans_list) {
        Multiset before(std::get<2>(trans));
        Multiset after(std::get<3>(trans));
        place_num_ = before.Length();
        transitions.push_back(
            std::make_unique<Transition>(std::get<0>(trans), std::get<1>(trans), before, after));
    }
    for (auto&& trans : transitions) {
        label_map[trans->label].push_back(trans.get());
    }
}

size_t PetriNet::GetPlaceNum() const {
    return place_num_;
}
