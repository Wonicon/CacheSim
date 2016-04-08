cmake_minimum_required(VERSION 3.4)
project(CacheSim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES
        Storage.h
        Memory.cpp Memory.h
        CPU.cpp CPU.h
        main.cpp Cache.cpp Cache.h)

include_directories(.)

add_executable(CacheSim ${SOURCE_FILES})
