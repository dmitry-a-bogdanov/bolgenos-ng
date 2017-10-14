project(basalt)

set(HEADERS
	"include/basalt/format_guard.hpp"
	"include/basalt/intrusive_list.hpp"
	"include/basalt/scope_logger.hpp"
	"include/basalt/type_traits.hpp"
)

set(SOURCES
	"scoped_format_guard.cpp"
	"scope_logger.cpp"
)

add_library(basalt STATIC ${HEADERS} ${SOURCES})
target_link_libraries(basalt PRIVATE kstdc++)
target_include_directories(basalt PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
