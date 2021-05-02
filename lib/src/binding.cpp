#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "petrinets/petrinet.h"
#include "petrinets/prooftree.h"

namespace py = pybind11;

bool CheckBisimilarity(
    std::vector<int> resource_one, std::vector<int> resource_two,
    std::vector<std::tuple<std::string, std::string, std::vector<int>, std::vector<int>>>
        transitions,
    bool record_basis, std::string path) {
    Multiset first(std::move(resource_one)), second(std::move(resource_two));
    PetriNet net(transitions);
    ProofTree tree(first, second, &net, record_basis);
    bool res = tree.CheckBisimilarity();
    tree.PrintTree(std::ofstream{path});
    return res;
}

PYBIND11_MODULE(_core, module) {
    module.doc() = "Binding for the main bisimilarity checking function";
    module.def("check_bisimilarity", &CheckBisimilarity,
               "A function that checks bisimilarity of two resources on a given Petri net and "
               "prints the resulting decision tree to a specified path.\n\nOptionally the function "
               "can approximate a basis of the bisimilarity.");
}