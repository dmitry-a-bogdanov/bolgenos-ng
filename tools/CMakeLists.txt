project(tools)

set(CMAKE_CXX_FLAGS "-std=c++11 -Wall -Wextra -pedantic")

set(gen_part_table_bin "gen_part_table")

set(export_prefix "tools::")
set(gen_part_table_export "${export_prefix}gen_part_table")
set(tools_export_dir "tools_export/")

add_executable(${gen_part_table_bin} gen_part_table.cpp)

install(TARGETS ${gen_part_table_bin} EXPORT ${gen_part_table_bin}
	RUNTIME DESTINATION ${tools_export_dir})
install(EXPORT ${gen_part_table_bin} NAMESPACE tools:: DESTINATION ${tools_export_dir})

add_executable(${gen_part_table_export} ALIAS ${gen_part_table_bin})
