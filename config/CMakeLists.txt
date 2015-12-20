set(config_h		"${CMAKE_CURRENT_BINARY_DIR}/config.h")
set(config_h_in		"config.h.in")

set(CONFIG__BOLGENOS_NG_VERSION "${bolgenos-ng_VERSION}")

configure_file(${config_h_in} ${config_h})

set(CONFIG_INCLUDE_DIRS "${CMAKE_CURRENT_BINARY_DIR}" PARENT_SCOPE)
