project(tools)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic")

set(gen_part_table_bin "gen_part_table")
add_executable(${gen_part_table_bin} gen_part_table.cpp)

