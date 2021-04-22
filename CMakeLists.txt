cmake_minimum_required(VERSION 3.17)
project(BisimilarityGame)

set(CMAKE_CXX_STANDARD 20)

add_executable(BisimilarityGame lib/src/petrinets/petrinet.h lib/src/petrinets/petrinet.cpp lib/src/petrinets/prooftree.cpp lib/src/petrinets/prooftree.h lib/main.cpp)