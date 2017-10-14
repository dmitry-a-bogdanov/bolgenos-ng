project(kstdc++)

set(HEADERS
	"include/algorithm"
	"include/atomic"
	"include/cstddef"
	"include/cstdint"
	"include/cstring"
	"include/forward_list"
	"include/functional"
	"include/impl/cstddef/byte.hpp"
	"include/impl/functional/function.hpp"
	"include/impl/ios/basic_ios.hpp"
	"include/impl/ios/basic_ostream.hpp"
	"include/impl/ios/char_traits.hpp"
	"include/impl/ios/ios_base.hpp"
	"include/impl/ios/types.hpp"
	"include/impl/memory/default_allocator.hpp"
	"include/impl/memory/default_delete.hpp"
	"include/impl/memory/pointer_control_block.hpp"
	"include/impl/memory/shared_ptr.hpp"
	"include/impl/memory/unique_ptr.hpp"
	"include/impl/memory/weak_ptr.hpp"
	"include/impl/type_traits/enable_if.hpp"
	"include/impl/type_traits/integral_constant.hpp"
	"include/impl/type_traits/is_signed.hpp"
	"include/impl/type_traits/is_unsigned.hpp"
	"include/impl/type_traits/make_signed.hpp"
	"include/impl/type_traits/make_unsigned.hpp"
	"include/impl/type_traits/remove_cv.hpp"
	"include/impl/type_traits/underlying_type.hpp"
	"include/ios"
	"include/memory"
	"include/new"
	"include/ostream"
	"include/streambuf"
	"include/streambuf_backup"
	"include/string"
	"include/type_traits"
	"include/utility"
)

set(SOURCES
	"ios_base.cpp"
	"new.cpp"
	"ostream.cpp"
	"snprintf.cpp"
	"streambuf.cpp"
	"string.cpp"
	"vga_buf.cpp"
	"vga_buf.hpp"
	"vga_log_buf.cpp"
	"vga_log_buf.hpp"
)

add_library(kstdc++ STATIC ${HEADERS} ${SOURCES}

)
target_include_directories(kstdc++ PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
