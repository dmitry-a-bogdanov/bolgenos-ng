#ifndef __BOLGENOS_NG__COMPILER_H__
#define __BOLGENOS_NG__COMPILER_H__


/**
* \def macro_concat(begin, end)
* \brief Concatenate given sequencies of symbols.
*
* Macro concatenates given values sequencies of symbols with expanding.
* If symbols stands for macro name, this name will be expanded to value.
* \param begin First part of resulting symbol.
* \param end Second part of resultng symbol.
* \return Concatenation of begin and end arguments.
*/
#define __macro_concat(begin, end) \
		begin ## end
#define macro_concat(begin, end) \
		__macro_concat(begin, end)


/**
* \def stringify(x)
* \brief Stringify symbol.
*
* Macro make an ascii null-terminated string from specified symbol.
* \param x Symbol to be stringified.
* \return Stringified symbol.
*/
#define __stringify(x) #x
#define stringify(x) __stringify(x)


/**
* \brief Declare placeholder name.
*
* Macro creates unique name. It can be used for declaring placeholders in
* packed structures.
*/
#define _placeholder(bitsize) \
	macro_concat(___placeholder_, __COUNTER__) : (bitsize)


/**
* \brief Declare placeholder.
*
* Macro declares variable or structure member of type uint64_t with unique
* name. It can be used in the same way as \ref _placeholder when type of
* place holder doesn't matter, e.g. if structure will not be used
* in strict aliasing operations.
*/
#define placeholder(bitsize) \
	uint64_t _placeholder(bitsize)


/**
* \brief Alignment value for CPU.
*
* Macro defines value that should be used for alignment for structures
* that are loaded to CPU like GDT, IDT.
*/
#define cpu_alignment			(16)


/**
* \brief Aligned for MMU.
*
* Alignes symbol to allow use for MMU-related instructions. Alias to
* __attribute__((aligned(16)))
*/
#define _mmu_aligned_			__attribute__((aligned(cpu_alignment)))


/**
* \brief Aligned for IRQ.
*
* Alignes symbol to allow use for IRQ-related instructions. Alias to
* __attribute__((aligned(16)))
*/
#define _irq_aligned_			__attribute__((aligned(cpu_alignment)))


/**
* \brief Declaration of packed symbol.
*
* Macro to be used for declaring packed structures or unions. Synonym
* for corresponding attribute declaration.
*/
#define _packed_			__attribute__((packed))

/**
* \brief Make symbol used.
*
* Macro to be used for declaring symbols as used. It will prevent linker
* from removing symbols marked as _used_ from removing during linkage. Synonym
* for corresponding attribute declaration.
*/
#define _used_				__attribute__((used))

/**
* \brief Link name from assember code.
*
* Macro for marking symbols that are defined in asm code. Actually, it's just
*	extern declaration.
*/
#define _asm_linked_			extern

#endif // __BOLGENOS_NG__COMPILER_H__
