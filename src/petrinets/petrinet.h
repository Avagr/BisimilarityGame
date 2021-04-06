#pragma once
#include <string>
#include "multiset.h"

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
    std::map<std::string, std::vector<Transition*>*> label_map;
    std::vector<std::unique_ptr<Transition>> transitions;

    [[nodiscard]] int GetPlaceNum() const;

private:
    int place_num_;
};
