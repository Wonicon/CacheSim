cmake_minimum_required(VERSION 2.8)
project(CacheSim)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0 -std=c++11")

set(SOURCE_FILES
    Storage.h
    Memory.cpp Memory.h
    CPU.cpp CPU.h
    main.cpp Cache.cpp Cache.h VictimCache.cpp VictimCache.h debug.h debug.cpp)

include_directories(.)

add_executable(CacheSim ${SOURCE_FILES})

add_custom_command(TARGET CacheSim PRE_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_SOURCE_DIR}/trace.sh $<TARGET_FILE_DIR:CacheSim>)
