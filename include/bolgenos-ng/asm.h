#ifndef __BOLGENOS_NG__ASM_H__
#define __BOLGENOS_NG__ASM_H__

#include <bolgenos-ng/int_types.h>
#include <bolgenos-ng/compiler.h>


/**
* \brief Pointer to descriptor table.
*
* Structure for keeping pointer to IDT and GDT.
*/
struct _packed_ table_pointer {
	uint16_t limit:16;	/*!< Size of table minus one */
	void *base;		/*!< 32-bit address of table in memory */
};


/**
* \brief Wrapper around outb assembler instruction.
*
* Fuction writes given one-byte value to given port.
*
* \param port Port for writing to.
* \param byte One-byte value to be written.
*/
static inline void outb(uint16_t port, uint8_t byte) {
	asm volatile ("outb %0, %1":: "a"(byte), "Nd"(port));
}


/**
* \brief Wrapper aroung inb assembler instruction.
*
* Fuction read one-byte value from given port.
*
* \param port Port for reading from.
* \return Value that was read from given port.
*/
static inline uint8_t inb(uint16_t port) {
	uint8_t byte;
	asm volatile("inb %1, %0": "=a"(byte) : "Nd"(port));
	return byte;
}


/**
* \brief Wait for end of I/O operation.
*
* Functions uses hack with writing any byte to 0x80 port in order to finish
*	I/O operation via CPU port.
*/
static inline void iowait() {
	outb(0x80, 0x00);
}


/**
* \brief Put CPU into halt state.
*
* Function puts CPU into halt state. In such state CPU does nothing until
*	any kind of interrupt occurs.
*/
static inline void halt_cpu() {
	asm volatile("hlt");
}


/**
* \brief Constant for MSR.
*
* Constants that are used for Machine Specific Register indexing.
*/
typedef enum {
	msr_ia32_apic_base = 0x1b /*!< Start address of APIC mapped memory. */
} msr_t;


/**
* \brief Read MSR.
*
* Read specified Machine Specific Register to given address that is split to
*	two 32-bit parts.
*
* \param msr Machine Specific Register id to be read.
* \param low Address of lowest bytes of output value.
* \param high Address of highest bytes of output value.
*/
static inline void read_msr(msr_t msr, uint32_t *low, uint32_t *high) {
	asm volatile("rdmsr": "=a"(*low), "=d"(*high): "c"(msr));
}


/**
* \brief Write MSR.
*
* Write to specified Machine Specific Register given value that is split to
*	two 32-bit parts.
*
* \param msr Machine Specific Register id to be written.
* \param low Lowest bytes of output value.
* \param high Highest bytes of output value.
*/
static inline void write_msr(msr_t msr, uint32_t low, uint32_t high) {
	asm volatile("wrmsr":: "a"(low), "d"(high), "c"(msr));
}

#endif // __BOLGENOS_NG__ASM_H__
