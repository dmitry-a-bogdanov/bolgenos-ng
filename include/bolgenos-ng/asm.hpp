#pragma once

#include <cstdint>

#include "compiler.h"

#include <lib/type_traits.hpp>


/// \brief Pointer to descriptor table.
///
/// Structure for keeping pointer to IDT and GDT.
struct _packed_ table_pointer {
	uint16_t limit:16;	/*!< Size of table minus one */
	void *base;		/*!< 32-bit address of table in memory */
};


/// \brief Privilege level.
///
/// Enum holds values of privilege level as index of protection level.
enum protection_ring_t {
	ring_null = 0x0,
	ring_kernel = 0x0,
	ring_1 = 0x1,
	ring_2 = 0x2,
	ring_user = 0x3
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
	asm volatile ("outb %0, %1 \n":: "a"(byte), "Nd"(port));
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
	asm volatile("inb %1, %0 \n": "=a"(byte) : "Nd"(port));
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
	asm volatile("rdmsr \n": "=a"(*low), "=d"(*high): "c"(msr));
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
	asm volatile("wrmsr \n":: "a"(low), "d"(high), "c"(msr));
}


namespace x86 {


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 1)>::type load(const T& from, T& to)
{
	asm volatile("movb %1, %b0 \n"
			: "=r"(to)
			: "m"(from)
			: "memory");
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 2)>::type load(const T& from, T& to)
{
	asm volatile("movw %1, %w0 \n"
			: "=r"(to)
			: "m"(from)
			: "memory");
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 4)>::type load(const T& from, T& to)
{
	asm volatile("movl %1, %k0 \n"
			: "=r"(to)
			: "m"(from)
			: "memory");
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 1)>::type store(const T& from, T& to)
{
	asm volatile("movb %b1, %0 \n"
			: "=m"(to)
			: "r"(from)
			: "memory");
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 2)>::type store(const T& from, T& to)
{
	asm volatile("movw %w1, %0 \n"
			: "=m"(to)
			: "r"(from)
			: "memory");
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 4)>::type store(const T& from, T& to)
{
	asm volatile("movl %k1, %0 \n"
			: "=m"(to)
			: "r"(from)
			: "memory");
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 1)>::type exchange(T& from, T& to)
{
	asm volatile("lock; xchgb %b1, %2"
			: "=a"(to)
			: "m"(from), "ar"(to)
			: "memory"
	);
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 2)>::type exchange(T& from, T& to)
{
	asm volatile("lock; xchgw %w1, %2 \n"
			: "=a"(to)
			: "m"(from), "ar"(to)
			: "memory"
	);
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 4)>::type exchange(T& from, T& to)
{
	asm volatile("lock; xchgl %k1, %2 \n"
			: "=a"(to)
			: "m"(from), "ar"(to)
			: "memory"
	);
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 1), T>::type fetch_add(T& variable, T increment)
{
	asm volatile("lock; xaddb %b0, %1 \n"
		: "+r"(increment), "+m"(variable)
		:
		: "memory");
	return increment;
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 2), T>::type fetch_add(T& variable, T increment)
{
	asm volatile("lock; xaddw %w0, %1 \n"
		: "+r"(increment), "+m"(variable)
		:
		: "memory");
	return increment;
}


template<typename T>
inline
typename lib::enable_if<(sizeof(T) == 4), T>::type fetch_add(T& variable, T increment)
{
	asm volatile("lock; xaddl %k0, %1 \n"
		: "+r"(increment), "+m"(variable)
		:
		: "memory");
	return increment;
}


// \brief Put CPU into halt state.
//
// Function puts CPU into halt state. In such state CPU does nothing until
//	any kind of interrupt occurs.
inline
void halt_cpu() {
	asm volatile("hlt");
}


inline
static uint32_t lzcnt(uint32_t value)
{
	uint32_t retval;
	asm("lzcntl %1, %0 \n": "=a"(retval) : "b"(value));
	return retval;
};

} // namespace asm
