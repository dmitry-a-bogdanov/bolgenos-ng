
macro(config_option o_name o_def_val)
	if (NOT DEFINED ${o_name})
		set(${o_name} ${o_def_val})
	endif()
endmacro(config_option)
