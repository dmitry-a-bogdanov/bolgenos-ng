project(basalt)

set(SOURCES
	"scoped_format_guard.cpp"
)

add_library(basalt STATIC ${SOURCES})
target_link_libraries(basalt PRIVATE kstdc++)
target_include_directories(basalt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
