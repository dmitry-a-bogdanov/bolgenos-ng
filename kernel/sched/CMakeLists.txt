project(sched)

add_library(sched STATIC
	include/sched.hpp
	include/sched/task.hpp

	sched.cpp
	scheduler.hpp
	scheduler.cpp
	task.cpp
	)

target_include_directories(sched PUBLIC include)
target_link_libraries(sched PUBLIC libkernelcxx libthreading libx86)
