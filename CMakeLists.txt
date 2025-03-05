cmake_minimum_required(VERSION 3.25)
project(Copper)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(BUILD_TESTS "Build tests cases" ON)

find_package(OpenSSL REQUIRED)
link_directories(${OPENSSL_LIBRARIES})
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(ZLIB REQUIRED)
find_package(spdlog REQUIRED)
find_package(sentry REQUIRED)

find_package(
        Boost 1.87.0
        COMPONENTS
        json
        thread
        program_options
        random
        coroutine
        charconv
        system
        iostreams
        REQUIRED
)

find_package(nlohmann_json 3.11.3 REQUIRED)
find_package(inja REQUIRED)

include(FetchContent)
include_directories(lib/include)

FetchContent_Declare(
        fmt
        GIT_REPOSITORY https://github.com/fmtlib/fmt
        GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281) # 10.2.1
FetchContent_MakeAvailable(fmt)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lib/sources/*.cpp")
file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc")

include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g -O0 --coverage")
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
    add_executable(tests
            deps/asio.cpp
            deps/beast.cpp
            deps/redis.cpp
            deps/mysql.cpp
            ${SOURCES}
            ${TESTS}
    )
    target_include_directories(tests PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(tests
            spdlog::spdlog
            ZLIB::ZLIB
            GTest::gtest_main
            ${OPENSSL_LIBRARIES}
            ${Boost_LIBRARIES}
            fmt::fmt
            bcrypt
            nlohmann_json::nlohmann_json
            sentry::sentry
    )
    target_compile_definitions(tests PRIVATE BOOST_ASIO_SEPARATE_COMPILATION)
    target_compile_definitions(tests PRIVATE BOOST_REDIS_SEPARATE_COMPILATION)
    target_compile_definitions(tests PRIVATE BOOST_BEAST_SEPARATE_COMPILATION)
    target_compile_definitions(tests PRIVATE BOOST_MYSQL_SEPARATE_COMPILATION)
    include(GoogleTest)
    gtest_discover_tests(tests)
endif ()

add_executable(copper
        deps/asio.cpp
        deps/beast.cpp
        deps/redis.cpp
        deps/mysql.cpp
        main.cpp
        ${SOURCES}
)
target_include_directories(copper PRIVATE ${ZLIB_INCLUDE_DIRS})
target_link_libraries(copper
        spdlog::spdlog
        ZLIB::ZLIB
        ${OPENSSL_LIBRARIES}
        ${Boost_LIBRARIES}
        fmt::fmt
        bcrypt
        nlohmann_json::nlohmann_json
        sentry::sentry
)
target_compile_definitions(copper PRIVATE BOOST_ASIO_SEPARATE_COMPILATION)
target_compile_definitions(copper PRIVATE BOOST_REDIS_SEPARATE_COMPILATION)
target_compile_definitions(copper PRIVATE BOOST_BEAST_SEPARATE_COMPILATION)
target_compile_definitions(copper PRIVATE BOOST_MYSQL_SEPARATE_COMPILATION)
