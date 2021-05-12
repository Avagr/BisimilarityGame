cmake_minimum_required(VERSION 3.17)
project(bisimilarity_checker VERSION "1.0")
set(CMAKE_CXX_STANDARD 17)
if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    set(CMAKE_CXX_FLAGS "-O3")
else()
    set(CMAKE_CXX_FLAGS "/Ox")
endif()

if (SKBUILD)
    execute_process(
            COMMAND
            "${PYTHON_EXECUTABLE}" -c
            "import pybind11; print(pybind11.get_cmake_dir())"
            OUTPUT_VARIABLE _tmp_dir
            OUTPUT_STRIP_TRAILING_WHITESPACE COMMAND_ECHO STDOUT)
    list(APPEND CMAKE_PREFIX_PATH "${_tmp_dir}")
endif ()

find_package(pybind11 CONFIG REQUIRED)
pybind11_add_module(_core MODULE src/binding.cpp src/petrinets/petrinet.h src/petrinets/petrinet.cpp src/petrinets/prooftree.cpp src/petrinets/prooftree.h src/binding.cpp)

target_compile_definitions(_core PRIVATE VERSION_INFO=${PROJECT_VERSION})

install(TARGETS _core DESTINATION .)

#add_executable(BisimilarityGame src/petrinets/petrinet.h src/petrinets/petrinet.cpp src/petrinets/prooftree.cpp src/petrinets/prooftree.h main.cpp)
