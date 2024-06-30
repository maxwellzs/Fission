cmake_minimum_required(VERSION 3.27)
project(Fission)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/03597a01ee50ed33e9dfd640b249b4be3799d395.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

find_package(LIBEVENT REQUIRED)

include_directories(${CMAKE_SOURCE_DIR})

enable_testing()

include_directories(${LIBEVENT_INCLUDE_DIR})

add_library(FissionUtils SHARED
        log/LogQueue.cpp
        log/LogQueue.h
        core/Interface.h
        log/Logger.cpp
        log/Logger.h
        log/LogManager.cpp
        log/LogManager.h
        core/utils/concurrent/ThreadPool.cpp
        core/utils/concurrent/ThreadPool.h
        core/utils/collections/ConcurrentQueue.cpp
        core/utils/collections/ConcurrentQueue.h
        core/utils/collections/HazardList.cpp
        core/utils/collections/HazardList.h
        core/utils/collections/HashMap.cpp
        core/utils/collections/HashMap.h
        core/utils/network/BufferReader.cpp
        core/utils/network/BufferReader.h
)
add_executable(Fission main.cpp
        core/utils/network/Controller.cpp
        core/utils/network/Controller.h)
add_executable(FissionTest
        test/LogTest.cpp
        test/FissionBaseTest.cpp)

target_link_libraries(FissionUtils
        ${LIBEVENT_LIB}
)
target_link_libraries(Fission
        FissionUtils
)
target_link_libraries(FissionTest
        FissionUtils
        GTest::gtest_main
        GTest::gmock
)

include(GoogleTest)
gtest_discover_tests(FissionTest)

