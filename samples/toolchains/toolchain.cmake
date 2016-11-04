set(CMAKE_SYSTEM_NAME elf)
set(CMAKE_SYSTEM_PROCESSOR i686)
set(sdk_path "/home/dabogdanov/cross-host/i686-elf")
set(toolchain_path "${sdk_path}/toolchain")
set(toolchain_bin_path "${toolchain_path}/bin")

set(toolchain_debug FALSE)

macro(verbose_set VAR VALUE)
	set("${VAR}" "${VALUE}")
	if(${toolchain_debug})
		message(STATUS "exporting (toochain ${triple}): "
			"${VAR} = ${${VAR}}")
	endif()
endmacro(verbose_set)

# yep, it is not an actually triple
verbose_set(triple "${CMAKE_SYSTEM_PROCESSOR}-${CMAKE_SYSTEM_NAME}")

set(output_format_flags "-m32 -march=i686")

# -ffeestanding implies -fno-builtin
set(dependencies_flags "-ffreestanding -nostdlib")

set(asm_c_cxx_common_flags "${output_format_flags} ${dependencies_flags}")

set(cxx_unsupported_features "-fno-rtti -fno-exceptions")
set(cxx_specific "${cxx_unsupported_features}")

verbose_set(CMAKE_C_COMPILER "${toolchain_bin_path}/${triple}-gcc")
verbose_set(CMAKE_C_COMPILER_FORCED TRUE)
verbose_set(CMAKE_C_FLAGS "${asm_c_cxx_common_flags}")

verbose_set(CMAKE_CXX_COMPILER "${toolchain_bin_path}/${triple}-g++")
verbose_set(CMAKE_CXX_COMPILER_FORCED TRUE)
verbose_set(CMAKE_CXX_FLAGS "${asm_c_cxx_common_flags} ${cxx_specific}")

verbose_set(CMAKE_ASM-ATT_COMPILER "${CMAKE_C_COMPILER}")
verbose_set(CMAKE_ASM-ATT_COMPILER_FORCED TRUE)
verbose_set(CMAKE_ASM-ATT_FLAGS "${asm_c_cxx_common_flags}")

