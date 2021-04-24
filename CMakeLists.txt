cmake_minimum_required(VERSION 3.17)
project(BisimilarityGame)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_FLAGS "-m64 -O3 ")

find_package(pybind11 REQUIRED)
pybind11_add_module(bisimilarity lib/src/binding.cpp lib/src/petrinets/petrinet.h lib/src/petrinets/petrinet.cpp lib/src/petrinets/prooftree.cpp lib/src/petrinets/prooftree.h lib/src/binding.cpp)
#add_executable(BisimilarityGame lib/src/petrinets/petrinet.h lib/src/petrinets/petrinet.cpp lib/src/petrinets/prooftree.cpp lib/src/petrinets/prooftree.h lib/main.cpp)
