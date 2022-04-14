cmake_minimum_required(VERSION 3.10)

project(FaultMapping LANGUAGES CXX)

find_package(ARGoS REQUIRED)

add_library(foraging_loop_functions SHARED
  lib/foraging_loop_functions.h
  lib/foraging_loop_functions.cpp)

target_include_directories(foraging_loop_functions PUBLIC ${ARGOS_INCLUDE_DIRS})

target_link_libraries(foraging_loop_functions PUBLIC ${ARGOS_LDFLAGS})

add_library(foraging_buzz_controller_kheperaiv SHARED
  lib/foraging_buzz_controller_kheperaiv.h
  lib/foraging_buzz_controller_kheperaiv.cpp)


target_include_directories(foraging_buzz_controller_kheperaiv PUBLIC ${ARGOS_INCLUDE_DIRS})

target_link_libraries(foraging_buzz_controller_kheperaiv PUBLIC ${ARGOS_LDFLAGS})
