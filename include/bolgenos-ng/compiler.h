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


#endif // __BOLGENOS_NG__COMPILER_H__
