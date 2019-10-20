cmake_minimum_required(VERSION 3.15)
project(Key_Value_store)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(Key_Value_store
        header.hpp
        KVClient.cpp
        KVServer.cpp)

add_executable(KVClient KVClient.cpp)
add_executable(KVServer KVServer.cpp)
add_executable(test cmake-build-debug/test.cpp)
add_executable(threadTest threadTest.cpp)
add_executable(KVCache KVCache.cpp)