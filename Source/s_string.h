/*============================================================================*/
/*
 Copyright (c) 2025, Isaac Marino Bavaresco
 All rights reserved.

 Redistribution and use in source and binary forms, with or without
 modification, are permitted provided that the following conditions are met:
	 * Redistributions of source code must retain the above copyright
	   notice, this list of conditions and the following disclaimer.
	 * Neither the name of the author nor the
	   names of its contributors may be used to endorse or promote products
	   derived from this software without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE AUTHOR ''AS IS'' AND ANY
 EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 DISCLAIMED. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
/*============================================================================*/
#if         !defined __S_STRING_H__
#define __S_STRING_H__
/*============================================================================*/
#include <stdint.h>
/*=========================================================================*//**
\brief          A data type to represent the s_string objects. It is an opaque
				data type, so the user's code cannot access its internals
				directly, only through the functions of the library.
*//*==========================================================================*/
typedef struct s_string s_string_t;
/*=========================================================================*//**
\brief          Creates on the stack (auto/local variable) an s_string object
				capable of holding up to \a maxsize characters. Can only be used
				inside a function. The resulting s_string starts empty.
\param name     Name of the variable.
\param maxsize  Maximum number of characters that the object will be able to
				hold.
*//*==========================================================================*/
#define auto_s_string( name, maxsize )                      uint8_t _##name##_buffer[_s_calcsize(maxsize)]; \
                                                            s_string_t * const name = (s_string_t*)_##name##_buffer; _s_string_init(name,maxsize,1)
/*=========================================================================*//**
\brief          Creates on the stack (auto/local variable) an s_string object
				capable of holding up to \a maxsize characters, with initial
				value \a src. Can only be used inside a function.
\param	name    Name of the variable.
\param	maxsize Maximum number of characters that the object will be able to
				store.
\param	src		A C-string (null terminated array of characters) that will be
				the initial value of the s_string.
*//*==========================================================================*/
#define auto_s_string_i( name, maxsize, src )               uint8_t _##name##_buffer[_s_calcsize(maxsize)]; \
                                                            s_string_t * const name = (s_string_t*)_##name##_buffer; _s_string_init(name,maxsize,1); \
                                                            s_strcpy_c(name,src)
/*=========================================================================*//**
\brief          Creates a static (static local or file scope variable) s_string
				object capable of holding up to \a maxsize characters. \a
				maxsize can be at most 254 characters. The resulting string is
				initialized with constant \a src.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
\param	src		A C-string (null terminated array of characters) that will be
				the initial value of the s_string.
*//*==========================================================================*/
#define static_s_string_tiny_i( name, maxsize, src )        static uint8_t _##name##_buffer[1+1+1+maxsize+1]; \
                                                            static s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"),used)) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x21 ), _##name##_buffer, src };
/*=========================================================================*//**
\brief          Creates a static (static local or file scope variable) s_string
				object capable of holding up to \a maxsize characters. \a
				maxsize can be at most 254 characters. The resulting string
				starts empty.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
*//*==========================================================================*/
#define static_s_string_tiny( name, maxsize )               static uint8_t _##name##_buffer[1+1+1+maxsize+1]; \
                                                            static s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"),used)) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x22 ), _##name##_buffer };
/*=========================================================================*//**
\brief          Creates a static (static local or file scope variable) s_string
				object capable of holding up to \a maxsize characters. \a
				maxsize can be at most 65534 characters. The resulting string is
				initialized with constant \a src.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
\param	src		A C-string (null terminated array of characters) that will be
				the initial value of the s_string.
*//*==========================================================================*/
#define static_s_string_small_i( name, maxsize, src )       static uint8_t _##name##_buffer[1+2+2+maxsize+1]; \
                                                            static s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"),used)) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x23 ), _##name##_buffer, src };
/*=========================================================================*//**
\brief          Creates a static (static local or file scope variable) s_string
				object capable of holding up to \a maxsize characters. \a
				maxsize can be at most 65534 characters. The resulting string
				starts empty.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
*//*==========================================================================*/
#define static_s_string_small( name, maxsize )              static uint8_t _##name##_buffer[1+2+2+maxsize+1]; \
                                                            static s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"),used)) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x24 ), _##name##_buffer };
/*=========================================================================*//**
\brief          Creates a static (static local or file scope variable) s_string
				object capable of holding up to \a maxsize characters. \a
				maxsize can be at most 4294967284 characters. The resulting
				string is initialized with constant \a src.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
*//*==========================================================================*/
#define static_s_string_large_i( name, maxsize, src )       static uint8_t _##name##_buffer[1+4+4+maxsize+1]; \
                                                            static s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"),used)) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x25 ), _##name##_buffer, src };
/*=========================================================================*//**
\brief          Creates a static (static local or file scope variable) s_string
				object capable of holding up to \a maxsize characters. \a
				maxsize can be at most 4294967284 characters. The resulting
				string starts empty.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
*//*==========================================================================*/
#define static_s_string_large( name, maxsize )              static uint8_t _##name##_buffer[1+4+4+maxsize+1]; \
                                                            static s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"),used)) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x26 ), _##name##_buffer };
/*=========================================================================*//**
\brief          Creates a static (static local or file scope variable) s_string
				object capable of holding up to \a maxsize characters. \a
				maxsize can be at most 254 characters. The resulting string is
				initialized with constant \a src.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
\param	src		A C-string (null terminated array of characters) that will be
				the initial value of the s_string.
*//*==========================================================================*/
#define global_s_string_tiny_i( name, maxsize, src )        static uint8_t _##name##_buffer[1+1+1+maxsize+1]; \
                                                            s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"),used)) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x21 ), _##name##_buffer, src };
/*=========================================================================*//**
\brief          Creates a global file scope s_string object capable of holding
                up to \a maxsize characters. \a maxsize can be at most 254
                characters. The resulting string starts empty.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
*//*==========================================================================*/
#define global_s_string_tiny( name, maxsize )               static uint8_t _##name##_buffer[1+1+1+maxsize+1]; \
                                                            s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"),used)) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x22 ), _##name##_buffer };
/*=========================================================================*//**
\brief          Creates a global file scope s_string object capable of holding
                up to \a maxsize characters. \a maxsize can be at most 65534
                characters. The resulting string is initialized with constant \a
                src.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
\param	src		A C-string (null terminated array of characters) that will be
				the initial value of the s_string.
*//*==========================================================================*/
#define global_s_string_small_i( name, maxsize, src )       static uint8_t _##name##_buffer[1+2+2+maxsize+1]; \
                                                            s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"))) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x23 ), _##name##_buffer, src };
/*=========================================================================*//**
\brief          Creates a global file scope s_string object capable of holding
                up to \a maxsize characters. \a maxsize can be at most 65534
                characters. The resulting string starts empty.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
*//*==========================================================================*/
#define global_s_string_small( name, maxsize )              static uint8_t _##name##_buffer[1+2+2+maxsize+1]; \
                                                            s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"))) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x24 ), _##name##_buffer };
/*=========================================================================*//**
\brief          Creates a GLOBAL file scope s_string object capable of holding
                up to \a maxsize characters. \a maxsize can be at most
                4294967284 characters. The resulting string is initialized with
                constant \a src.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
*//*==========================================================================*/
#define global_s_string_large_i( name, maxsize, src )       static uint8_t _##name##_buffer[1+4+4+maxsize+1]; \
                                                            s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"))) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x25 ), _##name##_buffer, src };
/*=========================================================================*//**
\brief          Creates a global file scope s_string object capable of holding
                up to \a maxsize characters. \a maxsize can be at most
                4294967284 characters. The resulting string starts empty.
\param	name    The name of the variable.
\param	maxsize The maximum number of characters that the object will be able to
				store.
*//*==========================================================================*/
#define global_s_string_large( name, maxsize )              static uint8_t _##name##_buffer[1+4+4+maxsize+1]; \
                                                            s_string_t * const name = (s_string_t*)_##name##_buffer; \
                                                            static void * __attribute__((section("s_string_init"))) _##name##_init[]   = { (void*)(( maxsize << 8 ) | 0x26 ), _##name##_buffer };
/*=========================================================================*//**
\brief          Returns the smaller of two ssize_t (signed) values.
\param a		The first value.
\param b		The second value.
*//*==========================================================================*/
static inline ssize_t __attribute__((always_inline)) ssmin( ssize_t a, ssize_t b )
    {
    return a < b ? a : b;
    }
/*=========================================================================*//**
\brief          Returns the greater of two ssize_t (signed) values.
\param a		The first value.
\param b		The second value.
*//*==========================================================================*/
static inline ssize_t __attribute__((always_inline)) ssmax( ssize_t a, ssize_t b )
    {
    return a > b ? a : b;
    }
/*=========================================================================*//**
\brief          Calculates the size in bytes required to hold an s_string object
				capable of storing up to \a len characters. This function is not
				intended to be called directly by the programmer's code.
\param len      The maximum number of characters that the object will be able to
				store.
\returns        The number of bytes required to hold the s_string object.
*//*==========================================================================*/
size_t       _s_calcsize    ( size_t len );
/*=========================================================================*//**
\brief          Initializes an s_string object right after creation. This
				function is not intended to be called directly by the
				programmer's code.
\param str      Pointer to the s_string to be initialized.
\param len      The maximum number of characters that the object will be able to
				store.
\param area     The memory area in what the string will be created. 0 = 'data'
				segment (global or static variables), 1 = 'stack' (local/
				automatic variables), 2 = 'heap' (dynamically created object).
*//*==========================================================================*/
void         _s_string_init (       s_string_t * restrict str, size_t len, int area );
/*=========================================================================*//**
\brief          Returns non-zero if the s_string \a str cannot be changed.
\param str      Pointer to the s_string.
\returns        Zero if the s_string can be changed, a positive value if it
				cannot be changed and a negative value if there is an error (it
				is a NULL pointer, for instance).
*//*==========================================================================*/
int          s_isreadonly   ( const s_string_t * restrict str );
/*=========================================================================*//**
\brief          Returns the maximum number of characters that the s_string \a
				str can hold.
\param str      Pointer to the s_string.
\returns        The maximum length of s_string \a str.
*//*==========================================================================*/
size_t       s_strmaxlen    ( const s_string_t * restrict str );
/*=========================================================================*//**
\brief          Returns the number of characters that the s_string \a str is
				holding.
\param str      Pointer to the s_string.
\returns        The length of the s_string \a str.
*//*==========================================================================*/
size_t       s_strlen       ( const s_string_t * restrict str );
/*=========================================================================*//**
\brief          Returns the lesser between \a len and the number of characters
				that the s_string \a str is holding.
\param str      Pointer to the s_string.
\param len      The maximum value that the function must return, in case the
				length of \a str is longer.
\returns        The length of the s_string \a str.
*//*==========================================================================*/
size_t       s_strnlen      ( const s_string_t * restrict str, size_t len );
/*=========================================================================*//**
\brief          Finds the first occurrence of character \a c in the s_string \a
				str starting from the position \a start towards the end of the
				string (forward search).
\param str      Pointer to the s_string.
\param c		The value of the character to be searched.
\param start	The index at which the search is to begin.
\returns        The index of the first occurrence of character \a c if found, or
				-1 if the character is not present in the string.
*//*==========================================================================*/
ssize_t      s_strchr       ( const s_string_t * restrict str, int c, ssize_t start );
/*=========================================================================*//**
\brief          Finds the first occurrence of character \a c in the s_string \a
				str starting from the position \a start towards the beginning of
				the string (backwards search).
\param str      Pointer to the s_string.
\param c		The value of the character to be searched.
\param start	The index at which the search is to begin.
\returns        The index of the first occurrence of character \a c if found, or
				-1 if the character is not present in the string.
*//*==========================================================================*/
ssize_t      s_strrchr      ( const s_string_t * restrict str, int c, ssize_t start );
/*=========================================================================*//**
\brief          Finds the first occurrence of character \a c in the s_string \a
				str starting from the position \a start towards the end of the
				string (forward search).
\param str      Pointer to the s_string.
\param c		The value of the character to be searched.
\param start	The index at which the search is to begin.
\returns        The index of the first occurrence of character \a c if found, or
				-1 if the character is not present in the string.
*//*==========================================================================*/
ssize_t      s_strichr      ( const s_string_t * restrict str, int c, ssize_t start );
/*=========================================================================*//**
\brief          Finds the first occurrence of character \a c in the s_string \a
				str starting from the position \a start towards the beginning of
				the string (backwards search).
\param str      Pointer to the s_string.
\param c		The value of the character to be searched.
\param start	The index at which the search is to begin.
\returns        The index of the first occurrence of character \a c if found, or
				-1 if the character is not present in the string.
*//*==========================================================================*/
ssize_t      s_strrichr     ( const s_string_t * restrict str, int c, ssize_t start );
/*=========================================================================*//**
\brief          Finds the first occurrence of sub-string \a src (s_string) in
				the s_string \a str starting from the position \a start towards
				the end of \a str (forward search).
\param str      Pointer to the s_string that will be scanned to find \a src.
\param src      Pointer to the s_string that will be looked for.
\param start	The index at which the search is to begin.
\returns        The index of the start of first occurrence of \a src if found,
				or -1 if \a src is not present in the string.
*//*==========================================================================*/
ssize_t      s_strstr       ( const s_string_t * restrict str, const s_string_t * restrict src,     ssize_t start );
/*=========================================================================*//**
\brief          Finds the first occurrence of sub-string \a src (C-string) in
				the s_string \a str starting from the position \a start towards
				the end of \a str (forward search).
\param str      Pointer to the s_string that will be scanned to find \a src.
\param src      Pointer to the s_string that will be looked for.
\param start	The index at which the search is to begin.
\returns        The index of the start of first occurrence of \a src if found,
				or -1 if \a src is not present in the string.
*//*==========================================================================*/
ssize_t      s_strstr_c     ( const s_string_t * restrict str, const char       * restrict src,     ssize_t start );
/*=========================================================================*//**
\brief          Finds the first occurrence of sub-string \a src (s_string) in
				the s_string \a str starting from the position \a start towards
				the end of \a str (forward search), ignoring the case of the
				characters.
\param str      Pointer to the s_string that will be scanned to find \a src.
\param src      Pointer to the s_string that will be looked for.
\param start	The index at which the search is to begin.
\returns        The index of the start of first occurrence of \a src if found,
				or -1 if \a src is not present in the string.
*//*==========================================================================*/
ssize_t      c_strstr_s     ( const char       * restrict str, const s_string_t * restrict src,     ssize_t start );
/*=========================================================================*//**
\brief          Finds the first occurrence of sub-string \a src (s_string) in
				the s_string \a str starting from the position \a start towards
				the end of \a str (forward search), ignoring the case of the
				characters.
\param str      Pointer to the s_string that will be scanned to find \a src.
\param src      Pointer to the s_string that will be looked for.
\param start	The index at which the search is to begin.
\returns        The index of the start of first occurrence of \a src if found,
				or -1 if \a src is not present in the string.
*//*==========================================================================*/
ssize_t      s_stristr      ( const s_string_t * restrict str, const s_string_t * restrict src,     ssize_t start );
/*=========================================================================*//**
\brief          Finds the first occurrence of sub-string \a src (C-string) in
				the s_string \a str starting from the position \a start towards
				the end of \a str (forward search), ignoring the case of the
				characters.
\param str      Pointer to the s_string that will be scanned to find \a src.
\param src      Pointer to the s_string that will be looked for.
\param start	The index at which the search is to begin.
\returns        The index of the start of first occurrence of \a src if found,
				or -1 if \a src is not present in the string.
*//*==========================================================================*/
ssize_t      s_stristr_c    ( const s_string_t * restrict str, const char       * restrict src,     ssize_t start );
/*=========================================================================*//**
\brief          Finds the first occurrence of sub-string \a src (s_string) in
				the C-string \a str starting from the position \a start towards
				the end of \a str (forward search), ignoring the case of the
				characters.
\param str      Pointer to the C-string that will be scanned to find \a src.
\param src      Pointer to the s_string that will be looked for.
\param start	The index at which the search is to begin.
\returns        The index of the start of first occurrence of \a src if found,
				or -1 if \a src is not present in the string.
*//*==========================================================================*/
ssize_t      c_stristr_s    ( const char       * restrict str, const s_string_t * restrict src,     ssize_t start );
/*=========================================================================*//**
\brief			Returns the position of the first occurrence of any character
				from the s_string \a charset in the s_string \a str, or -1 if
				the two strings have no characters in common.
\param str      Pointer to the s_string that will be scanned to find any
				characters from \a charset.
\param charset  Pointer to the s_string that contains the set of characters to
				be found in \a str.
\param start	The index at which the search is to begin.
\returns        The index of the start of first occurrence of \a src if found,
				or -1 if \a src is not present in the string.
*//*==========================================================================*/
ssize_t      s_strpbrk      ( const s_string_t * restrict str, const s_string_t * restrict charset, ssize_t start );
/*=========================================================================*//**
\brief			Returns the position of the first occurrence of any character
				from the C-string \a charset in the s_string \a str, or -1 if
				the two strings have no characters in common.
\param str      Pointer to the s_string that will be scanned to find any
				characters from \a charset.
\param charset  Pointer to the C-string that contains the set of characters to
				be found in \a str.
\param start	The index at which the search is to begin.
\returns        The index of the start of first occurrence of \a src if found,
				or -1 if \a src is not present in the string.
*//*==========================================================================*/
ssize_t      s_strpbrk_c    ( const s_string_t * restrict str, const char       * restrict charset, ssize_t start );
/*=========================================================================*//**
\brief          Returns the value of the character at position \a index of
				s_string \a str.
\param str      Pointer to the s_string.
\param index	The position of the character to be read from \a str. If \a
				index is negative, it is counted from the end of the string
				towards the beginning. -1 is the index of the last character of
				\a str.
\returns        The value of the character at position \a index, or -1 if an
				error occurred or the position does not exist.
*//*==========================================================================*/
int          s_charat       ( const s_string_t * restrict str, ssize_t index );
/*=========================================================================*//**
\brief          Returns a 'const char*' to the C-string that holds the
				characters in s_string \a str. This pointer can be passed to the
				standard C library functions.
\param str      Pointer to the s_string.
\returns        Pointer to the C-string. Can be NULL.
*//*==========================================================================*/
const char  *s_constcstr    ( const s_string_t * restrict str );
/*=========================================================================*//**
\brief          Replaces a single character of s_string \a dst at position \a
				index with the character in \a value.
\param dst      Pointer to the s_string to be changed.
\param index    Position at which the character is to be inserted. \a index can
				point to a character beyond the current length of the string,
				but it must not be beyond the maximum length the string can
				hold. If \a index is beyond the current length, the bytes
				between the current end of the string and the newly inserted
				character are filled with \a filler. \a index can be negative,
				in which case it is counted back from the end. index -1 is the
				last character of the string.
\param value    Character to be inserted into \a dst.
\param filler   Character used to fill any extra space created by inserting a
				character beyond the current length of the string.
\returns        The new length of the string in case of success or a negative
				value in case of failure.
*//*==========================================================================*/
ssize_t      s_setcharat    (       s_string_t * restrict dst, ssize_t index, int value, int filler );
/*=========================================================================*//**
\brief          Appends a single character to the end of string \a dst. If \a
				dst is already at its maximum length, the operation will be
				ignored.
\param dst      Pointer to the s_string to receive the character in \a value.
\param value    Character to be appended to \a dst.
\returns        The new length of \a dst after the character \a value is
				appended or a negative value if an error occurred.
*//*==========================================================================*/
ssize_t      s_appendchar   (       s_string_t * restrict dst, int value );
/*=========================================================================*//**
\brief          Truncates the length of string \a dst to at most \a len
				characters.
\param dst      Pointer to the s_string to be truncated.
\param len      The new length of s_string \a dst.
\returns        The new length of \a dst after the truncation or a negative
				value if an error occurred.
*//*==========================================================================*/
ssize_t      s_truncate     (       s_string_t * restrict dst, size_t len );
/*=========================================================================*//**
\brief  		Appends the contents of the s_string pointed to by \a src to the
				end of the s_string pointed to by \a dst. In case the resulting
				string would be longer than the maximum length \a dst can hold,
				only enough characters to fill \a dst will be copied.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source s_string.
\returns		The new length of \a dst after the appending or a negative value
				if an error occurred.
*//*==========================================================================*/
int          s_strcat       (       s_string_t * restrict dst, const s_string_t * restrict src );
/*=========================================================================*//**
\brief  		Appends the contents of the C-string pointed to by \a src to the
				end of the s_string pointed to by \a dst. In case the resulting
				string would be longer than the maximum length \a dst can hold,
				only enough characters to fill \a dst will be copied.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source C-string.
\returns		The new length of \a dst after the appending or a negative value
				if an error occurred.
*//*==========================================================================*/
int          s_strcat_c     (       s_string_t * restrict dst, const char       * restrict src );
/*=========================================================================*//**
\brief  		Appends characters from the beginning of the s_string pointed to
				by \a src to the end of the s_string pointed to by \a dst to
				make \a dst up to \a len characters long. If \a dst is alredy \a
				len characters long or more, no characters will be copied. If \a
				src does not contain enough characters then \a dst will not
				reach length \a len.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source s_string.
\param	len		The length \a dst should have after the append.
\returns		The number of characters stored in \a dst or a negative number
				in case of error.
*//*==========================================================================*/
int          s_strlcat      (       s_string_t * restrict dst, const s_string_t * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Appends characters from the beginning of the C-string pointed to
				by \a src to the end of the s_string pointed to by \a dst to
				make \a dst up to \a len characters long. If \a dst is alredy \a
				len characters long or more, no characters will be copied. If \a
				src does not contain enough characters then \a dst will not
				reach length \a len.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source C-string.
\param	len		The length \a dst should have after the append.
\returns		The number of characters stored in \a dst or a negative number
				in case of error.
*//*==========================================================================*/
int          s_strlcat_c    (       s_string_t * restrict dst, const char       * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Appends up to \a len characters from the beginning of the
				s_string pointed to by \a src to the end of the s_string pointed
				to by \a dst.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source s_string.
\param	len		The maximum number of characters to copy from \a src to \a dst.
\returns		The number of characters stored in \a dst or a negative number
				in case of error.
*//*==========================================================================*/
int          s_strncat      (       s_string_t * restrict dst, const s_string_t * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Appends up to \a len characters from the beginning of the
				C-string pointed to by \a src to the end of the s_string pointed
				to by \a dst.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source C-string.
\param	len		The maximum number of characters to copy from \a src to \a dst.
\returns		The number of characters stored in \a dst or a negative number
				in case of error.
*//*==========================================================================*/
int          s_strncat_c    (       s_string_t * restrict dst, const char       * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Copies the contents of the s_string pointed to by \a src into
				the s_string pointed to by \a dst. The previous content of \a
				dst is lost.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source s_string.
\returns		The number of characters stored in \a dst or a negative number
				in case of error.
*//*==========================================================================*/
int          s_strcpy       (       s_string_t * restrict dst, const s_string_t * restrict src );
/*=========================================================================*//**
\brief  		Copies the contents of the C-string pointed to by \a src into
				the s_string pointed to by \a dst. The previous content of \a
				dst is lost.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source C-string (null character terminated).
\returns		The number of characters stored in \a dst or a negative number
				in case of error.
*//*==========================================================================*/
int          s_strcpy_c     (       s_string_t * restrict dst, const char       * restrict src );
/*=========================================================================*//**
\brief  		Copies up to \a len characters of the contents of the s_string
				pointed to by \a src into the s_string pointed to by \a dst. The
				previous content of \a dst is lost.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source s_string.
\param	len		The maximum number of characters to copy from \a src to \a dst.
\returns		The number of characters stored in \a dst or a negative number
				in case of error.
*//*==========================================================================*/
int          s_strlcpy      (       s_string_t * restrict dst, const s_string_t * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Copies up to \a len characters of the contents of the C-string
				pointed to by \a src into the s_string pointed to by \a dst. The
				previous content of \a dst is lost.
\param	dst		Pointer to the destination s_string.
\param	src		Pointer to the source C-string.
\param	len		The maximum number of characters to copy from \a src to \a dst.
\returns		The number of characters stored in \a dst or a negative number
				in case of error.
*//*==========================================================================*/
int          s_strlcpy_c    (       s_string_t * restrict dst, const char       * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Compares two s_strings \a dst and \a src character by character.
\param	dst		The first s_string to be compared.
\param	src		The second s_string to be compared.
\returns		A negative value if \a dst is lexicographically before \a src,
				zero if both are equal and positive if \a dst is
				lexicographically after \a src.
*//*==========================================================================*/
int          s_strcmp       ( const s_string_t * restrict dst, const s_string_t * restrict src );
/*=========================================================================*//**
\brief  		Compares the s_string \a dst with the C-string \a src character
				by character.
\param	dst		The first string to be compared.
\param	src		The second string to be compared.
\returns		A negative value if \a dst is lexicographically before \a src,
				zero if both are equal and positive if \a dst is
				lexicographically after \a src.
*//*==========================================================================*/
int          s_strcmp_c     ( const s_string_t * restrict dst, const char       * restrict src );
/*=========================================================================*//**
\brief  		Compares two s_strings \a dst and \a src character by character
				ignoring the case.
\param	dst		The first s_string to be compared.
\param	src		The second s_string to be compared.
\returns		A negative value if \a dst is lexicographically before \a src,
				zero if both are equal and positive if \a dst is
				lexicographically after \a src.
*//*==========================================================================*/
int          s_stricmp      ( const s_string_t * restrict dst, const s_string_t * restrict src );
/*=========================================================================*//**
\brief  		Compares the s_string \a dst with the C-string \a src character
				by character ignoring the case.
\param	dst		The first string to be compared.
\param	src		The second string to be compared.
\returns		A negative value if \a dst is lexicographically before \a src,
				zero if both are equal and positive if \a dst is
				lexicographically after \a src.
*//*==========================================================================*/
int          s_stricmp_c    ( const s_string_t * restrict dst, const char       * restrict src );
/*=========================================================================*//**
\brief  		Compares up to \a len characters of two s_strings \a dst and \a
				src character by character.
\param	dst		The first s_string to be compared.
\param	src		The second s_string to be compared.
\param	len		The maximum number of characters to compare.
\returns		A negative value if \a dst is lexicographically before \a src,
				zero if both are equal and positive if \a dst is
				lexicographically after \a src.
*//*==========================================================================*/
int          s_strncmp      ( const s_string_t * restrict dst, const s_string_t * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Compares up to \a len characters of an s_string \a dst and a
				C-string \a src character by character.
\param	dst		The first string to be compared.
\param	src		The second string to be compared.
\param	len		The maximum number of characters to compare.
\returns		A negative value if \a dst is lexicographically before \a src,
				zero if both are equal, and positive if \a dst is
				lexicographically after \a src.
*//*==========================================================================*/
int          s_strncmp_c    ( const s_string_t * restrict dst, const char       * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Compares up to \a len characters of two s_strings \a dst and \a
				src character by character ignoring the case.
\param	dst		The first s_string to be compared.
\param	src		The second s_string to be compared.
\param	len		The maximum number of characters to compare.
\returns		A negative value if \a dst is lexicographically before \a src,
				zero if both are equal and positive if \a dst is
				lexicographically after \a src.
*//*==========================================================================*/
int          s_strnicmp     ( const s_string_t * restrict dst, const s_string_t * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Compares up to \a len characters of an s_string \a dst and a
				C-string \a src character by character ignoring the case.
\param	dst		The first string to be compared.
\param	src		The second string to be compared.
\param	len		The maximum number of characters to compare.
\returns		A negative value if \a dst is lexicographically before \a src,
				zero if both are equal, and positive if \a dst is
				lexicographically after \a src.
*//*==========================================================================*/
int          s_strnicmp_c   ( const s_string_t * restrict dst, const char       * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Returns the position of the first character in an s_string \a
				str that doesn't belong to a specified set of characters \a
				charset.
\param	str		Pointer to the s_string that will be scanned to find any
				characters from \a charset.
\param	charset	Pointer to the s_string that contains the set of characters to
				be found in \a str.
\param	start	The index at which the search is to begin.
\returns		The length of the portion at the beginning of \a str that is
				comprised only of characters in \a charset, or a negative value
				if there is an error.
*//*==========================================================================*/
ssize_t      s_strspn       ( const s_string_t * restrict str, const s_string_t * restrict charset, int start );
/*=========================================================================*//**
\brief  		Returns the position of the first character in an s_string \a
				str that doesn't belong to a specified set of characters \a
				charset.
\param	str		Pointer to the s_string that will be scanned to find any
				characters from \a charset.
\param	charset	Pointer to the C-string that contains the set of characters to
				be found in \a str.
\param	start	The index at which the search is to begin.
\returns		The length of the portion at the beginning of \a str that is
				comprised only of characters in \a charset, or a negative value
				if there is an error.
*//*==========================================================================*/
ssize_t      s_strspn_c     ( const s_string_t * restrict str, const char       * restrict charset, int start );
/*=========================================================================*//**
\brief  		Returns the position of the first character in an s_string \a
				str that belongs to a specified set of characters \a charset.
\param	str		Pointer to the s_string that will be scanned to find any
				characters from \a charset.
\param	charset	Pointer to the s_string that contains the set of characters to
				be found in \a str.
\param	start	The index at which the search is to begin.
\returns		The length of the portion at the beginning of \a str that has
				none of the characters in \a charset, or a negative value if
				there is an error.
*//*==========================================================================*/
ssize_t      s_strcspn      ( const s_string_t * restrict str, const s_string_t * restrict charset, int start );
/*=========================================================================*//**
\brief  		Returns the position of the first character in an s_string \a
				str that belongs to a specified set of characters \a charset.
\param	str		Pointer to the s_string that will be scanned to find any
				characters from \a charset.
\param	charset	Pointer to the C-string that contains the set of characters to
				be found in \a str.
\param	start	The index at which the search is to start.
\returns		The length of the portion at the beginning of \a str that has
				none of the characters in \a charset, or a negative value if
				there is an error.
*//*==========================================================================*/
ssize_t      s_strcspn_c    ( const s_string_t * restrict str, const char       * restrict charset, int start );
/*=========================================================================*//**
\brief  		Finds the next token in the s_string \a str delimited by
				characters from the s_string \a delim, starting the search from
				the position \a start.
\param	str		Pointer to the s_string that will be scanned to find the token.
\param	delim	Pointer to the s_string that contains the set of characters to
				be used as delimiters. The set of delimiters can vary from one
				call to another.
\param	length	Pointer to an ssize_t variable to receive the length of the
				token just found. It can be NULL, in which case the length will
				not be provided.
\param	start	Pointer to an ssize_t variable that contains the index where the
				search is to start. This variable will be updated with a value
				that can be used as the start for the next search, skipping the
				token just found.
\returns		The index of the character in \a str where the token begins, or
				a -1 if no token was found.
*//*==========================================================================*/
ssize_t      s_strtok       ( const s_string_t * restrict str, const s_string_t * restrict delim, ssize_t * restrict length, ssize_t * restrict start );
/*=========================================================================*//**
\brief  		Finds the next token in the s_string \a str delimited by
				characters from the C-string \a delim, starting the search from
				the position \a start.
\param	str		Pointer to the s_string that will be scanned to find the token.
\param	delim	Pointer to the C-string that contains the set of characters to
				be used as delimiters. The set of delimiters can vary from one
				call to another.
\param	length	Pointer to an ssize_t variable to receive the length of the
				token just found. It can be NULL, in which case the length will
				not be provided.
\param	start	Pointer to an ssize_t variable that contains the index where the
				search is to start. This variable will be updated with a value
				that can be used as the start for the next search, skipping the
				token just found.
\returns		The index of the character in \a str where the token begins, or
				a -1 if no token was found.
*//*==========================================================================*/
ssize_t      s_strtok_c     ( const s_string_t * restrict str, const char       * restrict delim, ssize_t * restrict length, ssize_t * restrict start );
/*=========================================================================*//**
\brief  		Finds the next token in the s_string \a str delimited by
				characters from the s_string \a delim, starting the search from
				the position \a start, and copies it to the s_string \a dst.
\param	dst		Pointer to the s_string that will receive the token found.
\param	str		Pointer to the s_string that will be scanned to find the token.
\param	delim	Pointer to the s_string that contains the set of characters to
				be used as delimiters. The set of delimiters can vary from one
				call to another.
\param	start	Pointer to an ssize_t variable that contains the index where the
				search is to start. This variable will be updated with a value
				that can be used as the start for the next search, skipping the
				token just found.
\returns		The index of the character in \a str where the token begins, or
				a -1 if no token was found.
*//*==========================================================================*/
ssize_t      s_strtok_s     (       s_string_t * restrict dst, const s_string_t * restrict str, const s_string_t * restrict delim, ssize_t * restrict start );
/*=========================================================================*//**
\brief  		Finds the next token in the s_string \a str delimited by
				characters from the C-string \a delim, starting the search from
				the position \a start, and copies it to the s_string \a dst.
\param	dst		Pointer to the s_string that will receive the token found.
\param	str		Pointer to the s_string that will be scanned to find the token.
\param	delim	Pointer to the C-string that contains the set of characters to
				be used as delimiters. The set of delimiters can vary from one
				call to another.
\param	start	Pointer to an ssize_t variable that contains the index where the
				search is to start. This variable will be updated with a value
				that can be used as the start for the next search, skipping the
				token just found.
\returns		The index of the character in \a str where the token begins, or
				a -1 if no token was found.
*//*==========================================================================*/
ssize_t      s_strtok_sc    (       s_string_t * restrict dst, const s_string_t * restrict str, const char       * restrict delim, ssize_t * restrict start );
/*=========================================================================*//**
\brief  		Finds the next token in the s_string \a str delimited by
				characters from the s_string \a delim, starting the search from
				the position \a start, and creates a new dynamically allocated
				s_string object with the token just found.
\param	str		Pointer to the s_string that will be scanned to find the token.
\param	delim	Pointer to the s_string that contains the set of characters to
				be used as delimiters. The set of delimiters can vary from one
				call to another.
\param	start	Pointer to an ssize_t variable that contains the index where the
				search is to start. This variable will be updated with a value
				that can be used as the start for the next search, skipping the
				token just found.
\returns		A pointer to the new s_string object that contains the token
				that has just been found.
*//*==========================================================================*/
s_string_t  *s_strtok_m     ( const s_string_t * restrict str, const s_string_t * restrict delim, ssize_t * restrict start );
/*=========================================================================*//**
\brief  		Finds the next token in the s_string \a str delimited by
				characters from the C-string \a delim, starting the search from
				the position \a start, and creates a new dynamically allocated
				s_string object with the token just found.
\param	str		Pointer to the s_string that will be scanned to find the token.
\param	delim	Pointer to the C-string that contains the set of characters to
				be used as delimiters. The set of delimiters can vary from one
				call to another.
\param	start	Pointer to an ssize_t variable that contains the index where the
				search is to start. This variable will be updated with a value
				that can be used as the start for the next search, skipping the
				token just found.
\returns		A pointer to the new s_string object that contains the token
				that has just been found.
*//*==========================================================================*/
s_string_t  *s_strtok_mc    ( const s_string_t * restrict str, const char       * restrict delim, ssize_t * restrict start );
/*=========================================================================*//**
\brief  		Creates in the heap a new dynamically allocated s_string object
				and copies to it the character sequence from the s_string \a src
				located between indexes \a start and \a end, inclusive.
\param	src		Pointer to the s_string that will be duplicated.
\param	start	The index of the first character from \a src that will be copied
				to the new s_string.
\param	end		The index of the last character from \a src that will be copied
				to the new s_string.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the character sequence from the s_string \a
				src. The new object must be freed with 'free' when not in use
				anymore.
*//*==========================================================================*/
s_string_t  *s_extract_me   ( const s_string_t * restrict src, ssize_t start, ssize_t end );
/*=========================================================================*//**
\brief  		Creates in the heap a new dynamically allocated s_string object
				and copies to it the character sequence from the C-string \a src
				located between indexes \a start and \a end, inclusive.
\param	src		Pointer to the C-string that will be duplicated.
\param	start	The index of the first character from \a src that will be copied
				to the new s_string.
\param	end		The index of the last character from \a src that will be copied
				to the new s_string.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the character sequence from the C-string \a
				src. The new object must be freed with 'free' when not in use
				anymore.
*//*==========================================================================*/
s_string_t  *s_extract_mec  ( const char       * restrict src, ssize_t start, ssize_t end );
/*=========================================================================*//**
\brief  		Creates in the heap a new dynamically allocated s_string object
				and copies to it the character sequence from the s_string \a
				src starting from index \a start and with length \a len.
\param	src		Pointer to the s_string that will be duplicated.
\param	start	The index of the first character from \a src that will be copied
				to the new s_string.
\param	len		The length of the character sequence from \a src that will be
				copied to the new s_string.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the character sequence from the s_string \a
				src. The new object must be freed with 'free' when not in use
				anymore.
*//*==========================================================================*/
s_string_t  *s_extract_ml   ( const s_string_t * restrict src, ssize_t start, size_t len );
/*=========================================================================*//**
\brief  		Creates in the heap a new dynamically allocated s_string object
				and copies to it the character sequence from the C-string \a src
				starting from index \a start and with length \a len.
\param	src		Pointer to the C-string that will be duplicated.
\param	start	The index of the first character from \a src that will be copied
				to the new s_string.
\param	len		The length of the character sequence from \a src that will be
				copied to the new s_string.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the character sequence from the C-string \a
				src. The new object must be freed with 'free' when not in use
				anymore.
*//*==========================================================================*/
s_string_t  *s_extract_mlc  ( const char       * restrict src, ssize_t start, size_t len );
/*=========================================================================*//**
\brief  		Extracts a sequence of characters from the s_string \a src
				located between indexes \a start and \a end, inclusive, and
				copies it to s_string \a dst. The original content of \a dst is
				lost. If \a dst cannot hold all the characters, only the
				characters that will fit in \a dst will be copied.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	src		Pointer to the s_string that will provide the sequence of
				characters to be copied to \a dst.
\param	start	The index of the first character from \a src that will be copied
				to \a dst.
\param	end		The index of the last character from \a src that will be copied
				to \a dst.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_extract_e    (       s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, ssize_t end );
/*=========================================================================*//**
\brief  		Extracts a sequence of characters from the C-string \a src
				located between indexes \a start and \a end, inclusive, and
				copies it to s_string \a dst. The original content of \a dst is
				lost. If \a dst cannot hold all the characters, only the
				characters that will fit in \a dst will be copied.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	src		Pointer to the C-string that will provide the sequence of
				characters to be copied to \a dst.
\param	start	The index of the first character from \a src that will be copied
				to \a dst.
\param	end		The index of the last character from \a src that will be copied
				to \a dst.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_extract_ec   (       s_string_t * restrict dst, const char       * restrict src, ssize_t start, ssize_t end );
/*=========================================================================*//**
\brief  		Extracts a sequence of characters from the s_string \a src
				starting at index \a start and with length \a len, and copies it
				to s_string \a dst. The original content of \a dst is lost. If
				\a dst cannot hold all the characters, only the characters that
				will fit in \a dst will be copied.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	src		Pointer to the s_string that will provide the sequence of
				characters to be copied to \a dst.
\param	start	The index of the first character from \a src that will be copied
				to \a dst.
\param	len		The length of the character sequence from \a src that will be
				copied to \a dst.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_extract_l    (       s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, size_t len );
/*=========================================================================*//**
\brief  		Extracts a sequence of characters from the C-string \a src
				starting at index \a start and with length \a len, and copies it
				to s_string \a dst. The original content of \a dst is lost. If
				\a dst cannot hold all the characters, only the characters that
				will fit in \a dst will be copied.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	src		Pointer to the C-string that will provide the sequence of
				characters to be copied to \a dst.
\param	start	The index of the first character from \a src that will be copied
				to \a dst.
\param	len		The length of the character sequence from \a src that will be
				copied to \a dst.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_extract_lc   (       s_string_t * restrict dst, const char       * restrict src, ssize_t start, size_t len );
/*=========================================================================*//**
\brief  		Inserts a sequence of characters obtained from the beginning of
				the s_string \a src into the s_string \a dst, between indexes \a
				start and \a end. If there are not enough characters in \a src,
				only the characters available will be copied.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	src		Pointer to the s_string that will provide the sequence of
				characters to be copied to \a dst.
\param	start	The index of the first character in \a dst that will be replaced
				by the characters from \a src.
\param	end		The index of the last character in \a dst that will be replaced
				by the characters from \a src.
\param filler   Character used to fill any extra space created by inserting
				characters beyond the current length of the string.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_insert_e     (       s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, ssize_t end, int filler );
/*=========================================================================*//**
\brief  		Inserts a sequence of characters obtained from the beginning of
				the C-string \a src into the s_string \a dst, between indexes \a
				start and \a end. If there are not enough characters in \a src,
				only the characters available will be copied.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	src		Pointer to the C-string that will provide the sequence of
				characters to be copied to \a dst.
\param	start	The index of the first character in \a dst that will be
				replaced by the characters from \a src.
\param	end		The index of the last character in \a dst that will be replaced
				by the characters from \a src.
\param filler   Character used to fill any extra space created by inserting
				characters beyond the current length of the string.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_insert_ec    (       s_string_t * restrict dst, const char       * restrict src, ssize_t start, ssize_t end, int filler );
/*=========================================================================*//**
\brief  		Inserts a sequence of characters obtained from the beginning of
				the s_string \a src into the s_string \a dst, starting at index
				\a start and with length \a len. If there are not enough
				characters in \a src, only the characters available will be
				copied.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	src		Pointer to the s_string that will provide the sequence of
				characters to be copied to \a dst.
\param	start	The index of the first character in \a dst that will be replaced
				by the characters from \a src.
\param	len		The length of the character sequence from \a src that will be
				copied to \a dst.
\param filler   Character used to fill any extra space created by inserting
				characters beyond the current length of the string.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_insert_l     (       s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, size_t len, int filler );
/*=========================================================================*//**
\brief  		Inserts a sequence of characters obtained from the beginning of
				the C-string \a src into the s_string \a dst, starting at index
				\a start and with length \a len. If there are not enough
				characters in \a src, only the characters available will be
				copied.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	src		Pointer to the C-string that will provide the sequence of
				characters to be copied to \a dst.
\param	start	The index of the first character in \a dst that will be replaced
				by the characters from \a src.
\param	len		The length of the character sequence from \a src that will be
				copied to \a dst.
\param filler   Character used to fill any extra space created by inserting
				characters beyond the current length of the string.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_insert_lc    (       s_string_t * restrict dst, const char       * restrict src, ssize_t start, size_t len, int filler );
/*=========================================================================*//**
\brief  		Extracts a sequence of characters of length \a len from s_string
				\a src, starting at index \a srcstart and inserts it into the
				s_string \a dst, starting at index \a dststart. If there are not
				enough characters in \a src from \a srcstart to its end, only
				the characters available will be copied. If there is no room in
				\a dst from \a dststart to its maximum length, the extra
				characters will be ignored.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	dststart The index of the first character in \a dst that will be replaced
				by the characters from \a src.
\param	src		Pointer to the s_string that will provide the sequence of
				characters to be copied to \a dst.
\param	srcstart The index of the first character in \a src that will be inserted
				in \a dst.
\param	len		The length of the sequence of characters that will be copied
				from \a src to \a dst.
\param filler   Character used to fill any extra space created by inserting
				characters beyond the current length of the string.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_extins_l     (       s_string_t * restrict dst, ssize_t dststart, const s_string_t * restrict src, ssize_t srcstart, size_t len, int filler );
/*=========================================================================*//**
\brief  		Extracts a sequence of characters of length \a len from C-string
				\a src, starting at index \a srcstart and inserts it into the
				s_string \a dst, starting at index \a dststart. If there are not
				enough characters in \a src from \a srcstart to its end, only
				the characters available will be copied. If there is no room in
				\a dst from \a dststart to its maximum length, the extra
				characters will be ignored.
\param	dst		Pointer to the s_string that will receive the sequence of
				characters from \a src.
\param	dststart The index of the first character in \a dst that will be replaced
				by the characters from \a src.
\param	src		Pointer to the C-string that will provide the sequence of
				characters to be copied to \a dst.
\param	srcstart The index of the first character in \a src that will be inserted
				in \a dst.
\param	len		The length of the sequence of characters that will be copied
				from \a src to \a dst.
\param filler   Character used to fill any extra space created by inserting
				characters beyond the current length of the string.
\returns		The number of characters that were copied to \a dst or a
				negative value in case of error.
*//*==========================================================================*/
ssize_t      s_extins_lc    (       s_string_t * restrict dst, ssize_t dststart, const char       * restrict src, ssize_t srcstart, size_t len, int filler );
/*=========================================================================*//**
\brief  		Creates in the heap a dynamically allocated copy of the s_string
				\a src.
\param	src		Pointer to the s_string that will be duplicated.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the characters from the s_string \a src. The
				new object must be freed with 'free' when not in use anymore.
*//*==========================================================================*/
s_string_t  *s_strdup       ( const s_string_t * restrict src );
/*=========================================================================*//**
\brief  		Creates in the heap a dynamically allocated copy of the C-string
				\a src.
\param	src		Pointer to the s_string that will be duplicated.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the characters from the C-string \a src. The
				new object must be freed with 'free' when not in use anymore.
*//*==========================================================================*/
s_string_t  *s_strdup_c     ( const char       * restrict src );
/*=========================================================================*//**
\brief  		Creates in the heap a dynamically allocated s_string object
				capable of holding at most the minimum between \a len and the
				length of \a src, and copies that amount of characters from \a
				src to it.
\param	src		Pointer to the s_string that will be duplicated.
\param	len		The maximum number of characters that the new s_string will be
				able to hold.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the characters from the s_string \a src. The
				new object must be freed with 'free' when not in use anymore.
*//*==========================================================================*/
s_string_t  *s_strndup      ( const s_string_t * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Creates in the heap a dynamically allocated s_string object
				capable of holding at most the minimum between \a len and the
				length of \a src, and copies that amount of characters from \a
				src to it.
\param	src		Pointer to the s_string that will be duplicated.
\param	len		The maximum number of characters that the new s_string will be
				able to hold.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the characters from the s_string \a src. The
				new object must be freed with 'free' when not in use anymore.
*//*==========================================================================*/
s_string_t  *s_strndup_c    ( const char       * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Creates in the heap a dynamically allocated s_string object
				capable of holding at most the maximum between \a len and the
				length of \a src, and copies all the characters from \a src to
				it.
\param	src		Pointer to the s_string that will be duplicated.
\param	len		The minimum number of characters that the new s_string will be
				able to hold.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the characters from the s_string \a src. The
				new object must be freed with 'free' when not in use anymore.
*//*==========================================================================*/
s_string_t  *s_strldup      ( const s_string_t * restrict src, size_t len );
/*=========================================================================*//**
\brief  		Creates in the heap a dynamically allocated s_string object
				capable of holding at most the maximum between \a len and the
				length of \a src, and copies all the characters from \a src to
				it.
\param	src		Pointer to the C-string that will be duplicated.
\param	len		The minimum number of characters that the new s_string will be
				able to hold.
\returns		A pointer to a new, dynamically allocated s_string object that
				contains a copy of the characters from the s_string \a src. The
				new object must be freed with 'free' when not in use anymore.
*//*==========================================================================*/
s_string_t  *s_strldup_c    ( const char       * restrict src, size_t len );
/*============================================================================*/
#endif  /*  !defined __S_STRING_H__ */
/*============================================================================*/
