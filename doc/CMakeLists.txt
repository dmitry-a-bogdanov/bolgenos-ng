project(doc
	LANGUAGES NONE)

find_package(Doxygen)

if(NOT DOXYGEN_FOUND)
	message(FATAL_ERROR "Couldn't find doxygen")
endif()

set(doxygen_config_in "doxygen.conf.in")
set(doxygen_config "doxygen.conf")

configure_file(${doxygen_config_in} ${doxygen_config})

add_custom_target(doc
	COMMAND ${DOXYGEN_EXECUTABLE} ${doxygen_config}
	COMMENT "Generating documentation..."
)
