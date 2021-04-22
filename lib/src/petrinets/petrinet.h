#pragma once
#include <vector>
#include <string>
#include <map>
#include <memory>

class Transition;

/**
 * Multiset of markings on the petri net
 * @tparam Num Number type to count for smaller net optimization
 */
class Multiset {
public:
    explicit Multiset(size_t place_num);

    // Equality Const
    bool operator==(const Multiset& other) const;

    static Multiset ReduceChild(const Multiset& intersect, const Multiset& other_second_rem,
                                const Multiset& second_rem, const Multiset& first_rem);

    // Is subset Const
    [[nodiscard]] inline bool SubsetOf(const Multiset& set) const {
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

    /**
     * Constructs the new state using the weak transition property
     * @param init initial vector of the transition
     * @param delta transition to execute
     * @return resulting multiset
     */
    static Multiset WeakTransition(const Multiset* init, const Transition* delta);

    /**
     * Mirrors a weak transition done us
     * @param init initial state
     * @param prev state on which delta_used fired
     * @param delta transition that we are mirroring
     * @param gamma transition to execute
     * @param res resulting multiset
     * @return true if the transition was successful, false otherwise
     */
    static bool MirrorTransition(const Multiset* init, const Multiset* prev,
                                 const Transition* delta, const Transition* gamma, Multiset* res);

    static Multiset SplitIntersection(const Multiset& left, const Multiset& right,
                                      Multiset* left_rem, Multiset* right_rem);

    [[nodiscard]] size_t Length() const;

    std::vector<int> arr_;  // todo: switch back

private:
    int64_t power_ = 0;
};

/**
 * A single transition in a Petri Net
 */
struct Transition {
    Transition(std::string label, Multiset before, Multiset after);

    std::string label;
    Multiset before, after;
};

class PetriNet {
public:
    std::map<std::string, std::vector<Transition*>> label_map;
    std::vector<std::unique_ptr<Transition>> transitions;

    [[nodiscard]] size_t GetPlaceNum() const;
    size_t place_num_;  // todo

private:
};
