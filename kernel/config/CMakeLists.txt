project(config
	LANGUAGES NONE)

set(config_h		"${CMAKE_CURRENT_BINARY_DIR}/config.h")
set(config_h_in		"config.h.in")

set(ost_h		"${CMAKE_CURRENT_BINARY_DIR}/ost.h")
set(ost_h_in		"ost.h.in")

set(CONFIG__BOLGENOS_NG_VERSION "${bolgenos-ng_VERSION}")

if (NOT DEFINED KERNEL_CONFIG)
	set(KERNEL_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/default.cmake")
endif()

if (NOT DEFINED OST_CONFIG)
	set(OST_CONFIG "${CMAKE_CURRENT_SOURCE_DIR}/ost.cmake")
endif()

include(${KERNEL_CONFIG})
include(${OST_CONFIG})

configure_file(${config_h_in} ${config_h})
configure_file(${ost_h_in} ${ost_h})

add_library(config INTERFACE)
target_sources(config INTERFACE ${ost_h} ${config_h})
target_include_directories(config INTERFACE "${CMAKE_CURRENT_BINARY_DIR}")
