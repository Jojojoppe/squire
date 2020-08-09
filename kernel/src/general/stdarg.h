#ifndef __H_STDARG
#define __H_STDARG 1
/**
 * @file
 * @section DESCRIPTION
 * libc stdarg
 */

/**
 * @brief type for iterating arguments
 *
 * va_list is a complete object type suitable for holding the information needed by the macros va_start, va_copy, va_arg, and va_end.
 * If a va_list instance is created, passed to another function, and used via va_arg in that function, then any subsequent use in the calling function should be preceded by a call to va_end.
 * It is legal to pass a pointer to a va_list object to another function and then use that object after the function returns.
 */
typedef char * va_list;

/** @cond private */
#define	STACKITEM	int

/* round up width of objects pushed on stack. The expression before the
& ensures that we get 0 for objects of size 0. */
#define	VA_SIZE(TYPE)					\
	((sizeof(TYPE) + sizeof(STACKITEM) - 1)	\
		& ~(sizeof(STACKITEM) - 1))
/** @endcond */



/**
 * @brief Start iterating arguments with a va_list
 * @param AP an instance of the va_list type
 * @param LASTARG the named parameter preceding the first variable parameter
 *
 * The va_start macro enables access to the variable arguments following the named argument parm_n.
 * va_start should be invoked with an instance to a valid va_list object ap before any calls to va_arg.
 * If parm_n is declared with reference type or with a type not compatible with the type that results from default argument promotions, the behavior is undefined.
 */
#define	va_start(AP, LASTARG)	\
	(AP=((va_list)&(LASTARG) + VA_SIZE(LASTARG)))


/**
 * @brief Free a va_list
 * @param AP an instance of the va_list type to clean up
 *
 * The va_end macro performs cleanup for an ap object initialized by a call to va_start or va_copy. va_end may modify ap so that it is no longer usable.
 * If there is no corresponding call to va_start or va_copy, or if va_end is not called before a function that calls va_start or va_copy returns, the behavior is undefined.
 */
#define va_end(AP)

/**
 * @brief Retrieve an argument
 * @param AP an instance of the va_list type
 * @param TYPE the type of the next parameter in ap
 *
 * The va_arg macro expands to an expression of type T that corresponds to the next parameter from the va_list ap.
 * Prior to calling va_arg, ap must be initialized by a call to either va_start or va_copy, with no intervening call to va_end. Each invocation of the va_arg macro modifies ap to point to the next variable argument.
 * If va_arg is called when there are no more arguments in ap, or if the type of the next argument in ap (after promotions) is not compatible with T, the behavior is undefined, unless:
 * - one type is a signed integer type, the other type is the corresponding unsigned integer type, and the value is representable in both types; or
 * - one type is pointer to void and the other is a pointer to a character type (char, signed char, or unsigned char).
 */
#define va_arg(AP, TYPE)	\
	(AP += VA_SIZE(TYPE), *((TYPE *)(AP - VA_SIZE(TYPE))))

#endif