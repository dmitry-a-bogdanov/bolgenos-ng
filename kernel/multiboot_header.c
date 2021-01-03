#include <bolgenos-ng/compiler.h>

#include <stdint.h>

struct _packed_ multiboot_hdr {
	uint32_t magic;
	uint32_t flags;
	uint32_t checksum;
	uint32_t header_addr;
	uint32_t load_addr;
	uint32_t load_end_addr;
	uint32_t bss_end_addr;
	uint32_t entry_addr;
	uint32_t mode_type;
	uint32_t width;
	uint32_t height;
	uint32_t depth;
};

#define mboot_section			".multiboot_header"
#define __multiboot_section__		__attribute__((section(mboot_section)))

#define mboot_magic			(0x1badb002)
#define mboot_checksum(flags)		(-(mboot_magic + ((uint32_t) flags)))

enum mboot_flags {
	mbf_align			= 1 << 0,
	mbf_meminfo			= 1 << 1,
};

#define		mboot_header_initializer(_flags) {			\
	.magic		= mboot_magic,					\
	.flags		= (_flags),					\
	.checksum	= mboot_checksum((_flags))			\
}

/**
* \brief Multiboot header.
*
* This symbols declares multiboot header for kernel. It must be placed into
*	separate section and this section must be placed by linker to the
*	beginning of resulting ELF-file.
*/
struct multiboot_hdr __multiboot_section__ _used_
	multiboot_header = mboot_header_initializer(mbf_align | mbf_meminfo);
