
function(add_flags flags)
	add_compile_options(${flags})
endfunction()

function(add_flags_cxx flags)
	add_compile_options("$<$<COMPILE_LANGUAGE:CXX>:${flags}>")
endfunction()

set(flags_platform
	-ffreestanding
	-nostdlib
	-m32 -march=i686)


set(flags_platform_cxx
	-std=c++17
	-fno-exceptions
	-fno-rtti
)

set(flags_diagnostic_common
	-Wall
	-Werror
	-Winline
	-Wextra
	-Wmissing-include-dirs
	-Wnull-dereference
	-Wstrict-aliasing
	-Wpedantic
	-Wcast-qual
	-Wcast-align
	-Wwrite-strings
	-Wlogical-op
	-Wlogical-not-parentheses
	-Wno-aggressive-loop-optimizations
	-Winline
	-Wdisabled-optimization
	-pedantic
	-pedantic-errors
)

set(flags_diagnostic_cxx
	-Weffc++
	-Wold-style-cast
	-Wsuggest-final-methods
	-Wsuggest-override
	-Wplacement-new=2
	-Wzero-as-null-pointer-constant
	-Wuseless-cast
)

add_flags("${flags_platform}")
add_flags_cxx("${flags_platform_cxx}")

add_flags("${flags_diagnostic_common}")
add_flags_cxx("${flags_diagnostic_cxx}")
