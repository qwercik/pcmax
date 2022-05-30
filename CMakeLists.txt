cmake_minimum_required(VERSION 2.8)
project(pcmax)

add_executable(pcmax src/pcmax/main.cpp)

target_include_directories(pcmax PUBLIC include/)
target_link_libraries(pcmax PUBLIC OpenCL m)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Wpedantic -O2 -fopenmp")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Wpedantic -O2 -fopenmp")

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/src/kernel/kernel.cl ${CMAKE_CURRENT_BINARY_DIR}/kernel.cl COPYONLY)

