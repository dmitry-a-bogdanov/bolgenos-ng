project(libthreading)

add_library(libthreading STATIC
	lock.cpp
	mutex.cpp
	threading.cpp
)

target_include_directories(libthreading PUBLIC ../include/)

target_link_libraries(libthreading PUBLIC libx86)