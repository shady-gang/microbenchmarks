cmake_minimum_required(VERSION 3.13)
project(shady-microbenchmarks)

set(CMAKE_CXX_STANDARD 17)

find_package(shady REQUIRED)
#add_executable(microbenchmarks main.cpp)
#target_link_libraries(microbenchmarks PRIVATE shady::runtime shady::driver)

function(add_benchmark)
    cmake_parse_arguments(PARSE_ARGV 0 F "" "NAME" "" )

    # prepare the .ll file for the runtime to eat
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${F_NAME}.comp.cpp.ll COMMAND shady::vcc ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${F_NAME}.comp.cpp -std=c++17 --only-run-clang -O3 -o ${CMAKE_BINARY_DIR}/${F_NAME}.comp.cpp.ll DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${F_NAME}.comp.cpp)
    add_custom_target("${F_NAME}_ll" DEPENDS  ${CMAKE_BINARY_DIR}/${F_NAME}.comp.cpp.ll)

    add_executable("${F_NAME}" ${F_NAME}.cpp bench.cpp)
    target_compile_definitions("${F_NAME}" PRIVATE -DLL_FILE_NAME="${F_NAME}.comp.cpp.ll")
    target_compile_definitions("${F_NAME}" PRIVATE -DBENCH_NAME=${F_NAME})
    target_link_libraries("${F_NAME}" PRIVATE shady::runtime shady::driver)
    add_dependencies(${F_NAME} "${F_NAME}_ll")
    #target_link_libraries("${F_NAME}_lib")

    # todo nvcc ?
endfunction()

add_benchmark(NAME add_buffers)

