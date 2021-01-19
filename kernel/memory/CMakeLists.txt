project(memory)

add_library(memory STATIC
	buddy_allocator.cpp
	buddy_allocator.hpp
	free_list.cpp
	free_list.hpp
	mallocator.cpp
	mallocator.hpp
	memory.cpp
	memory_region.hpp
	page.cpp
	page_allocator.cpp
	page_allocator.hpp
	slab.cpp
	)
target_include_directories(memory PUBLIC include)
target_link_libraries(memory PRIVATE libkernelcxx libthreading)
