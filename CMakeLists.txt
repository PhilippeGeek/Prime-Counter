cmake_minimum_required(VERSION 2.6)

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
