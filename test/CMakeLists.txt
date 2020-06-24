cmake_minimum_required(VERSION 3.10)

find_package(GTest REQUIRED)

add_executable(testall 
                testall.cpp
                "../src/version.cpp"
                )

set_target_properties(testall PROPERTIES     
    CXX_STANDARD 17
    CXX_STANDARD_REQUIRED ON
)

target_include_directories(testall PRIVATE 
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/src
    ${GTEST_INCLUDE_DIRS}
    "${CMAKE_BINARY_DIR}/include"
)

target_link_libraries( testall 
                        ${GTEST_LIBRARIES} 
                        Threads::Threads
                        )

if (MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    target_compile_options(testall PRIVATE
        /W4
    )
else ()
    target_compile_options(testall PRIVATE
        -Wall -Wextra -pedantic -Werror
    )
endif()
