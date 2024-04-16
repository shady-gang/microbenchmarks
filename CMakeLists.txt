cmake_minimum_required(VERSION 3.13)
project(shady-microbenchmarks)

set(CMAKE_CXX_STANDARD 17)

find_package(shady REQUIRED)
add_executable(microbenchmarks main.cpp)
target_link_libraries(microbenchmarks PRIVATE shady::runtime shady::driver)

# prepare the .ll file for the runtime to eat
add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/main_gpu.ll COMMAND shady::vcc ARGS ${CMAKE_CURRENT_SOURCE_DIR}/main_gpu.cpp -std=c++17 --only-run-clang -O3 -o ${CMAKE_BINARY_DIR}/main_gpu.ll DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/main_gpu.cpp)
add_custom_target(main_gpu_ll DEPENDS  ${CMAKE_BINARY_DIR}/main_gpu.ll)
add_dependencies(microbenchmarks main_gpu_ll)
