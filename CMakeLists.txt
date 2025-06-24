cmake_minimum_required(VERSION 3.14)
project(MemoryFragmentation)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
    set(PLATFORM_LIBS psapi)
elseif(APPLE)
    include_directories("/opt/homebrew/include")
    link_directories("/opt/homebrew/lib")
    find_library(JEMALLOC jemalloc REQUIRED PATHS /opt/homebrew/lib)
    set(PLATFORM_LIBS ${JEMALLOC})
else()
    find_library(JEMALLOC jemalloc REQUIRED)
    set(PLATFORM_LIBS ${JEMALLOC})
endif()

add_executable(fragmentation main.cpp)
target_link_libraries(fragmentation PRIVATE ${PLATFORM_LIBS})
