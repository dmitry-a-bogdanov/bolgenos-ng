project(tools)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic")

add_executable(generate_partition_table generate_partition_table.cpp)
