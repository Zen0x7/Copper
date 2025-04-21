cmake_minimum_required(VERSION 3.25)
project(Copper)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(Boost_USE_STATIC_LIBS ON)
set(Boost_USE_STATIC_RUNTIME ON)
set(BOOST_ALL_NO_LIB ON)
set(BUILD_SHARED_LIBS OFF)
set(ZLIB_USE_STATIC_LIBS ON)
set(OPENSSL_USE_STATIC_LIBS TRUE)

# Detectar arquitectura y ajustar rutas
if(CMAKE_SYSTEM_PROCESSOR MATCHES "aarch64")
    set(ARCH_LIB_DIR "/usr/lib/aarch64-linux-gnu")
elseif(CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64")
    set(ARCH_LIB_DIR "/usr/lib/x86_64-linux-gnu")
else()
    message(FATAL_ERROR "Arquitectura no soportada: ${CMAKE_SYSTEM_PROCESSOR}")
endif()

set(OPENSSL_CRYPTO_LIBRARY "${ARCH_LIB_DIR}/libcrypto.a")
set(OPENSSL_SSL_LIBRARY "${ARCH_LIB_DIR}/libssl.a")
set(ZLIB_LIBRARY "${ARCH_LIB_DIR}/libz.a")
set(ZLIB_INCLUDE_DIR "/usr/include")

set(CMAKE_FIND_LIBRARY_SUFFIXES ".a")

option(BUILD_TESTS "Build tests cases" ON)
option(BUILD_EXECUTABLE "Build executable" ON)
option(DEBUG_FEATURES "Enable debugging features in build" ON)
option(BUILD_FOR_BENCHMARK "Enable runtime to benchmark environment" OFF)

if(DEBUG_FEATURES)
    add_definitions(-DDEBUG_ENABLED)
endif()

if (BUILD_FOR_BENCHMARK)
    add_definitions(-DBUILD_FOR_BENCHMARK)
endif ()

add_definitions(-DBOOST_ASIO_CONCURRENCY_HINT_DEFAULT=BOOST_ASIO_CONCURRENCY_HINT_UNSAFE)

find_package(OpenSSL REQUIRED)
link_directories(${OPENSSL_LIBRARIES})
include_directories(${OPENSSL_INCLUDE_DIR})

find_package(ZLIB REQUIRED)

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
        GIT_TAG        e69e5f977d458f2650bb346dadf2ad30c5320281
)
FetchContent_MakeAvailable(fmt)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/lib/sources/*.cpp")
file(GLOB_RECURSE TESTS CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/tests/*.cc")

include_directories(${Boost_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "-Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-g --coverage -fdump-ipa-inline")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -march=native -flto -fno-rtti -fomit-frame-pointer -ffunction-sections -fdata-sections -fno-stack-protector -DNDEBUG")
set(CMAKE_C_FLAGS "--coverage")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "-Wl,--gc-sections -Wl,--strip-all")

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
            deps/asio.cxx
            deps/beast.cxx
            deps/redis.cxx
            deps/mysql.cxx
            ${SOURCES}
            ${TESTS}
    )
    target_include_directories(tests PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(tests
            ${ZLIB_LIBRARY}
            GTest::gtest_main
            ${OPENSSL_SSL_LIBRARY}
            ${OPENSSL_CRYPTO_LIBRARY}
            ${Boost_LIBRARIES}
            fmt::fmt
            bcrypt
            nlohmann_json::nlohmann_json
    )
    include(GoogleTest)
    gtest_discover_tests(tests)
endif ()

if (BUILD_EXECUTABLE)
    add_executable(copper
            deps/asio.cxx
            deps/beast.cxx
            deps/redis.cxx
            deps/mysql.cxx
            main.cpp
            ${SOURCES}
    )
    target_include_directories(copper PRIVATE ${ZLIB_INCLUDE_DIRS})
    target_link_libraries(copper
            ${ZLIB_LIBRARY}
            ${OPENSSL_SSL_LIBRARY}
            ${OPENSSL_CRYPTO_LIBRARY}
            ${Boost_LIBRARIES}
            fmt::fmt
            bcrypt
            nlohmann_json::nlohmann_json
    )
    target_compile_definitions(copper PRIVATE
            BOOST_ASIO_NO_DEPRECATED
            BOOST_ASIO_HAS_CO_AWAIT
            BOOST_BEAST_USE_STD_STRING_VIEW
    )
endif ()
