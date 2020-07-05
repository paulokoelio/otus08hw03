cmake_minimum_required(VERSION 3.10)

set(BUILD_VER 1)
set(BUILD_SUBVER 0)
set(BUILD_PATCH 0)

if (DEFINED ENV{TRAVIS_BUILD_NUMBER})
    set (BUILD_PATCH $ENV{TRAVIS_BUILD_NUMBER})
endif()

project(allocator VERSION ${BUILD_VER}.${BUILD_SUBVER}.${BUILD_PATCH})

add_subdirectory(include)
add_subdirectory(src)
add_subdirectory(test)

enable_testing()
add_test(testfull "test/testall")
