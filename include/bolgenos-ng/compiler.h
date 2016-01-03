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


#define __assert_ct_ge(first, second) \
		static char \
		macro_concat(__a_ct_ge_, __COUNTER__)[(first) - (second)] \
		__attribute__((unused))


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
* \brief Two numbers equality compile-time assertion.
*
* Statement checks that given arguments are equal. If check fails
* compilation will be aborted.
* \param first The first value.
* \param second The second value.
*/
#define assert_ct_equal(first, second) \
		__assert_ct_ge(first, second); \
		__assert_ct_ge(second, first)


/**
* \brief Size of type compile-time assertion.
*
* Statement checks that size of given type equals to specified value.
* If check is failed compilation will be aborted.
* \param type Type that should be checked for size.
* \param size Expected size of given type in bytes.
*/
#define assert_type_size(type, size) \
		assert_ct_equal(sizeof(type), (size))


/**
* \brief Aligned for MMU.
*
* Alignes symbol to allow use for MMU-related instructions. Alias to
* __attribute__((aligned(16)))
*/
#define _mmu_aligned_			__attribute__((aligned(16)))


/**
* \brief Aligned for IRQ.
*
* Alignes symbol to allow use for IRQ-related instructions. Alias to
* __attribute__((aligned(16)))
*/
#define _irq_aligned_			__attribute__((aligned(16)))

#define _packed_			__attribute__((packed))

#define _used_				__attribute__((used))

/**
* \brief Link name from assember code.
*
* Macro for marking symbols that are defined in asm code. Actually, it's just
*	extern declaration.
*/
#define _asm_linked_			extern

#endif // __BOLGENOS_NG__COMPILER_H__
