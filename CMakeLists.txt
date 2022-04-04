cmake_minimum_required(VERSION 3.10)

project(FaultMapping LANGUAGES CXX)

find_package(ARGoS REQUIRED)

add_library(foraging_loop_functions SHARED
  lib/foraging_loop_functions.h
  lib/foraging_loop_functions.cpp)

target_include_directories(foraging_loop_functions PUBLIC ${ARGOS_INCLUDE_DIRECTORIES})

target_link_libraries(foraging_loop_functions PUBLIC ${ARGOS_LIBRARY})
