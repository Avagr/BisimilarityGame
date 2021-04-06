#include "multiset.h"

Multiset::Multiset(int place_num) : arr_(place_num) {
}

Multiset Multiset::WeakTransition(const Multiset* init, const Transition* delta) {
    Multiset res(init->Length());
    for (int i = 0; i < res.Length(); ++i) {
        res.arr_[i] = std::max(init->arr_[i], delta->before.arr_[i]) - delta->before.arr_[i] +
                      delta->after.arr[i];
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
        res->arr_[i] = tmp + gamma->after.arr[i];
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

inline bool Multiset::SubsetOf(const Multiset& set) const {
    if (power_ > set.power_) {
        return false;
    }
    for (int i = 0; i < Length(); ++i) {
        if (arr_[i] > set.arr_[i]) {
            return false;
        }
    }
    return true;
}

Multiset Multiset::SplitIntersection(const Multiset& left, const Multiset& right,
                                     Multiset* left_rem, Multiset* right_rem) {
    Multiset intersect(left.Length());
    for (int i = 0; i < left.Length(); ++i) {
        intersect.arr_[i] = std::min(left.arr_[i], right.arr_[i]);
        intersect.power_ += intersect.arr_[i];
        left_rem->arr_[i] = left.arr_[i] - intersect.arr_[i];
        left_rem->power_ += left_rem->arr_[i];
        right_rem->arr_[i] = right.arr_[i] - intersect.arr_[i];
        right_rem->power_ += right_rem->arr_[i];
    }
    return intersect;
}

int Multiset::Length() const {
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
}
