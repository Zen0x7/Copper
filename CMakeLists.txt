cmake_minimum_required(VERSION 3.25)
project(Copper)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILD_TESTS "Build tests cases" ON)

find_package(OpenSSL REQUIRED)
link_directories(${OPENSSL_LIBRARIES})
include_directories(${OPENSSL_INCLUDE_DIR})

include(FetchContent)
include_directories(lib/include)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lib/sources/*.cpp")
file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc")

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g")
set(CMAKE_CXX_FLAGS_RELEASE "-O3")

SET(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/bin)

if (BUILD_TESTS)
    fetchcontent_declare(
            googletest
            URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
    )

    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
    fetchcontent_makeavailable(googletest)

    enable_testing()
    add_executable(tests ${TESTS} ${SOURCES})
    target_link_libraries(tests GTest::gtest_main ${OPENSSL_LIBRARIES})
    include(GoogleTest)
    gtest_discover_tests(tests)
endif ()

add_executable(copper main.cpp ${SOURCES})
target_link_libraries(copper ${OPENSSL_LIBRARIES})
