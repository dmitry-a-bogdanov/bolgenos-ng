project(libkernelcxx)

set(libkernelcxx_HEADERS
	include/algorithm.hpp
	include/array.hpp
	include/atomic.hpp
	include/cstddef.hpp
	include/cstdint.hpp
	include/cstring.hpp
	include/ext/fixed_size_vector.hpp
	include/ext/intrusive_circular_list.hpp
	include/ext/memory.hpp
	include/ext/scoped_format_guard.hpp
	include/ext/snprintf_stream.hpp
	include/forward_list.hpp
	include/impl/basic_forward_list.hpp
	include/impl/cstddef/byte.hpp
	include/impl/type_traits/enable_if.hpp
	include/impl/type_traits/integral_constant.hpp
	include/impl/type_traits/is_signed.hpp
	include/impl/type_traits/is_unsigned.hpp
	include/impl/type_traits/make_signed.hpp
	include/impl/type_traits/make_unsigned.hpp
	include/impl/type_traits/remove_cv.hpp
	include/memory.hpp
	include/ostream.hpp
	include/streambuf.hpp
	include/type_traits.hpp
	include/utility.hpp

	src/cstring.cpp
	src/ext/scoped_format_guard.cpp
	src/ext/snprintf_stream.cpp
	src/ostream.cpp
	src/streambuf.cpp
)

set(libkernelcxx_SOURCES
	src/cstring.cpp
	src/ostream.cpp
	src/streambuf.cpp
	src/string.cpp
	src/new.cpp

	src/ext/scoped_format_guard.cpp
	src/ext/snprintf_stream.cpp
)

add_library(libkernelcxx STATIC
	${libkernelcxx_HEADERS}
	${libkernelcxx_SOURCES}
)

target_include_directories(libkernelcxx PUBLIC include/)
target_link_libraries(libkernelcxx PUBLIC log)
