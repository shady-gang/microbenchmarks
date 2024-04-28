cmake_minimum_required(VERSION 3.13)
project(shady-microbenchmarks)

set(CMAKE_CXX_STANDARD 17)

find_package(shady REQUIRED)
#add_executable(microbenchmarks main.cpp)
#target_link_libraries(microbenchmarks PRIVATE shady::runtime shady::driver)

function(add_benchmark)
    cmake_parse_arguments(PARSE_ARGV 0 F "" "NAME;HOST_FILE" "OPTIONS" )

    # prepare the .ll file for the runtime to eat
    add_custom_command(OUTPUT ${CMAKE_BINARY_DIR}/${F_NAME}.comp.cpp.ll COMMAND shady::vcc ARGS ${CMAKE_CURRENT_SOURCE_DIR}/${F_NAME}.comp.cpp ${F_OPTIONS} --only-run-clang -o ${CMAKE_BINARY_DIR}/${F_NAME}.comp.cpp.ll DEPENDS ${CMAKE_CURRENT_SOURCE_DIR}/${F_NAME}.comp.cpp)
    add_custom_target("${F_NAME}_ll" DEPENDS  ${CMAKE_BINARY_DIR}/${F_NAME}.comp.cpp.ll)

    add_executable("${F_NAME}" ${F_HOST_FILE}.cpp bench.cpp)
    target_compile_definitions("${F_NAME}" PRIVATE -DLL_FILE_NAME="${F_NAME}.comp.cpp.ll")
    target_compile_definitions("${F_NAME}" PRIVATE -DBENCH_NAME=${F_NAME})
    target_link_libraries("${F_NAME}" PRIVATE shady::runtime shady::driver)
    add_dependencies(${F_NAME} "${F_NAME}_ll")
    #target_link_libraries("${F_NAME}_lib")

    # todo nvcc ?
endfunction()

add_benchmark(NAME add_buffers                 HOST_FILE add_buffers         OPTIONS -std=c++17 -O3)
add_benchmark(NAME add_buffers_generic         HOST_FILE add_buffers         OPTIONS -std=c++17 -O3)
add_benchmark(NAME add_buffers_chunked         HOST_FILE add_buffers_chunked OPTIONS -std=c++17 -O3)
add_benchmark(NAME add_buffers_chunked_generic HOST_FILE add_buffers_chunked OPTIONS -std=c++17 -O3)
add_benchmark(NAME fib                         HOST_FILE fib                 OPTIONS -std=c++17 -O1)
add_benchmark(NAME fib_tailrec                 HOST_FILE fib                 OPTIONS -std=c++17 -O3)
add_benchmark(NAME fib_mangled                 HOST_FILE fib                 OPTIONS -std=c++17 -O1)
add_benchmark(NAME fn_pointers                 HOST_FILE fn_pointers         OPTIONS -std=c++17 -O3)
add_benchmark(NAME fn_switched                 HOST_FILE fn_pointers         OPTIONS -std=c++17 -O1)
add_benchmark(NAME binary_tree                 HOST_FILE binary_tree         OPTIONS -std=c++17 -O1)
add_benchmark(NAME unions                      HOST_FILE unions              OPTIONS -std=c++17 -O1)

