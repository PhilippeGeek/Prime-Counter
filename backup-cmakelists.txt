cmake_minimum_required(VERSION 3.1)
set(GCC_COVERAGE_COMPILE_FLAGS "-std=c90 -pedantic")
project(Primes)

if(UNIX)
    link_libraries(m)
    link_libraries(pthread)
endif(UNIX)

set(EXECUTABLE_OUTPUT_PATH build/${CMAKE_BUILD_TYPE})

file(
    GLOB_RECURSE
    src
    src/*
)

add_executable(
    prime.out
    ${src}
)

add_definitions(${GCC_COVERAGE_COMPILE_FLAGS})
