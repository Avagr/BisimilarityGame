#include "petrinet.h"
#include <utility>

Transition::Transition(std::string label, Multiset before, Multiset after)
    : label(std::move(label)), before(std::move(before)), after(std::move(after)) {
}
int PetriNet::GetPlaceNum() const {
    return place_num_;
}
