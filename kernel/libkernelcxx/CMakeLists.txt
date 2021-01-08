project(libkernelcxx)

set(libkernelcxx_HEADERS
	include/algorithm.hpp
	include/array.hpp
	include/cstddef.hpp
	include/cstdint.hpp
	include/cstring.hpp
	include/memory.hpp
	include/type_traits.hpp
	include/utility.hpp

	include/ext/scoped_format_guard.hpp
)

set(libkernelcxx_SOURCES
	src/cstring.cpp
	src/ostream.cpp
	src/snprintf.cpp
	src/streambuf.cpp

	src/ext/scoped_format_guard.cpp
)

add_library(libkernelcxx OBJECT
	${libkernelcxx_HEADERS}
	${libkernelcxx_SOURCES}
)

target_include_directories(libkernelcxx PUBLIC include/)