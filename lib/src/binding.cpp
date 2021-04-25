#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "petrinets/petrinet.h"
#include "petrinets/prooftree.h"

namespace py = pybind11;

bool CheckBisimilarity(
    std::vector<int> resource_one, std::vector<int> resource_two,
    std::vector<std::tuple<std::string, std::string, std::vector<int>, std::vector<int>>>
        transitions) {
    Multiset first(std::move(resource_one)), second(std::move(resource_two));
    PetriNet net(transitions);
    ProofTree tree(first, second, &net);
    return tree.CheckBisimilarity();
}

PYBIND11_MODULE(_core, module) {
    module.doc() = "Binding for the main bisimilarity checking function";
    module.def("check_bisimilarity", &CheckBisimilarity,
               "A function that checks the bisimilarity of two resources on a given Petri net");
}