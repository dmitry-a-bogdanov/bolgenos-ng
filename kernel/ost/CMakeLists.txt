project(ost)

add_library(ost STATIC
	include/bolgenos-ng/ost.hpp
	src/bitarray.cpp
	src/memory.cpp
	src/ost.cpp
	src/type_traits.cpp
)

target_link_libraries(ost PRIVATE libkernelcxx libx86)
target_include_directories(ost PUBLIC include)
