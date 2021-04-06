cmake_minimum_required(VERSION 3.17)
project(BisimilarityGame)

set(CMAKE_CXX_STANDARD 20)

add_executable(BisimilarityGame src/petrinets/multiset.cpp src/petrinets/multiset.h src/petrinets/prooftree.cpp src/petrinets/prooftree.h src/petrinets/petrinet.cpp src/petrinets/petrinet.h)