cmake_minimum_required(VERSION 3.14)
project(MemoryFragmentation)

set(CMAKE_CXX_STANDARD 17)

if (WIN32)
	set(LIBS psapi)
else()
	set(LIBS "")
endif()

add_executable(fragmentation main.cpp)
target_link_libraries(fragmentation ${LIBS})
