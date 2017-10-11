project(kernelcxx)

set(kstdc++_SOURCES
	"scoped_format_guard.cpp"
)

add_library(kstdc++ STATIC ${SOURCES}
	ios_base.cpp
	new.cpp
	ostream.cpp
	streambuf.cpp
	vga_buf.cpp
	vga_buf.hpp
	vga_log_buf.cpp
	vga_log_buf.hpp
)
target_include_directories(kstdc++ PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
