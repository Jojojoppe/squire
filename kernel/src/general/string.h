#ifndef __H_STRING
#define __H_STRING 1

/**
 * @file
 * @section DESCRIPTION
 * string functions
 */

/**
 * @brief Locate first occurrence of character in string
 * @param str C String
 * @param c Character to be located. It is passed as its int promotion, but it is internally converted back to char for the comparison.
 * @returns A pointer to the first occurrence of character in str. If the character is not found, the function returns a null pointer.
 *
 * Returns a pointer to the first occurrence of character in the C string str. The terminating null-character is considered 
 * part of the C string. Therefore, it can also be located in order to retrieve a pointer to the end of a string. 
 */
extern char * strchr(const char * str, int c);

/**
 * @brief Copy string
 * @param dest Pointer to the destination array where the content is to be copied.
 * @param src C string to be copied.
 *
 * Copies the C string pointed by source into the array pointed by destination, including the terminating null character 
 * (and stopping at that point). To avoid overflows, the size of the array pointed by destination shall be long enough to 
 * contain the same C string as source (including the terminating null character), and should not overlap in memory with source.
 */
extern void strcpy(char * dest, const char * src);

/**
 * @brief Get string length
 * @param s C string
 * @returns The lenght of string
 *
 * The length of a C string is determined by the terminating null-character:
 * A C string is as long as the number of characters between the beginning of
 * the string and the terminating null character (without including the
 * terminating null character itself).\n
 * This should not be confused with the size of the array that holds the string.
 * For example:
 * @code
 * char mystr[100]="test string";
 * @endcode
 * defines an array of characters with a size of 100 chars, but the C string with which
 * mystr has been initialized has a length of only 11 characters. Therefore, while
 * sizeof(mystr) evaluates to 100, strlen(mystr) returns 11.
 */
extern unsigned strlen(const char * s);

/**
 * @brief Compare two strings
 * @param s1 C string to be compared.
 * @param s2 C string to be compared.
 * @returns Returns an integral value indicating the relationship between the strings:
 * 				return value|indicates
 * 				------------|---------
 * 				<0			|the first character that does not match has a lower value in ptr1 than in ptr2
 * 				0			|the contents of both strings are equal
 * 				>0			|the first character that does not match has a greater value in ptr1 than in ptr2
 *
 *
 * Compares the C string str1 to the C string str2. This function starts comparing the
 * first character of each string. If they are equal to each other, it continues with
 * the following pairs until the characters differ or until a terminating
 * null-character is reached.
 */
extern int strcmp (const char *s1, const char *s2);

/**
 * @brief Fill block of memory
 * @param ptr Pointer to the block of memory to fill.
 * @param value Value to be set. The value is passed as an int, but the function fills the block of memory using the unsigned char conversion of this value.
 * @param num Number of bytes to be set to the value.
 * @returns ptr is returned
 *
 * Sets the first num bytes of the block of memory pointed by ptr to the specified value (interpreted as an unsigned char).
 */
extern void * memset(void * ptr, int value, unsigned num);

/**
 * @brief Copy block of memory
 * @param dest Pointer to the destination array where the content is to be copied, type-casted to a pointer of type void*.
 * @param src Pointer to the source of data to be copied, type-casted to a pointer of type const void*.
 * @param count Number of bytes to copy.
 * @returns destination is returned
 *
 * Copies the values of num bytes from the location pointed to by source directly to the memory block pointed to by destination.
 * The underlying type of the objects pointed to by both the source and destination pointers are irrelevant for this function; The
 * result is a binary copy of the data. The function does not check for any terminating null character in source - it always copies
 * exactly num bytes.
 */
extern void * memcpy(void *dest, const void *src, unsigned count);

#endif