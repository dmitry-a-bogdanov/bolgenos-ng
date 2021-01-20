project(ost)

add_library(ost STATIC
	include/bolgenos-ng/ost.hpp
	src/bitarray.cpp
	src/bitarray.hpp
	src/memory.cpp
	src/memory.hpp
	src/ost.cpp
	src/type_traits.cpp
)

target_link_libraries(ost PRIVATE libkernelcxx)
target_include_directories(ost PUBLIC include)