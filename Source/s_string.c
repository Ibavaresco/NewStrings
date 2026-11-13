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
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <stdarg.h>

#include "s_string.h"
/*============================================================================*/
#if			!defined DOXYGEN
/*============================================================================*/
typedef struct s_string
	{
	struct
		{
		uint8_t		Writable	: 1;
		uint8_t		Area		: 2;
		uint8_t		Bits		: 2;
		uint8_t		Sizes		: 2;
		uint8_t		MustBeZero	: 1;
		};
	uint8_t			PayLoad[];
	} s_string_t;
/*============================================================================*/
#endif	/*	!defined DOXYGEN */
/*============================================================================*/
int	s_isreadonly( const s_string_t * restrict str )
	{
	if( str == NULL )
		return -1;		/* -1 is also 'true', but if needed it can be differentiated from a true 'true'.
						   That is, if -1 is returned, it is read-only because it in reality doesn't exist. */

	/* The 'must be zero' field in this first implementation MUST BE ZERO. */
	if( str->MustBeZero != 0 )
		return -1;

	return !str->Writable;
	}
/*============================================================================*/
ssize_t s_strmaxlen( const s_string_t * restrict str )
	{
	size_t	Offset;

	if( str == NULL )
		return 0;

	/* The 'must be zero' field in this first implementation MUST BE ZERO. */
	if( str->MustBeZero != 0 )
		return 0;

	/* If the 'area' field is 3 (heap with references counter) we have a 'size_t' counter before the 'maximum length'. */
	Offset	= str->Area == 3 ? sizeof( size_t ) : 0;

	switch( str->Bits )
		{
		case 0:
			return  (unsigned int)str->PayLoad[Offset+0] << 0;
		case 1:
			return (( (size_t)str->PayLoad[Offset+0] <<  0 ) | ( (size_t)str->PayLoad[Offset+1] <<  8 ));
		case 2:
			return (( (size_t)str->PayLoad[Offset+0] <<  0 ) | ( (size_t)str->PayLoad[Offset+1] <<  8 ) |
					( (size_t)str->PayLoad[Offset+2] << 16 ) | ( (size_t)str->PayLoad[Offset+3] << 24 ));
#if			__SIZEOF_POINTER__ > 4
		case 3:
			return (( (size_t)str->PayLoad[Offset+0] <<  0 ) | ( (size_t)str->PayLoad[Offset+1] <<  8 ) |
					( (size_t)str->PayLoad[Offset+2] << 16 ) | ( (size_t)str->PayLoad[Offset+3] << 24 ) |
					( (size_t)str->PayLoad[Offset+4] << 32 ) | ( (size_t)str->PayLoad[Offset+5] << 40 ) |
					( (size_t)str->PayLoad[Offset+6] << 48 ) | ( (size_t)str->PayLoad[Offset+7] << 56 ));
#endif	/*	__SIZEOF_POINTER__ > 4 */
		}

	/* Should never reach here... */
	return 0;
	}
/*============================================================================*/
static int SetMaxLen( s_string_t * restrict str, size_t MaxLength )
	{
	size_t	Offset;

#if			defined PARANOID_MODE
	if( str == NULL )
		/* ...return an error result. */
		return -1;

	/* The 'must be zero' field in this first implementation MUST BE ZERO... */
	if( str->MustBeZero != 0 )
		/* ...return an error result. */
		return -1;
#endif	/*	defined PARANOID_MODE */

	/* If the 'area' field is 3 (heap with references counter) we have a 'size_t' counter before the 'maximum length'. */
	Offset	= str->Area == 3 ? sizeof( size_t ) : 0;

	switch( str->Bits )
		{
		case 0:
			str->PayLoad[Offset+0]	= (uint8_t)( MaxLength >>  0 );
			return MaxLength;
		case 1:
			str->PayLoad[Offset+0]	= (uint8_t)( MaxLength >>  0 );
			str->PayLoad[Offset+1]	= (uint8_t)( MaxLength >>  8 );
			return MaxLength;
		case 2:
			str->PayLoad[Offset+0]	= (uint8_t)( MaxLength >>  0 );
			str->PayLoad[Offset+1]	= (uint8_t)( MaxLength >>  8 );
			str->PayLoad[Offset+2]	= (uint8_t)( MaxLength >> 16 );
			str->PayLoad[Offset+3]	= (uint8_t)( MaxLength >> 24 );
			return MaxLength;
#if			__SIZEOF_POINTER__ > 4
		case 3:
			str->PayLoad[Offset+0]	= (uint8_t)( MaxLength >>  0 );
			str->PayLoad[Offset+1]	= (uint8_t)( MaxLength >>  8 );
			str->PayLoad[Offset+2]	= (uint8_t)( MaxLength >> 16 );
			str->PayLoad[Offset+3]	= (uint8_t)( MaxLength >> 24 );
			str->PayLoad[Offset+4]	= (uint8_t)( MaxLength >> 32 );
			str->PayLoad[Offset+5]	= (uint8_t)( MaxLength >> 40 );
			str->PayLoad[Offset+6]	= (uint8_t)( MaxLength >> 48 );
			str->PayLoad[Offset+7]	= (uint8_t)( MaxLength >> 56 );
			return MaxLength;
#endif	/*	__SIZEOF_POINTER__ > 4 */
		}

	/* Should never reach here... */
	return 0;
	}
/*============================================================================*/
ssize_t s_strlen( const s_string_t * restrict str )
	{
	size_t	Offset;

	if( str == NULL )
		return 0;

	/* The 'must be zero' field in this first implementation MUST BE ZERO. */
	if( str->MustBeZero != 0 )
		return 0;

	if( str->Sizes == 0 )
		return s_strmaxlen( str );

	/* If the 'area' field is 3 (heap with references counter) we have a 'size_t'
	   counter, besides the 'maximum length' before 'used length'. */
	Offset	= ( str->Area == 3 ? sizeof( size_t ) : 0 ) + ( 1 << str->Bits );

	switch( str->Bits )
		{
		case 0:
			return ( (size_t)str->PayLoad[Offset+0] <<  0 );
		case 1:
			return ( (size_t)str->PayLoad[Offset+0] <<  0 ) | ( (size_t)str->PayLoad[Offset+1] <<  8 );
		case 2:
			return ( (size_t)str->PayLoad[Offset+0] <<  0 ) | ( (size_t)str->PayLoad[Offset+1] <<  8 ) |
				   ( (size_t)str->PayLoad[Offset+2] << 16 ) | ( (size_t)str->PayLoad[Offset+3] << 24 );
#if			__SIZEOF_POINTER__ > 4
		case 3:
			return ( (size_t)str->PayLoad[Offset+0] <<  0 ) | ( (size_t)str->PayLoad[Offset+1] <<  8 ) |
				   ( (size_t)str->PayLoad[Offset+2] << 16 ) | ( (size_t)str->PayLoad[Offset+3] << 24 ) |
				   ( (size_t)str->PayLoad[Offset+4] << 32 ) | ( (size_t)str->PayLoad[Offset+5] << 40 ) |
				   ( (size_t)str->PayLoad[Offset+6] << 48 ) | ( (size_t)str->PayLoad[Offset+7] << 56 );
#endif	/*	__SIZEOF_POINTER__ > 4 */
		}

	/* Should never reach here... */
	return 0;
	}
/*============================================================================*/
ssize_t s_strllen( const s_string_t * restrict str, ssize_t max )
	{
	return ssmin( ssmax( 0, max ), s_strlen( str ));
	}
/*============================================================================*/
ssize_t s_strnlen( const s_string_t * restrict str, ssize_t min )
	{
	return ssmax( min, s_strlen( str ));
	}
/*============================================================================*/
static int SetUsedLen( s_string_t * restrict str, size_t NewLength )
	{
	size_t	Offset;

#if			defined PARANOID_MODE

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

#endif	/*	defined PARANOID_MODE */

	/* There is no 'used length', we have only the 'maximum length', which cannot be changed... */
	if( str->Sizes == 0 )
		/* ...return an error result. */
		return -1;

	if( NewLength > s_strmaxlen( str ))
		return -1;

	/* If the 'area' field is 3 (heap with references counter) we have a 'size_t'
	   counter, besides the 'maximum length', before 'used length'. */
	Offset	= ( str->Area == 3 ? sizeof( size_t ) : 0 ) + ( 1 << str->Bits );

	switch( str->Bits )
		{
		case 0:
			str->PayLoad[Offset+0]	= (uint8_t)( NewLength >>  0 );
			return NewLength;
		case 1:
			str->PayLoad[Offset+0]	= (uint8_t)( NewLength >>  0 );
			str->PayLoad[Offset+1]	= (uint8_t)( NewLength >>  8 );
			return NewLength;
		case 2:
			str->PayLoad[Offset+0]	= (uint8_t)( NewLength >>  0 );
			str->PayLoad[Offset+1]	= (uint8_t)( NewLength >>  8 );
			str->PayLoad[Offset+2]	= (uint8_t)( NewLength >> 16 );
			str->PayLoad[Offset+3]	= (uint8_t)( NewLength >> 24 );
			return NewLength;
#if			__SIZEOF_POINTER__ > 4
		case 3:
			str->PayLoad[Offset+0]	= (uint8_t)( NewLength >>  0 );
			str->PayLoad[Offset+1]	= (uint8_t)( NewLength >>  8 );
			str->PayLoad[Offset+2]	= (uint8_t)( NewLength >> 16 );
			str->PayLoad[Offset+3]	= (uint8_t)( NewLength >> 24 );
			str->PayLoad[Offset+4]	= (uint8_t)( NewLength >> 32 );
			str->PayLoad[Offset+5]	= (uint8_t)( NewLength >> 40 );
			str->PayLoad[Offset+6]	= (uint8_t)( NewLength >> 48 );
			str->PayLoad[Offset+7]	= (uint8_t)( NewLength >> 56 );
			return NewLength;
#endif	/*	__SIZEOF_POINTER__ > 4 */
		}

	/* Should never reach here... */
	return 0;
	}
/*============================================================================*/
static char *s_cstr( s_string_t * restrict str )
	{
	size_t	NumCounters;
	size_t	Offset;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return NULL;

	NumCounters	= 1 + str->Sizes;
	Offset		= ( str->Area == 3 ? sizeof( size_t ) : 0 ) + NumCounters * ( 1 << str->Bits );

	return (char*)&str->PayLoad[Offset];
	}
/*============================================================================*/
const char *s_constcstr( const s_string_t * restrict str, ssize_t start )
	{
	static const char	EmptyString[] = "";
	size_t				NumCounters;
	ssize_t				Offset;
	ssize_t				UsedLen;

	if( str == NULL || str->MustBeZero != 0 )
		return EmptyString;

	UsedLen	= s_strlen( str );

	if( start < -UsedLen || start >= UsedLen )
		return EmptyString;

	if( start < 0 )
		start  += UsedLen;

	NumCounters	= 1 + str->Sizes;
	Offset		= ( str->Area == 3 ? sizeof( size_t ) : 0 ) + NumCounters * ( 1 << str->Bits );

	return (const char*)&str->PayLoad[Offset + start];
	}
/*============================================================================*/
int s_charat( const s_string_t * restrict str, ssize_t index )
	{
	const char	*Ptr;

	Ptr = s_constcstr( str, index );

	return (int)*Ptr;
	}
/*============================================================================*/
ssize_t s_setcharat( s_string_t * restrict dst, ssize_t index, int value, int filler )
	{
	ssize_t	UsedLen;
	char	*Ptr;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	/* We can change a character beyond the current used length, as long as it
	   is not beyond the maximum length. */
	if( index >= s_strmaxlen( dst ))
		return -1;

	UsedLen = s_strlen( dst );

	/* We can have a negative index for indexing from the end of the string (-1 is
	   the last character), but the index cannot be beyond the current used length. */
	if( index < -UsedLen )
		return -1;

	/* 'index' is negative... */
	if( index < 0 )
		/* ...we must revert it, counting from the end to point to the real index. */
		index  += UsedLen;

	if(( Ptr = s_cstr( dst )) == NULL )
		return -1;

	/* The character being changed is more than one position beyond the end of the string... */
	if( index > UsedLen )
		{
		/* ...but 'filer' is zero... */
		if( filler == 0 )
			/* ...so the operation will be ignored. */
			return -1;
		/* ...and 'filler' is not zero... */
		else
			/* ...so we will fill the empty space with 'filler'. */
			memset( &Ptr[UsedLen], filler, index - UsedLen );
		}

	Ptr[index]	= (uint8_t)value;

	if( index >= UsedLen )
		{
		UsedLen = index + 1;
		SetUsedLen( dst, UsedLen );
		Ptr[UsedLen]	= '\0';
		}

	return UsedLen;
	}
/*============================================================================*/
ssize_t s_appendchar( s_string_t * restrict dst, int value )
	{
	ssize_t	UsedLen;
	char	*Ptr;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	UsedLen = s_strlen( dst );

	/* We cannot grow the string beyond its maximum length. */
	if( UsedLen >= s_strmaxlen( dst ))
		return -1;

	if(( Ptr = s_cstr( dst )) == NULL )
		return -1;

	Ptr[UsedLen]		= (uint8_t)value;

	UsedLen++;

	SetUsedLen( dst, UsedLen );
	Ptr[UsedLen]	= '\0';

	return UsedLen;
	}
/*=========================================================================*//**
<pre>
s_delete_e
--------------------------------------------------------------------------------
            ss  se               ss               ss = strstart
        +---+---+---+        +---+---+            se = strend
str:    | A | B | C |    =>  | A | C |
        +---+---+---+        +---+---+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*=========================================================================*//**
s_delete_e Tested OK!!!
*//*==========================================================================*/
ssize_t s_delete_e( s_string_t * restrict str, ssize_t start, ssize_t end )
	{
	ssize_t	UsedLen;
	char	*Ptr;

	/* Either the s_string is invalid or it cannot be changed... */
	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		/* ...that's an error. */
		return -1;

	UsedLen = s_strlen( str );

	/* Either the portion to remove begins after the end of the string or
	   it ends before the beginning of the string... */
	if( start >= UsedLen || end < -UsedLen )
		/* ...the string won't be changed. */
		return 0;

	/* The portion to remove begins before the beginning of the string... */
	if( start < -UsedLen )
		/* ...let's adjust the beginning. */
		start	= 0;
	/* 'start' is negative... */
	else if( start < 0 )
		/* ...let's normalize it counting from the end of the string. */
		start  += UsedLen;

	/* The portion to remove ends after the end of the string... */
	if( end >= UsedLen )
		/* ...let's adjust the end. */
		end		= UsedLen - 1;
	/* 'end' is negative... */
	else if( end < 0 )
		/* ...let's normalize it counting from the end of the string. */
		end	   += UsedLen;

	/* The portion to remove ends before its beginning... */
	if( end < start )
		/* ..the string  won't be changed. */
		return 0;

	/* Get the pointer to the storage area. But is is invalid... */
	if(( Ptr = s_cstr( str )) == NULL )
		/* ...there's nothing we can do, it is an error. */
		return -1;

	/* Move the remaining of the string after the portion to be removed
	   to be right at the beginning of the portion to be removed. */
	memmove( &Ptr[start], &Ptr[end + 1], UsedLen - end - 1 );

	/* Adjust the resulting used length of the string. */
	UsedLen		   -= end - start + 1;

	/* Terminates the string at its new end. */
	Ptr[UsedLen]	= '\0';

	/* Update the used length of the string. */
	SetUsedLen( str, UsedLen );

	/* Return how many characters were effectively removed. */
	return end - start + 1;
	}
/*=========================================================================*//**
<pre>
s_delete_l
--------------------------------------------------------------------------------
            ss                       ss               ss = strstart
        +---+-------+---+        +---+---+
str:    | A |   B   | C |    =>  | A | C |
        +---+-------+---+        +---+---+
            +--len--+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*=========================================================================*//**
s_delete_l Tested OK!!!
*//*==========================================================================*/
ssize_t s_delete_l( s_string_t * restrict str, ssize_t start, ssize_t len )
	{
	ssize_t	UsedLen;
	char	*Ptr;

	/* Either the s_string is invalid or it cannot be changed... */
	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		/* ...that's an error. */
		return -1;

	UsedLen = s_strlen( str );

	/* Either the portion to remove begins after the end of the string or
	   its length is zero... */
	if( start >= UsedLen || len <= 0 )
		return 0;

	/* The portion to remove begins before the beginning of the string... */
	if( start < -UsedLen )
		{
		/* ...let's adjust the length... */
		if(( len += start + UsedLen ) <= 0 )
			return 0;
		/* ...and the beginning. */
		start	= 0;
		}
	/* 'start' is negative... */
	else if( start < 0 )
		/* ...let's normalize it counting from the end of the string. */
		start  += UsedLen;

	if( start + len > UsedLen || start + len < 0 ) /* start + len < 0 could happen due to overflow. */
		len	= UsedLen - start;

	/* Get the pointer to the storage area. But is is invalid... */
	if(( Ptr = s_cstr( str )) == NULL )
		/* ...there's nothing we can do, it is an error. */
		return -1;

	memmove( &Ptr[start], &Ptr[start+len], UsedLen - start - len );

	/* Adjust the resulting used length of the string. */
	UsedLen	   -= len;

	/* Terminates the string at its new end. */
	Ptr[UsedLen]	= '\0';

	/* Update the used length of the string. */
	SetUsedLen( str, UsedLen );

	/* Return how many characters were effectively removed. */
	return len;
	}
/*=========================================================================*//**
<pre>
s_truncate
--------------------------------------------------------------------------------
            ln                   ln               ln = len
        +---+---+            +---+
str:    | A | B |        =>  | A |
        +---+---+            +---+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_truncate( s_string_t * restrict str, ssize_t len )
	{
	ssize_t	UsedLen;
	char	*Ptr;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	UsedLen = s_strlen( str );

	/* The new length is longer than the current length... */
	if( len >= UsedLen )
		/* ...nothing to be done. */
		return UsedLen;

	if( len < -UsedLen )
		len		= 0;
	else if( len < 0 )
		len	   += UsedLen;

	if(( Ptr = s_cstr( str )) == NULL )
		return -1;

	Ptr[len]	= '\0';

	SetUsedLen( str, len );

	return len;
	}
/*=========================================================================*//**
<pre>
s_strcat
--------------------------------------------------------------------------------
            ss                   ss                ss = srcstart
        +---+---+            +---+---+
src:    | A | B |            | A | B |
        +---+---+            +---+---+
                    =>
        +---+                +---+---+
dst:    | C |                | C | B |
        +---+                +---+---+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strcat( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t sstart )
	{
	ssize_t		DstMaxLen, DstUsedLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	DstUsedLen	= s_strlen( dst );

	/* The source is a null pointer... */
	if( src == NULL || src->MustBeZero != 0 )
		/* ...so the destination string will not be modified. */
		return DstUsedLen;

	SrcUsedLen	= s_strlen( src );

	/* The source string is empty... */
	if( SrcUsedLen == 0 || sstart < -SrcUsedLen || sstart >= SrcUsedLen )
		/* ...so again the destination string will not be modified. */
		return DstUsedLen;

	if( sstart < 0 )
		sstart += SrcUsedLen;

	DstMaxLen	= s_strmaxlen( dst );

	DstPtr		= s_cstr( dst );
	SrcPtr		= s_constcstr( src, sstart );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( DstMaxLen - DstUsedLen, SrcUsedLen - sstart );

	memcpy( DstPtr + DstUsedLen, SrcPtr, BytesToCopy );

	DstUsedLen += BytesToCopy;

	/* Terminate the new resulting string. */
	DstPtr[DstUsedLen]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, DstUsedLen );

	return DstUsedLen;
	}
/*=========================================================================*//**
<pre>
s_strcat_c
--------------------------------------------------------------------------------
        +---+                +---+
src:    | A |                | A |
        +---+                +---+
                    =>
        +---+                +---+---+
dst:    | C |                | C | A |
        +---+                +---+---+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strcat_c( s_string_t * restrict dst, const char * restrict src )
	{
	ssize_t		DstMaxLen, DstUsedLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	DstUsedLen	= s_strlen( dst );

	/* The source is a null pointer or it is an empty string... */
	if( src == NULL || ( SrcUsedLen	= strlen( src )) == 0 )
		/* ...so the destination string will not be modified. */
		return DstUsedLen;

	DstMaxLen	= s_strmaxlen( dst );

	DstPtr		= s_cstr( dst );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( DstMaxLen - DstUsedLen, SrcUsedLen );

	memcpy( DstPtr + DstUsedLen, src, BytesToCopy );

	DstUsedLen += BytesToCopy;

	/* Terminate the new resulting string. */
	DstPtr[DstUsedLen]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, DstUsedLen );

	return DstUsedLen;
	}
/*=========================================================================*//**
<pre>
s_strlcat
--------------------------------------------------------------------------------
            ss                         ss                ss = srcstart
        +---+-------+---+          +---+-------+---+
src:    | A |   B   | C |          | A |   B   | C |
        +---+-------+---+          +---+-------+---+
                                       +-------+     <- len - s_strlen(dst)

                            =>
                                   +----len----+
        +---+                      +---+-------+
dst:    | D |                      | D |   B   |
        +---+                      +---+-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strlcat( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t srcstart, ssize_t len )
	{
	ssize_t		ResultLen, DstUsedLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	DstUsedLen	= s_strlen( dst );

	/* 'len' is shorter than the current destination length... */
	if( len <= DstUsedLen )
		/* ...there is nothing to be done. */
		return DstUsedLen;

	/* The source is a null pointer... */
	if( src == NULL || src->MustBeZero != 0 )
		/* ...so the destination string will not be modified. */
		return DstUsedLen;

	if(( SrcUsedLen = s_strlen( src )) == 0 || srcstart >= SrcUsedLen )
		return DstUsedLen;

	if( srcstart < -SrcUsedLen )
		return -1;

	if( srcstart < 0 )
		srcstart+= SrcUsedLen;

	ResultLen	= ssmin( len, s_strmaxlen( dst ));

	DstPtr		= s_cstr( dst );
	SrcPtr		= s_constcstr( src, srcstart );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( ResultLen - DstUsedLen, SrcUsedLen - srcstart );

	memcpy( DstPtr + DstUsedLen, SrcPtr, BytesToCopy );

	DstUsedLen += BytesToCopy;

	/* Terminate the new resulting string. */
	DstPtr[DstUsedLen]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, DstUsedLen );

	return DstUsedLen;
	}
/*=========================================================================*//**
<pre>
s_strlcat_c
--------------------------------------------------------------------------------
        +-------+---+              +-------+---+
src:    |   A   | B |              |   A   | B |
        +-------+---+              +-------+---+
                                   +-------+     <- len - s_strlen(dst)

                            =>

        +---+ <- s_strlen(dst)     +----len----+
        +---+                      +---+-------+
dst:    | C |                      | C |   A   |
        +---+                      +---+-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strlcat_c( s_string_t * restrict dst, const char * restrict src, ssize_t len )
	{
	ssize_t		ResultLen, DstUsedLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	DstUsedLen	= s_strlen( dst );

	/* 'len' is shorter than the current destination length... */
	if( len <= DstUsedLen )
		/* ...there is nothing to be done. */
		return DstUsedLen;

	/* The source is a null pointer... */
	if( src == NULL )
		/* ...so the destination string will not be modified. */
		return DstUsedLen;

	SrcUsedLen	= strlen( src );

	/* The source string is empty... */
	if( SrcUsedLen == 0 )
		/* ...so again the destination string will not be modified. */
		return DstUsedLen;

	ResultLen	= ssmin( len, s_strmaxlen( dst ));

	DstPtr		= s_cstr( dst );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( ResultLen - DstUsedLen, SrcUsedLen );

	memcpy( DstPtr + DstUsedLen, src, BytesToCopy );

	DstUsedLen += BytesToCopy;

	/* Terminate the new resulting string. */
	DstPtr[DstUsedLen]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, DstUsedLen );

	return DstUsedLen;
	}
/*=========================================================================*//**
<pre>
s_strncat
--------------------------------------------------------------------------------
            ss                           ss                ss = srcstart
        +---+-------+---+            +---+-------+---+
src:    | A |   B   | C |            | A |   B   | C |
        +---+-------+---+            +---+-------+---+
            +--len--+                    +--len--+
                            =>
                                         +--len--+
        +---+                        +---+-------+
dst:    | D |                        | D |   B   |
        +---+                        +---+-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strncat( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t srcstart, ssize_t len )
	{
	ssize_t		DstMaxLen, DstUsedLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	DstUsedLen	= s_strlen( dst );

	/* The number of bytes to copy is zero... */
	if( len <= 0 )
		/* ...the destination will not be modified. */
		return DstUsedLen;

	/* The source is a null pointer or it is an empty string... */
	if( src == NULL || src->MustBeZero != 0 || ( SrcUsedLen = s_strlen( src )) == 0 )
		/* ...so the destination string will not be modified too. */
		return DstUsedLen;

	if( srcstart >= SrcUsedLen )
		return DstUsedLen;

	if( srcstart < -SrcUsedLen )
		return -1;

	if( srcstart < 0 )
		srcstart+= SrcUsedLen;

	DstMaxLen	= s_strmaxlen( dst );

	DstPtr		= s_cstr( dst );
	SrcPtr		= s_constcstr( src, srcstart );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( DstMaxLen - DstUsedLen, ssmin( SrcUsedLen - srcstart, len ));

	memcpy( DstPtr + DstUsedLen, SrcPtr, BytesToCopy );

	DstUsedLen += BytesToCopy;

	/* Terminate the new resulting string. */
	DstPtr[DstUsedLen]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, DstUsedLen );

	return DstUsedLen;
	}
/*=========================================================================*//**
<pre>
s_strncat_c
--------------------------------------------------------------------------------
        +-------+---+            +-------+---+
src:    |   A   | B |            |   A   | B |
        +-------+---+            +-------+---+
        +--len--+                +--len--+
                        =>
                                     +--len--+
        +---+                    +---+-------+
dst:    | C |                    | C |   A   |
        +---+                    +---+-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strncat_c( s_string_t * restrict dst, const char * restrict src, ssize_t len )
	{
	ssize_t		DstMaxLen, DstUsedLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	DstUsedLen	= s_strlen( dst );

	/* The number of bytes to copy is zero... */
	if( len <= 0 )
		/* ...the destination will not be modified. */
		return DstUsedLen;

	/* The source is a null pointer or it is an empty string... */
	if( src == NULL || ( SrcUsedLen	= strlen( src )) == 0 )
		/* ...so the destination string will not be modified too. */
		return DstUsedLen;

	DstMaxLen	= s_strmaxlen( dst );

	DstPtr		= s_cstr( dst );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( DstMaxLen - DstUsedLen, ssmin( SrcUsedLen, len ));

	memcpy( DstPtr + DstUsedLen, src, BytesToCopy );

	DstUsedLen += BytesToCopy;

	/* Terminate the new resulting string. */
	DstPtr[DstUsedLen]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, DstUsedLen );

	return DstUsedLen;
	}
/*=========================================================================*//**
<pre>
s_strcpy
--------------------------------------------------------------------------------
            ss               ss                ss = srcstart
        +---+---+        +---+---+
src:    | A | B |        | A | B |
        +---+---+        +---+---+
                    =>
                         +---+
dst:      ?              | B |
                         +---+
--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strcpy( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t srcstart )
	{
	ssize_t		DstMaxLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == src )
		return -1;

	DstPtr		= s_cstr( dst );

	/* The source is a null pointer or it is an empty string... */
	if( src == NULL || src->MustBeZero != 0 || ( SrcUsedLen	= s_strlen( src )) == 0 || srcstart >= SrcUsedLen )
		/* ...so the destination string will be emptied. */
		{
		DstPtr[0]	= '\0';
		SetUsedLen( dst, 0 );
		return 0;
		}

	if( srcstart < -SrcUsedLen )
		return -1;

	if( srcstart < 0 )
		srcstart+= SrcUsedLen;

	DstMaxLen	= s_strmaxlen( dst );
	SrcPtr		= s_constcstr( src, srcstart );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( DstMaxLen, SrcUsedLen - srcstart );

	memcpy( DstPtr, SrcPtr, BytesToCopy );

	/* Terminate the new resulting string. */
	DstPtr[BytesToCopy]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, BytesToCopy );

	return BytesToCopy;
	}
/*=========================================================================*//**
<pre>
s_strcpy_c
--------------------------------------------------------------------------------
        +---+            +---+
src:    | A |            | A |
        +---+            +---+
                    =>
                         +---+
dst:      ?              | A |
                         +---+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strcpy_c( s_string_t * restrict dst, const char * restrict src )
	{
	ssize_t		DstMaxLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	DstPtr		= s_cstr( dst );

	/* The source is a null pointer or it is an empty string... */
	if( src == NULL || ( SrcUsedLen	= strlen( src )) == 0 )
		/* ...so the destination string will be emptied. */
		{
		DstPtr[0]	= '\0';
		SetUsedLen( dst, 0 );
		return 0;
		}

	DstMaxLen	= s_strmaxlen( dst );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( DstMaxLen, SrcUsedLen );

	memcpy( DstPtr, src, BytesToCopy );

	/* Terminate the new resulting string. */
	DstPtr[BytesToCopy]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, BytesToCopy );

	return BytesToCopy;
	}
/*=========================================================================*//**
<pre>
s_strlcpy
--------------------------------------------------------------------------------
            ss                       ss                ss = srcstart
        +---+-------+---+        +---+-------+---+
src:    | A |   B   | C |        | A |   B   | C |
        +---+-------+---+        +---+-------+---+
            +--len--+                +--len--+
                            =>
                                 +--len--+
                                 +-------+
dst:      ?                      |   B   |
                                 +-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strlcpy( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t srcstart, ssize_t len )
	{
	ssize_t		DstMaxLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == src )
		return -1;

	DstPtr		= s_cstr( dst );

	/* The source is a null pointer or it is an empty string... */
	if( len <= 0 || src == NULL || src->MustBeZero != 0 || ( SrcUsedLen	= s_strlen( src )) == 0 || srcstart >= SrcUsedLen )
		/* ...so the destination string will be emptied. */
		{
		DstPtr[0]	= '\0';
		SetUsedLen( dst, 0 );
		return 0;
		}

	if( srcstart < -SrcUsedLen )
		return -1;

	if( srcstart < 0 )
		srcstart+= SrcUsedLen;

	DstMaxLen	= s_strmaxlen( dst );
	SrcPtr		= s_constcstr( src, srcstart );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( len, ssmin( DstMaxLen, SrcUsedLen - srcstart ));

	memcpy( DstPtr, SrcPtr, BytesToCopy );

	/* Terminate the new resulting string. */
	DstPtr[BytesToCopy]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, BytesToCopy );

	return BytesToCopy;
	}
/*=========================================================================*//**
<pre>
s_strlcpy_c
--------------------------------------------------------------------------------
        +-------+---+        +-------+---+
src:    |   A   | B |        |   A   | B |
        +-------+---+        +-------+---+
        +--len--+            +--len--+
                        =>
                             +--len--+
                             +-------+
dst:        ?                |   A   |
                             +-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_strlcpy_c( s_string_t * restrict dst, const char * restrict src, ssize_t len )
	{
	ssize_t		DstMaxLen;
	ssize_t		SrcUsedLen;
	char		*DstPtr;
	ssize_t		BytesToCopy;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 )
		return -1;

	DstPtr		= s_cstr( dst );

	/* The source is a null pointer or it is an empty string... */
	if( len <= 0 || src == NULL || ( SrcUsedLen	= strlen( src )) == 0 )
		/* ...so the destination string will be emptied. */
		{
		DstPtr[0]	= '\0';
		SetUsedLen( dst, 0 );
		return 0;
		}

	DstMaxLen	= s_strmaxlen( dst );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( len, ssmin( DstMaxLen, SrcUsedLen ));

	memcpy( DstPtr, src, BytesToCopy );

	/* Terminate the new resulting string. */
	DstPtr[BytesToCopy]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, BytesToCopy );

	return BytesToCopy;
	}
/*============================================================================*/
/*  Deprecated. In the C library implementation it is less safe than strlcpy. In
	our implementation both would be equivalent.

ssize_t s_strncpy( s_string_t * restrict dst, const s_string_t * restrict src, size_t len )
	{
	return 0;
	}
*/
/*============================================================================*/
int s_strcmp( const s_string_t * restrict s1, ssize_t s1start, const s_string_t * restrict s2, ssize_t s2start )
	{
	const char	*s1Ptr;
	const char	*s2Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= s_strlen( s1 );
	s2Len		= s_strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( s1 == s2 )
		return -1;

	if( s1start < -s1Len || s1start >= s1Len )
		s1start		= s1Len;
	else if( s1start < 0 )
		s1start    += s1Len;

	if( s2start < -s2Len || s2start >= s2Len )
		s2start		= s2Len;
	else if( s2start < 0 )
		s2start    += s2Len;

	Len			= ssmin( s2Len - s2start, s1Len - s1start ) + 1;

	s2Ptr		= s_constcstr( s2, s2start );
	s1Ptr		= s_constcstr( s1, s1start );

	for( Index = 0; Index < Len; Index++ )
		if( s1Ptr[Index] != s2Ptr[Index] )
			return s1Ptr[Index] - s2Ptr[Index] < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int s_strcmp_c( const s_string_t * restrict s1, ssize_t s1start, const char * restrict s2 )
	{
	const char	*s1Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= s_strlen( s1 );
	s2Len		= strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	if( s1start < -s1Len || s1start >= s1Len )
		s1start	= s1Len;
	else if( s1start < 0 )
		s1start    += s1Len;

	Len			= ssmin( s2Len, s1Len - s1start ) + 1;

	s1Ptr		= s_constcstr( s1, s1start );

	for( Index = 0; Index < Len; Index++ )
		if( s1Ptr[Index] != s2[Index] )
			return s1Ptr[Index] - s2[Index] < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int c_strcmp_s( const char * restrict s1, const s_string_t * restrict s2, ssize_t s2start )
	{
	const char	*s2Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= strlen( s1 );
	s2Len		= s_strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	if( s2start < -s2Len || s2start >= s2Len )
		s2start		= s2Len;
	else if( s2start < 0 )
		s2start    += s2Len;

	Len			= ssmin( s2Len - s2start, s1Len ) + 1;

	s2Ptr		= s_constcstr( s2, s2start );

	for( Index = 0; Index < Len; Index++ )
		if( s1[Index] != s2Ptr[Index] )
			return s1[Index] - s2Ptr[Index] < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int s_stricmp( const s_string_t * restrict s1, ssize_t s1start, const s_string_t * restrict s2, ssize_t s2start )
	{
	const char	*s1Ptr;
	const char	*s2Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= s_strlen( s1 );
	s2Len		= s_strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( s1 == s2 )
		return -1;

	if( s1start < -s1Len || s1start >= s1Len )
		s1start	= s1Len;
	else if( s1start < 0 )
		s1start   += s1Len;

	if( s2start < -s2Len || s2start >= s2Len )
		s2start		= s2Len;
	else if( s2start < 0 )
		s2start   += s2Len;

	Len			= ssmin( s2Len - s2start, s1Len - s1start ) + 1;

	s2Ptr		= s_constcstr( s2, s2start );
	s1Ptr		= s_constcstr( s1, s1start );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( s1Ptr[Index] ) != toupper( s2Ptr[Index] ))
			return toupper( s1Ptr[Index] ) - toupper( s2Ptr[Index] ) < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int s_stricmp_c( const s_string_t * restrict s1, ssize_t s1start, const char * restrict s2 )
	{
	const char	*s1Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= s_strlen( s1 );
	s2Len		= strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	if( s1start < -s1Len || s1start >= s1Len )
		s1start		= s1Len;
	else if( s1start < 0 )
		s1start    += s1Len;

	Len			= ssmin( s2Len, s1Len - s1start ) + 1;

	s1Ptr		= s_constcstr( s1, s1start );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( s1Ptr[Index] ) != toupper( s2[Index] ))
			return toupper( s1Ptr[Index] ) - toupper( s2[Index] ) < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int c_stricmp_s( const char * restrict s1, const s_string_t * restrict s2, ssize_t s2start )
	{
	const char	*s2Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= strlen( s1 );
	s2Len		= s_strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	if( s2start < -s2Len || s2start >= s2Len )
		s2start		= s2Len;
	else if( s2start < 0 )
		s2start    += s2Len;

	Len			= ssmin( s2Len - s2start, s1Len ) + 1;

	s2Ptr		= s_constcstr( s2, s2start );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( s1[Index] ) != toupper( s2Ptr[Index] ))
			return toupper( s1[Index] ) - toupper( s2Ptr[Index] ) < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int s_strncmp( const s_string_t * restrict s1, ssize_t s1start, const s_string_t * restrict s2, ssize_t s2start, ssize_t len )
	{
	const char	*s1Ptr;
	const char	*s2Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

	/* We are comparing zero bytes... */
	if( len == 0 )
		/* ...two empty strings will be always equal. */
		return 0;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= s_strlen( s1 );
	s2Len		= s_strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( s1 == s2 )
		return -1;

	Len	= ssmin( s1Len, s2Len );

	if( len < -Len )
		return 0;
	if( len < 0 )
		len	+= Len;

	if( s1start < -s1Len || s1start >= s1Len )
		s1start		= s1Len;
	else if( s1start < 0 )
		s1start    += s1Len;

	if( s2start < -s2Len || s2start >= s2Len )
		s2start		= s2Len;
	else if( s2start < 0 )
		s2start    += s2Len;

	Len			= ssmin( len, ssmin( s2Len - s2start, s1Len - s1start ) + 1 );

	s2Ptr		= s_constcstr( s2, s2start );
	s1Ptr		= s_constcstr( s1, s1start );

	for( Index = 0; Index < Len; Index++ )
		if( s1Ptr[Index] != s2Ptr[Index] )
			return s1Ptr[Index] - s2Ptr[Index] < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int s_strncmp_c( const s_string_t * restrict s1, ssize_t s1start, const char * restrict s2, ssize_t len )
	{
	const char	*s1Ptr;

	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

	/* We are comparing zero bytes... */
	if( len == 0 )
		/* ...two empty strings will be always equal. */
		return 0;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= s_strlen( s1 );
	s2Len		= strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	Len	= ssmin( s1Len, s2Len );

	if( len < -Len )
		return 0;
	if( len < 0 )
		len	+= Len;

	if( s1start < -s1Len || s1start >= s1Len )
		s1start		= s1Len;
	else if( s1start < 0 )
		s1start   += s1Len;

	Len			= ssmin( len, ssmin( s2Len, s1Len - s1start ) + 1 );

	s1Ptr		= s_constcstr( s1, s1start );

	for( Index = 0; Index < Len; Index++ )
		if( s1Ptr[Index] != s2[Index] )
			return s1Ptr[Index] - s2[Index] < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int c_strncmp_s( const char * restrict s1, const s_string_t * restrict s2, ssize_t s2start, ssize_t len )
	{

	const char	*s2Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

	/* We are comparing zero bytes... */
	if( len == 0 )
		/* ...two empty strings will be always equal. */
		return 0;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= strlen( s1 );
	s2Len		= s_strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	Len	= ssmin( s1Len, s2Len );

	if( len < -Len )
		return 0;
	if( len < 0 )
		len	+= Len;

	if( s2start < -s2Len || s2start >= s2Len )
		s2start		= s2Len;
	else if( s2start < 0 )
		s2start    += s2Len;

	Len			= ssmin( len, ssmin( s2Len - s2start, s1Len ) + 1 );

	s2Ptr		= s_constcstr( s2, s2start );

	for( Index = 0; Index < Len; Index++ )
		if( s1[Index] != s2Ptr[Index] )
			return s1[Index] - s2Ptr[Index] < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int s_strnicmp( const s_string_t * restrict s1, ssize_t s1start, const s_string_t * restrict s2, ssize_t s2start, ssize_t len )
	{
	const char	*s1Ptr;
	const char	*s2Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

	/* We are comparing zero bytes... */
	if( len == 0 )
		/* ...two empty strings will be always equal. */
		return 0;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= s_strlen( s1 );
	s2Len		= s_strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( s1 == s2 )
		return -1;

	Len	= ssmin( s1Len, s2Len );

	if( len < -Len )
		return 0;
	if( len < 0 )
		len	+= Len;

	if( s1start < -s1Len || s1start >= s1Len )
		s1start		= s1Len;
	else if( s1start < 0 )
		s1start   += s1Len;

	if( s2start < -s2Len || s2start >= s2Len )
		s2start		= s2Len;
	else if( s2start < 0 )
		s2start    += s2Len;

	Len			= ssmin( len, ssmin( s2Len - s2start, s1Len - s1start ) + 1 );

	s2Ptr		= s_constcstr( s2, s2start );
	s1Ptr		= s_constcstr( s1, s1start );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( s1Ptr[Index] ) != toupper( s2Ptr[Index] ))
			return toupper( s1Ptr[Index] ) - toupper( s2Ptr[Index] ) < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int s_strnicmp_c( const s_string_t * restrict s1, ssize_t s1start, const char * restrict s2, ssize_t len )
	{
	const char	*s1Ptr;

	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

	/* We are comparing zero bytes... */
	if( len == 0 )
		/* ...two empty strings will be always equal. */
		return 0;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= s_strlen( s1 );
	s2Len		= strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	Len	= ssmin( s1Len, s2Len );

	if( len < -Len )
		return 0;
	if( len < 0 )
		len	+= Len;

	if( s1start < -s1Len || s1start >= s1Len )
		s1start		= s1Len;
	else if( s1start < 0 )
		s1start   += s1Len;

	Len			= ssmin( len, ssmin( s2Len, s1Len - s1start ) + 1 );

	s1Ptr		= s_constcstr( s1, s1start );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( s1Ptr[Index] ) != toupper( s2[Index] ))
			return toupper( s1Ptr[Index] ) - toupper( s2[Index] ) < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*============================================================================*/
int c_strnicmp_s( const char * restrict s1, const s_string_t * restrict s2, ssize_t s2start, ssize_t len )
	{

	const char	*s2Ptr;
	ssize_t		Index;
	ssize_t		Len;
	ssize_t		s1Len, s2Len;

	/* We are comparing zero bytes... */
	if( len == 0 )
		/* ...two empty strings will be always equal. */
		return 0;

#if 0
	/* Both s1 and s2 are null pointers... */
	if( s1 == NULL && s2 == NULL )
		/* ...so both are equal. */
		return 0;
	/* s1 is a null pointer...*/
	if( s1 == NULL )
		/* ...so it is smaller than s2. */
		return -1;
	/* s2 is a null pointer...*/
	if( s2 == NULL )
		/* ...so s1 is bigger than s2. */
		return +1;
#endif
	s1Len		= strlen( s1 );
	s2Len		= s_strlen( s2 );

	/* Both 's2' and 's1' are either NULL pointers or empty strings... */
	if( s1Len == 0 && s2Len == 0 )
		/* ...so they are equal. */
		return 0;
	if( s1Len == 0 )
		return -1;
	if( s2Len == 0 )
		return +1;

	Len	= ssmin( s1Len, s2Len );

	if( len < -Len )
		return 0;
	if( len < 0 )
		len	+= Len;

	if( s2start < -s2Len || s2start >= s2Len )
		s2start		= s2Len;
	else if( s2start < 0 )
		s2start    += s2Len;

	Len			= ssmin( len, ssmin( s2Len - s2start, s1Len ) + 1 );

	s2Ptr		= s_constcstr( s2, s2start );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( s1[Index] ) != toupper( s2Ptr[Index] ))
			return toupper( s1[Index] ) - toupper( s2Ptr[Index] ) < 0 ? -( Index + 1 ) : ( Index + 1 );

	return 0;
	}
/*=========================================================================*//**
s_strchr Tested OK!!!
*//*==========================================================================*/
ssize_t s_strchr( const s_string_t * restrict str, ssize_t start, int c )
	{
	ssize_t		StrLen;
	const char	*Ptr;
	ssize_t		Index;

	/* str is a null pointer... */
	if( str == NULL )
		/* ...nothing can be found there, let's return a 'not found' result. */
		return -1;

	StrLen	= s_strlen( str );

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	Ptr		= s_constcstr( str, 0 );

	/* Search for the character in the string. */
	for( Index = start; Index < StrLen && Ptr[Index] != c; Index++ )
		{}

	/* If the character was found, return its index in the string, otherwise return a 'not found' result. */
	return ( Index >= 0 && Index < StrLen ) ? Index : -1;
	}
/*=========================================================================*//**
s_strrchr Tested OK!!!
*//*==========================================================================*/
ssize_t s_strrchr( const s_string_t * restrict str, ssize_t start, int c )
	{
	ssize_t		StrLen;
	const char	*Ptr;
	ssize_t		Index;

	/* str is a null pointer... */
	if( str == NULL || str->MustBeZero != 0 )
		/* ...nothing can be found there, let's return a 'not found' result. */
		return -1;

	StrLen	= s_strlen( str );

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	Ptr		= s_constcstr( str, 0 );

	/* Search for the character in the string. */
	for( Index = start; Index >= 0 && Ptr[Index] != c; Index-- )
		{}

	/* If the character was found, return its index in the string, otherwise return a 'not found' result. */
	return ( Index >= 0 && Index < StrLen ) ? Index : -1;
	}
/*============================================================================*/
ssize_t s_strichr( const s_string_t * restrict str, ssize_t start, int c )
	{
	ssize_t		StrLen;
	const char	*Ptr;
	ssize_t		Index;

	/* str is a null pointer... */
	if( str == NULL || str->MustBeZero != 0 )
		/* ...nothing can be found there, let's return a 'not found' result. */
		return -1;

	StrLen	= s_strlen( str );

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	Ptr		= s_constcstr( str, 0 );
	c		= toupper( c );

	/* Search for the character in the string. */
	for( Index = start; Index < StrLen && toupper( Ptr[Index] ) != c; Index++ )
		{}

	/* If the character was found, return its index in the string, otherwise return a 'not found' result. */
	return ( Index >= 0 && Index < StrLen ) ? Index : -1;
	}
/*============================================================================*/
ssize_t s_strrichr( const s_string_t * restrict str, ssize_t start, int c )
	{
	ssize_t		StrLen;
	const char	*Ptr;
	ssize_t		Index;

	/* str is a null pointer... */
	if( str == NULL || str->MustBeZero != 0 )
		/* ...nothing can be found there, let's return a 'not found' result. */
		return -1;

	StrLen	= s_strlen( str );

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	Ptr		= s_constcstr( str, 0 );
	c		= toupper( c );

	/* Search for the character in the string. */
	for( Index = start; Index >= 0 && toupper( Ptr[Index] ) != c; Index-- )
		{}

	/* If the character was found, return its index in the string, otherwise return a 'not found' result. */
	return ( Index >= 0 && Index < StrLen ) ? Index : -1;
	}
/*============================================================================*/
ssize_t s_strstr( const s_string_t * restrict s1, ssize_t s1start, const s_string_t * restrict s2, ssize_t s2start )
	{
	ssize_t		s1Len, s2Len;
	ssize_t		Index;
	const char	*s1Ptr, *s2Ptr;

	/* At least one of the strings is a NULL pointer... */
	if( s1 == NULL || s1->MustBeZero != 0 || s2 == NULL || s2->MustBeZero != 0 )
		/* ...no search is possible. */
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( s1 == s2 )
		return -1;

	/* 'src' is an empty string... */
	if(( s2Len = s_strlen( s2 )) == 0 )
		/* ...it will never be found inside 'str'. */
		return -1;

	/* 's2start' points outside of the string 'source'... */
	if( s2start < -s2Len || s2start > s2Len )
		/* ...nothing to be searched. */
		return -1;

	/* 's2start' is negative... */
	if( s2start < 0 )
		/* ...let's normalize it counting from the end. */
		s2start+= s2Len;

	/* Calculate the effective length of the string to be searched. */
	s2Len -= s2start;

	/* 'str' is an empty string... */
	if(( s1Len = s_strlen( s1 )) == 0 )
		/* ...nothing can be found inside it. */
		return -1;

	/* 's1start' points outside of the string 'str'... */
	if( s1start < -s1Len || s1start > s1Len )
		/* ...there's nothing to search in. */
		return -1;

	/* 's1start' is negative... */
	if( s1start < 0 )
		/* ...let's normalize it counting from the end. */
		s1start+= s1Len;

	/* Calculate the effective length of the string to be searched in. */
	s1Len -= s1start;

	/* The string to be searched is longer than the string to search in... */
	if( s2Len > s1Len )
		/* ...it will never be found. */
		return -1;

	s1Ptr	= s_constcstr( s1, s1start );
	s2Ptr	= s_constcstr( s2, s2start );

	for( Index = 0; Index <= s1Len - s2Len ; Index++ )
		{
		size_t	j;
		for( j = 0; j < s2Len && s1Ptr[Index+j] == s2Ptr[j] ; j++ )
			{}
		if( j == s2Len )
			return Index + s1start;
		}

	return -1;
	}
/*============================================================================*/
ssize_t s_strstr_c( const s_string_t * restrict s1, ssize_t s1start, const char * restrict s2 )
	{
	ssize_t		s1Len, s2Len;
	ssize_t		Index;
	const char	*s1Ptr;

	/* At least one of the strings is a NULL pointer... */
	if( s1 == NULL || s1->MustBeZero != 0 || s2 == NULL )
		/* ...no search is possible. */
		return -1;

	/* If 's2' is an empty string it will not be found inside 's1'... */
	if(( s2Len = strlen( s2 )) == 0 )
		return -1;

	/* 's1' is an empty string... */
	if(( s1Len = s_strlen( s1 )) == 0 )
		/* ...nothing can be found inside it. */
		return -1;

	/* 's1start' points outside of the string 's1'... */
	if( s1start < -s1Len || s1start > s1Len )
		/* ...there's nothing to search in. */
		return -1;

	/* 's1start' is negative... */
	if( s1start < 0 )
		/* ...let's normalize it counting from the end. */
		s1start+= s1Len;

	/* Calculate the effective length of the string to be searched in. */
	s1Len -= s1start;

	/* The string to be searched is longer than the string to search in... */
	if( s2Len > s1Len )
		/* ...it will never be found. */
		return -1;

	s1Ptr	= s_constcstr( s1, s1start );


	for( Index = 0; Index <= s1Len - s2Len ; Index++ )
		{
		size_t	j;
		for( j = 0; j < s2Len && s1Ptr[Index+j] == s2[j] ; j++ )
			{}
		if( j == s2Len )
			return Index + s1start;
		}

	return -1;
	}
/*============================================================================*/
ssize_t c_strstr_s( const char * restrict s1, const s_string_t * restrict s2, ssize_t s2start )
	{
	ssize_t		s1Len, s2Len;
	ssize_t		Index;
	const char	*s2Ptr;

	/* At least one of the strings is a NULL pointer... */
	if( s1 == NULL || s2 == NULL || s2->MustBeZero != 0 )
		/* ...no search is possible. */
		return -1;

	/* 's2' is an empty string... */
	if(( s2Len = s_strlen( s2 )) == 0 )
		/* ...it will never be found inside 's1'. */
		return -1;

	/* 's2start' points outside of the string 'source'... */
	if( s2start < -s2Len || s2start > s2Len )
		/* ...nothing to be searched. */
		return -1;

	/* 's2start' is negative... */
	if( s2start < 0 )
		/* ...let's normalize it counting from the end. */
		s2start+= s2Len;

	/* Calculate the effective length of the string to be searched. */
	s2Len -= s2start;

	if(( s1Len = strlen( s1 )) == 0 )
		return -1;

	/* The string to be searched is longer than the string to search in... */
	if( s2Len > s1Len )
		/* ...it will never be found. */
		return -1;


	s2Ptr	= s_constcstr( s2, s2start );

	for( Index = 0; Index <= s1Len - s2Len ; Index++ )
		{
		size_t	j;
		for( j = 0; j < s2Len && s1[Index+j] == s2Ptr[j] ; j++ )
			{}
		if( j == s2Len )
			return Index;
		}

	return -1;
	}
/*============================================================================*/
ssize_t s_stristr( const s_string_t * restrict s1, ssize_t s1start, const s_string_t * restrict s2, ssize_t s2start )
	{
	ssize_t		s1Len, s2Len;
	ssize_t		Index;
	const char	*s1Ptr, *s2Ptr;

	/* At least one of the strings is a NULL pointer... */
	if( s1 == NULL || s1->MustBeZero != 0 || s2 == NULL || s2->MustBeZero != 0 )
		/* ...no search is possible. */
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( s1 == s2 )
		return -1;

	/* 's2' is an empty string... */
	if(( s2Len = s_strlen( s2 )) == 0 )
		/* ...it will never be found inside 's1'. */
		return -1;

	/* 's2start' points outside of the string 'source'... */
	if( s2start < -s2Len || s2start > s2Len )
		/* ...nothing to be searched. */
		return -1;

	/* 's2start' is negative... */
	if( s2start < 0 )
		/* ...let's normalize it counting from the end. */
		s2start+= s2Len;

	/* Calculate the effective length of the string to be searched. */
	s2Len -= s2start;

	/* 's1' is an empty string... */
	if(( s1Len = s_strlen( s1 )) == 0 )
		/* ...nothing can be found inside it. */
		return -1;

	/* 's1start' points outside of the string 's1'... */
	if( s1start < -s1Len || s1start > s1Len )
		/* ...there's nothing to search in. */
		return -1;

	/* 's1start' is negative... */
	if( s1start < 0 )
		/* ...let's normalize it counting from the end. */
		s1start+= s1Len;

	/* Calculate the effective length of the string to be searched in. */
	s1Len -= s1start;

	/* The string to be searched is longer than the string to search in... */
	if( s2Len > s1Len )
		/* ...it will never be found. */
		return -1;

	s1Ptr	= s_constcstr( s1, s1start );
	s2Ptr	= s_constcstr( s2, s2start );

	for( Index = 0; Index <= s1Len - s2Len ; Index++ )
		{
		size_t	j;
		for( j = 0; j < s2Len && toupper( s1Ptr[Index+j] ) == toupper( s2Ptr[j] ); j++ )
			{}
		if( j == s2Len )
			return Index + s1start;
		}

	return -1;
	}
/*============================================================================*/
ssize_t s_stristr_c( const s_string_t *s1, ssize_t s1start, const char *s2 )
	{
	ssize_t		s1Len, s2Len;
	ssize_t		Index;
	const char	*s1Ptr;

	/* At least one of the strings is a NULL pointer... */
	if( s1 == NULL || s1->MustBeZero != 0 || s2 == NULL )
		/* ...no search is possible. */
		return -1;

	/* If 's2' is an empty string it will not be found inside 's1'... */
	if(( s2Len = strlen( s2 )) == 0 )
		return -1;

	/* 's1' is an empty string... */
	if(( s1Len = s_strlen( s1 )) == 0 )
		/* ...nothing can be found inside it. */
		return -1;

	/* 's1start' points outside of the string 's1'... */
	if( s1start < -s1Len || s1start > s1Len )
		/* ...there's nothing to search in. */
		return -1;

	/* 's1start' is negative... */
	if( s1start < 0 )
		/* ...let's normalize it counting from the end. */
		s1start+= s1Len;

	/* Calculate the effective length of the string to be searched in. */
	s1Len -= s1start;

	/* The string to be searched is longer than the string to search in... */
	if( s2Len > s1Len )
		/* ...it will never be found. */
		return -1;

	s1Ptr	= s_constcstr( s1, s1start );


	for( Index = 0; Index <= s1Len - s2Len ; Index++ )
		{
		size_t	j;
		for( j = 0; j < s2Len && toupper( s1Ptr[Index+j] ) == toupper( s2[j] ); j++ )
			{}
		if( j == s2Len )
			return Index + s1start;
		}

	return -1;
	}
/*============================================================================*/
ssize_t c_stristr_s( const char *s1, const s_string_t *s2, ssize_t s2start )
	{
	ssize_t		s1Len, s2Len;
	ssize_t		Index;
	const char	*s2Ptr;

	/* At least one of the strings is a NULL pointer... */
	if( s1 == NULL || s2 == NULL || s2->MustBeZero != 0 )
		/* ...no search is possible. */
		return -1;

	/* 's2' is an empty string... */
	if(( s2Len = s_strlen( s2 )) == 0 )
		/* ...it will never be found inside 's1'. */
		return -1;

	/* 's2start' points outside of the string 'source'... */
	if( s2start < -s2Len || s2start > s2Len )
		/* ...nothing to be searched. */
		return -1;

	/* 's2start' is negative... */
	if( s2start < 0 )
		/* ...let's normalize it counting from the end. */
		s2start+= s2Len;

	/* Calculate the effective length of the string to be searched. */
	s2Len -= s2start;

	if(( s1Len = strlen( s1 )) == 0 )
		return -1;

	/* The string to be searched is longer than the string to search in... */
	if( s2Len > s1Len )
		/* ...it will never be found. */
		return -1;


	s2Ptr	= s_constcstr( s2, s2start );

	for( Index = 0; Index <= s1Len - s2Len ; Index++ )
		{
		size_t	j;
		for( j = 0; j < s2Len && toupper( s1[Index+j] ) == toupper( s2Ptr[j] ); j++ )
			{}
		if( j == s2Len )
			return Index;
		}

	return -1;
	}
/*============================================================================*/
ssize_t s_strpbrk( const s_string_t * restrict str, ssize_t start, const s_string_t * restrict charset, ssize_t charsetstart )
	{
	ssize_t		StrLen, CharSetLen;
	ssize_t		Index, j;
	const char	*StrPtr, *CharSetPtr;

	if( str == NULL || str->MustBeZero != 0 || charset == NULL || charset->MustBeZero != 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( str == charset )
		return -1;

	/* 'charset' is an empty string... */
	if(( CharSetLen = s_strlen( charset )) == 0 )
		return -1;

	if( charsetstart < -CharSetLen || start >= CharSetLen )
		return -1;

	if( start < 0 )
		charsetstart  += CharSetLen;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	StrPtr		= s_constcstr( str, 0 );
	CharSetPtr	= s_constcstr( charset, 0 );

	for( Index = start; Index < StrLen; Index++ )
		for( j = charsetstart; j < CharSetLen; j++ )
			if( StrPtr[Index] == CharSetPtr[j] )
				return Index;

	return -1;
	}
/*============================================================================*/
ssize_t s_strpbrk_c( const s_string_t * restrict str, ssize_t start, const char * restrict charset )
	{
	ssize_t		StrLen, CharSetLen;
	ssize_t		Index, j;
	const char	*StrPtr;

	if( str == NULL || str->MustBeZero != 0 || charset == NULL )
		return -1;

	/* 'charset' is an empty string... */
	if(( CharSetLen = strlen( charset )) == 0 )
		return -1;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	StrPtr		= s_constcstr( str, 0 );

	for( Index = start; Index < StrLen; Index++ )
		for( j = 0; j < CharSetLen; j++ )
			if( StrPtr[Index] == charset[j] )
				return Index;

	return -1;
	}
/*============================================================================*/
ssize_t s_strcpbrk( const s_string_t * restrict str, ssize_t start, const s_string_t * restrict charset, ssize_t charsetstart )
	{
	ssize_t		StrLen, CharSetLen;
	ssize_t		Index, j;
	const char	*StrPtr, *CharSetPtr;

	if( str == NULL || str->MustBeZero != 0 || charset == NULL || charset->MustBeZero != 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( str == charset )
		return -1;

	/* 'charset' is an empty string... */
	if(( CharSetLen = s_strlen( charset )) == 0 )
		return -1;

	if( charsetstart < -CharSetLen || start >= CharSetLen )
		return -1;

	if( start < 0 )
		charsetstart  += CharSetLen;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	StrPtr		= s_constcstr( str, 0 );
	CharSetPtr	= s_constcstr( charset, 0 );

	for( Index = start; Index < StrLen; Index++ )
		{
		for( j = charsetstart; j < CharSetLen && StrPtr[Index] != CharSetPtr[j]; j++ )
			{}
		if( j >= CharSetLen )
			return Index;
		}

	return -1;
	}
/*============================================================================*/
ssize_t s_strcpbrk_c( const s_string_t * restrict str, ssize_t start, const char * restrict charset )
	{
	ssize_t		StrLen, CharSetLen;
	ssize_t		Index, j;
	const char	*StrPtr;

	if( str == NULL || str->MustBeZero != 0 || charset == NULL )
		return -1;

	/* 'charset' is an empty string... */
	if(( CharSetLen = strlen( charset )) == 0 )
		return -1;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	StrPtr		= s_constcstr( str, 0 );

	for( Index = start; Index < StrLen; Index++ )
		{
		for( j = 0; j < CharSetLen && StrPtr[Index] != charset[j]; j++ )
			{}
		if( j >= CharSetLen )
			return Index;
		}

	return -1;
	}
/*============================================================================*/
s_string_t *s_strdup( const s_string_t * restrict src, ssize_t srcstart )
	{
	ssize_t		SrcLen;
	ssize_t		Size;
	s_string_t	*Str;

	if( src == NULL || src->MustBeZero != 0 )
		return NULL;

	SrcLen	= s_strlen( src );

	if( SrcLen == 0 )
		return NULL;

	if( srcstart < -SrcLen || srcstart >= SrcLen )
		return NULL;

	if( srcstart < 0 )
		srcstart  += SrcLen;

	SrcLen -= srcstart;

	Size	= _s_calcsize( SrcLen );

	if(( Str = malloc( Size )) == NULL )
		return NULL;

	_s_string_init( Str, SrcLen, 2 );

	s_strcpy( Str, src, srcstart );

	return Str;
	}
/*============================================================================*/
s_string_t *s_strdup_c( const char * restrict src )
	{
	ssize_t		SrcLen;
	ssize_t		Size;
	s_string_t	*Str;

	if( src == NULL )
		return NULL;

	SrcLen	= strlen( src );

	if( SrcLen == 0 )
		return NULL;

	Size	= _s_calcsize( SrcLen );

	Str		= malloc( Size );

	if( Str == NULL )
		return NULL;

	_s_string_init( Str, SrcLen, 2 );

	s_strcpy_c( Str, src );

	return Str;
	}
/*============================================================================*/
s_string_t *s_strldup( const s_string_t * restrict src, ssize_t start, ssize_t len )
	{
	ssize_t		SrcLen;
	ssize_t		Size;
	s_string_t	*Str;

	if( src == NULL || src->MustBeZero != 0 || len <= 0 )
		return NULL;

	if(( SrcLen	= s_strlen( src )) == 0 )
		return NULL;

	if( start < -SrcLen || start > SrcLen )
		return NULL;

	if( start < 0 )
		start += SrcLen;

	SrcLen -= start;

	SrcLen	= ssmin( len, SrcLen );

	Size	= _s_calcsize( SrcLen );

	if(( Str = malloc( Size )) == NULL )
		return NULL;

	_s_string_init( Str, SrcLen, 2 );

	s_strcpy( Str, src, start );

	return Str;
	}
/*============================================================================*/
s_string_t *s_strldup_c( const char * restrict src, ssize_t len )
	{
	ssize_t		SrcLen;
	ssize_t		Size;
	s_string_t	*Str;

	if( src == NULL || len <= 0 )
		return NULL;

	SrcLen	= ssmin( len, strlen( src ));

	if( SrcLen == 0 )
		return NULL;

	Size	= _s_calcsize( SrcLen );

	if(( Str = malloc( Size )) == NULL )
		return NULL;

	_s_string_init( Str, SrcLen, 2 );

	s_strcpy_c( Str, src );

	return Str;
	}
/*============================================================================*/
s_string_t *s_strndup( const s_string_t * restrict src, ssize_t start, ssize_t len )
	{
	ssize_t		SrcLen, DstLen;
	ssize_t		Size;
	s_string_t	*Dst;

/*
	if( src == NULL || src->MustBeZero != 0 )
		return NULL;
*/

	SrcLen	= s_strlen( src );

	if( start < -SrcLen || start > SrcLen )
		return NULL;

	if( start < 0 )
		start  += SrcLen;

	SrcLen -= start;

	if( SrcLen == 0 && len <= 0 )
		return NULL;

	DstLen	= ssmax( len, SrcLen );

	Size	= _s_calcsize( DstLen );

	if(( Dst = malloc( Size )) == NULL )
		return NULL;

	_s_string_init( Dst, DstLen, 2 );

	s_strcpy( Dst, src, start );

	return Dst;
	}
/*============================================================================*/
s_string_t *s_strndup_c( const char * restrict src, ssize_t len )
	{
	ssize_t		SrcLen, DstLen;
	ssize_t		Size;
	s_string_t	*Dst;

	if( src == NULL )
		SrcLen	= 0;
	else
		SrcLen	= strlen( src );

	if( SrcLen == 0 && len <= 0 )
		return NULL;

	DstLen	= ssmax( len, SrcLen );

	Size	= _s_calcsize( DstLen );

	Dst		= malloc( Size );

	if( Dst == NULL )
		return NULL;

	_s_string_init( Dst, DstLen, 2 );

	s_strcpy_c( Dst, src );

	return Dst;
	}
/*=========================================================================*//**
s_strtok Tested OK!!!
*//*==========================================================================*/
ssize_t s_strtok( const s_string_t * restrict str, ssize_t * restrict start, const s_string_t * restrict delim, ssize_t delimstart, ssize_t * restrict length )
	{
	ssize_t		StrLen, DelimLen;
	ssize_t		Index, j, TokenLength, TokenStart;
	const char	*StrPtr, *DelimPtr;

	if( str == NULL || str->MustBeZero != 0 || delim == NULL || delim->MustBeZero != 0 || start == NULL )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( str == delim )
		return -1;

	/* 'delim' is an empty string... */
	if(( DelimLen = s_strlen( delim )) == 0 )
		return -1;

	if( delimstart < -DelimLen || delimstart >= DelimLen )
		return -1;

	if( delimstart < 0 )
		delimstart += DelimLen;

	DelimLen   -= delimstart;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( *start < -StrLen || *start >= StrLen )
		return -1;

	if( *start < 0 )
		*start += StrLen;

	StrPtr		= s_constcstr( str, 0 );
	DelimPtr	= s_constcstr( delim, delimstart );

	/* Skip delimiters at the beginning. */
	for( Index = *start; Index < StrLen; Index++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != DelimPtr[j]; j++ )
			{}
		if( j >= DelimLen )
			break;
		}

	/* We have reached the end of the string, there is no more tokens... */
	if( Index >= StrLen )
		{
		*start	= StrLen;

		if( length != NULL )
			*length = 0;

		return -1;
		}

	TokenStart	= Index;

	/* Search the next delimiter. */
	for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != DelimPtr[j]; j++ )
			{}
		if( j < DelimLen )
			break;
		}

	*start	= Index;

	if( length != NULL )
		*length = TokenLength;

	return TokenStart;
	}
/*=========================================================================*//**
s_strtok_c Tested OK!!!
*//*==========================================================================*/
ssize_t s_strtok_c( const s_string_t * restrict str, ssize_t * restrict start, const char * restrict delim, ssize_t * restrict length )
	{
	ssize_t		StrLen, DelimLen;
	ssize_t		Index, j, TokenLength, TokenStart;
	const char	*StrPtr;

	if( str == NULL || str->MustBeZero != 0 || delim == NULL || start == NULL )
		return -1;

	/* 'delim' is an empty string... */
	if(( DelimLen = strlen( delim )) == 0 )
		return -1;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( *start < -StrLen || *start >= StrLen )
		return -1;

	if( *start < 0 )
		*start += StrLen;

	StrPtr		= s_constcstr( str, 0 );

	/* Skip delimiters at the beginning. */
	for( Index = *start; Index < StrLen; Index++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != delim[j]; j++ )
			{}
		if( j >= DelimLen )
			break;
		}

	/* We have reached the end of the string, there is no more tokens... */
	if( Index >= StrLen )
		{
		*start	= StrLen;

		if( length != NULL )
			*length = 0;

		return -1;
		}

	TokenStart	= Index;

	/* Search the next delimiter. */
	for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != delim[j]; j++ )
			{}
		if( j < DelimLen )
			break;
		}

	*start	= Index;

	if( length != NULL )
		*length = TokenLength;

	return TokenStart;
	}
/*=========================================================================*//**
s_strtok_s Tested OK!!!
*//*==========================================================================*/
ssize_t s_strtok_s( s_string_t * restrict dst, const s_string_t * restrict str, ssize_t * restrict start, const s_string_t * restrict delim, ssize_t delimstart )
	{
	ssize_t		StrLen, DelimLen;
	ssize_t		Index, j, TokenLength, TokenStart;
	const char	*StrPtr, *DelimPtr;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 || str == NULL || str->MustBeZero != 0 || delim == NULL || delim->MustBeZero != 0 || start == NULL )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == str || dst == delim || str == delim )
		return -1;

	/* 'delim' is an empty string... */
	if(( DelimLen = s_strlen( delim )) == 0 )
		return -1;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( *start < -StrLen || *start >= StrLen )
		return -1;

	if( *start < 0 )
		*start += StrLen;

	StrPtr		= s_constcstr( str, 0 );
	DelimPtr	= s_constcstr( delim, delimstart );

	/* Skip delimiters at the beginning. */
	for( Index = *start; Index < StrLen; Index++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != DelimPtr[j]; j++ )
			{}
		if( j >= DelimLen )
			break;
		}

	/* We have reached the end of the string, there is no more tokens... */
	if( Index >= StrLen )
		{
		*start	= StrLen;
		return -1;
		}

	TokenStart	= Index;

	/* Search the next delimiter. */
	for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != DelimPtr[j]; j++ )
			{}
		if( j < DelimLen )
			break;
		}

	*start	= Index;

	s_strlcpy_c( dst, &StrPtr[TokenStart], TokenLength );

	return TokenStart;
	}
/*=========================================================================*//**
s_strtok_sc Tested OK!!!
*//*==========================================================================*/
ssize_t s_strtok_sc( s_string_t * restrict dst, const s_string_t * restrict str, ssize_t * restrict start, const char * restrict delim )
	{
	ssize_t		StrLen, DelimLen;
	ssize_t		Index, j, TokenLength, TokenStart;
	const char	*StrPtr;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 || str == NULL || str->MustBeZero != 0 || delim == NULL || start == NULL )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == str )
		return -1;

	/* 'delim' is an empty string... */
	if(( DelimLen = strlen( delim )) == 0 )
		return -1;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( *start < -StrLen || *start >= StrLen )
		return -1;

	if( *start < 0 )
		*start += StrLen;

	StrPtr		= s_constcstr( str, 0 );

	/* Skip delimiters at the beginning. */
	for( Index = *start; Index < StrLen; Index++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != delim[j]; j++ )
			{}
		if( j >= DelimLen )
			break;
		}

	/* We have reached the end of the string, there is no more tokens... */
	if( Index >= StrLen )
		{
		*start	= StrLen;
		return -1;
		}

	TokenStart	= Index;

	/* Search the next delimiter. */
	for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != delim[j]; j++ )
			{}
		if( j < DelimLen )
			break;
		}

	*start	= Index;

	s_strlcpy_c( dst, &StrPtr[TokenStart], TokenLength );

	return TokenStart;
	}
/*=========================================================================*//**
s_strtok_m Tested OK!!!
*//*==========================================================================*/
s_string_t *s_strtok_m( const s_string_t * restrict str, ssize_t * restrict start, const s_string_t * restrict delim, ssize_t delimstart )
	{
	ssize_t		StrLen, DelimLen;
	ssize_t		Index, j, TokenLength, TokenStart;
	const char	*StrPtr, *DelimPtr;

	if( str == NULL || str->MustBeZero != 0 || delim == NULL || delim->MustBeZero != 0 || start == NULL )
		return NULL;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( str == delim )
		return NULL;

	/* 'delim' is an empty string... */
	if(( DelimLen = s_strlen( delim )) == 0 )
		return NULL;

	if( delimstart < -DelimLen || delimstart >= DelimLen )
		return NULL;

	if( delimstart < 0 )
		delimstart += DelimLen;

	DelimLen   -= delimstart;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return NULL;

	if( *start < -StrLen || *start >= StrLen )
		return NULL;

	if( *start < 0 )
		*start += StrLen;

	StrPtr		= s_constcstr( str, 0 );
	DelimPtr	= s_constcstr( delim, delimstart );

	/* Skip delimiters at the beginning. */
	for( Index = *start; Index < StrLen; Index++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != DelimPtr[j]; j++ )
			{}
		if( j >= DelimLen )
			break;
		}

	/* We have reached the end of the string, there is no more tokens... */
	if( Index >= StrLen )
		{
		*start	= StrLen;

		return NULL;
		}

	TokenStart	= Index;

	/* Search the next delimiter. */
	for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != DelimPtr[j]; j++ )
			{}
		if( j < DelimLen )
			break;
		}

	*start	= Index;

	return s_strldup_c( &StrPtr[TokenStart], TokenLength );
	}
/*=========================================================================*//**
s_strtok_mc Tested OK!!!
*//*==========================================================================*/
s_string_t *s_strtok_mc( const s_string_t * restrict str, ssize_t * restrict start, const char * restrict delim )
	{
	ssize_t		StrLen, DelimLen;
	ssize_t		Index, j, TokenLength, TokenStart;
	const char	*StrPtr;

	if( str == NULL || str->MustBeZero != 0 || delim == NULL || start == NULL )
		return NULL;

	/* 'delim' is an empty string... */
	if(( DelimLen = strlen( delim )) == 0 )
		return NULL;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return NULL;

	if( *start < -StrLen || *start >= StrLen )
		return NULL;

	if( *start < 0 )
		*start += StrLen;

	StrPtr		= s_constcstr( str, 0 );

	/* Skip delimiters at the beginning. */
	for( Index = *start; Index < StrLen; Index++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != delim[j]; j++ )
			{}
		if( j >= DelimLen )
			break;
		}

	/* We have reached the end of the string, there is no more tokens... */
	if( Index >= StrLen )
		{
		*start	= StrLen;

		return NULL;
		}

	TokenStart	= Index;

	/* Search the next delimiter. */
	for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
		{
		for( j = 0; j < DelimLen && StrPtr[Index] != delim[j]; j++ )
			{}
		if( j < DelimLen )
			break;
		}

	*start	= Index;

	return s_strldup_c( &StrPtr[TokenStart], TokenLength );
	}
/*============================================================================*/
ssize_t s_strspn( const s_string_t * restrict str, ssize_t start, const s_string_t * restrict charset, ssize_t csstart )
	{
	ssize_t		StrLen, CharSetLen;
	ssize_t		Count, j;
	const char	*StrPtr, *CharSetPtr;

	if( str == NULL || str->MustBeZero != 0 || charset == NULL || charset->MustBeZero != 0 )
		return -1;

	/* 'charset' is an empty string... */
	if(( CharSetLen = s_strlen( charset )) == 0 )
		return -1;

	if( csstart < -CharSetLen || csstart >= CharSetLen )
		return -1;

	if( csstart < 0 )
		csstart  += CharSetLen;

	CharSetLen -= csstart;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	StrPtr		= s_constcstr( str, 0 );
	CharSetPtr	= s_constcstr( charset, csstart );

	for( Count = 0; Count < StrLen; Count++ )
		{
		for( j = 0; j < CharSetLen && StrPtr[start+Count] != CharSetPtr[j]; j++ )
			{}
		if( j >= CharSetLen )
			return Count;
		}

	return Count;
	}
/*============================================================================*/
ssize_t s_strspn_c( const s_string_t * restrict str, ssize_t start, const char * restrict charset )
	{
	ssize_t		StrLen, CharSetLen;
	ssize_t		Count, j;
	const char	*StrPtr;

	if( str == NULL || str->MustBeZero != 0 || charset == NULL )
		return -1;

	/* 'charset' is an empty string... */
	if(( CharSetLen = strlen( charset )) == 0 )
		return -1;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	StrPtr		= s_constcstr( str, 0 );

	for( Count = 0; Count < StrLen; Count++ )
		{
		for( j = 0; j < CharSetLen && StrPtr[start+Count] != charset[j]; j++ )
			{}
		if( j == CharSetLen )
			return Count;
		}

	return Count;
	}
/*============================================================================*/
ssize_t s_strcspn( const s_string_t * restrict str, ssize_t start, const s_string_t * restrict charset, ssize_t csstart )
	{
	ssize_t		StrLen, CharSetLen;
	ssize_t		Count, j;
	const char	*StrPtr, *CharSetPtr;

	if( str == NULL || str->MustBeZero != 0 || charset == NULL || charset->MustBeZero != 0 )
		return -1;

	/* 'charset' is an empty string... */
	if(( CharSetLen = s_strlen( charset )) == 0 )
		return -1;

	if( csstart < -CharSetLen || csstart >= CharSetLen )
		return -1;

	if( csstart < 0 )
		csstart  += CharSetLen;

	CharSetLen -= csstart;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	StrPtr		= s_constcstr( str, 0 );
	CharSetPtr	= s_constcstr( charset, csstart );

	for( Count = 0; Count < StrLen; Count++ )
		for( j = 0; j < CharSetLen; j++ )
			if( StrPtr[start+Count] == CharSetPtr[j] )
				return Count;

	return Count;
	}
/*============================================================================*/
ssize_t s_strcspn_c( const s_string_t * restrict str, ssize_t start, const char * restrict charset )
	{
	ssize_t		StrLen, CharSetLen;
	ssize_t		Count, j;
	const char	*StrPtr;

	if( str == NULL || str->MustBeZero != 0 || charset == NULL )
		return -1;

	/* 'charset' is an empty string... */
	if(( CharSetLen = strlen( charset )) == 0 )
		return -1;

	/* 'str' is an empty string... */
	if(( StrLen = s_strlen( str )) == 0 )
		return -1;

	if( start < -StrLen || start >= StrLen )
		return -1;

	if( start < 0 )
		start  += StrLen;

	StrPtr		= s_constcstr( str, 0 );

	for( Count = 0; Count < StrLen; Count++ )
		for( j = 0; j < CharSetLen; j++ )
			if( StrPtr[start+Count] == charset[j] )
				return Count;

	return Count;
	}
/*=========================================================================*//**
<pre>
s_extract_?e
--------------------------------------------------------------------------------
            ss  se               ss  se           ss = srcstart
        +---+---+---+        +---+---+---+        se = srcend
src:    | A | B | C |        | A | B | C |
        +---+---+---+        +---+---+---+
                        =>
                             +---+
dst:      ?                  | B |
                             +---+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
s_string_t *s_extract_me( const s_string_t * restrict src, ssize_t start, ssize_t end )
	{
	ssize_t	SrcLen;

	if( src == NULL || src->MustBeZero != 0 )
		return NULL;	/* Would it be better returning an empty s_string??? */

	if(( SrcLen = s_strlen( src )) == 0 )
		return NULL;

	if( start >= SrcLen )
		return NULL;

	if( start < -SrcLen )
		start	= 0;
	else if( start < 0 )
		start  += SrcLen;

	if( end >= SrcLen )
		end		= SrcLen - 1;
	else if( end < -SrcLen )
		end		= 0;
	else if( end < 0 )
		end	   += SrcLen;

	if( start > end )
		return NULL;	/* Would it be better returning an empty s_string??? */

	return s_strndup_c( s_constcstr( src, start ), end - start + 1 );
	}
/*=========================================================================*//**
<pre>
s_extract_?ec
--------------------------------------------------------------------------------
            ss  se               ss  se           ss = srcstart
        +---+---+---+        +---+---+---+        se = srcend
src:    | A | B | C |        | A | B | C |
        +---+---+---+        +---+---+---+
                        =>
                             +---+
dst:      ?                  | B |
                             +---+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
s_string_t *s_extract_mec( const char * restrict src, ssize_t start, ssize_t end )
	{
	ssize_t	SrcLen;

	if( src == NULL )
		return NULL;	/* Would it be better returning an empty s_string??? */

	if(( SrcLen = strlen( src )) == 0 )
		return NULL;

	if( start >= SrcLen )
		return NULL;

	if( start < -SrcLen )
		start	= 0;
	else if( start < 0 )
		start  += SrcLen;

	if( end >= SrcLen )
		end		= SrcLen - 1;
	else if( end < -SrcLen )
		end		= 0;
	else if( end < 0 )
		end	   += SrcLen;

	if( start > end )
		return NULL;	/* Would it be better returning an empty s_string??? */

	return s_strndup_c( src + start, end - start + 1 );
	}
/*=========================================================================*//**
<pre>
s_extract_?l
--------------------------------------------------------------------------------
            ss                       ss               ss = srcstart
        +---+-------+---+        +---+-------+---+
src:    | A |   B   | C |        | A |   B   | C |
        +---+-------+---+        +---+-------+---+
            +--len--+                +--len--+

                            =>
                                 +--len--+
                                 +-------+
dst:      ?                      |   B   |
                                 +-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
s_string_t *s_extract_ml( const s_string_t * restrict src, ssize_t start, ssize_t len )
	{
	ssize_t	SrcLen;

	if( src == NULL || src->MustBeZero != 0 || len <= 0 )
		return NULL;	/* Would it be better returning an empty s_string??? */

	if(( SrcLen = s_strlen( src )) == 0 )
		return NULL;

	if( start >= SrcLen )
		return NULL;	/* Would it be better returning an empty s_string??? */
	else if( start < -SrcLen )
		start	= 0;
	else if( start < 0 )
		start  += SrcLen;

	if( start + len > SrcLen )
		len		= SrcLen - start;

	return s_strndup_c( s_constcstr( src, start ), len );
	}
/*=========================================================================*//**
<pre>
s_extract_?l?
--------------------------------------------------------------------------------
            ss                       ss               ss = srcstart
        +---+-------+---+        +---+-------+---+
src:    | A |   B   | C |        | A |   B   | C |
        +---+-------+---+        +---+-------+---+
            +--len--+                +--len--+

                            =>
                                 +--len--+
                                 +-------+
dst:      ?                      |   B   |
                                 +-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
s_string_t *s_extract_mlc( const char * restrict src, ssize_t start, ssize_t len )
	{
	ssize_t	SrcLen;

	if( src == NULL || len <= 0 )
		return NULL;	/* Would it be better returning an empty s_string??? */

	SrcLen		= strlen( src );

	if( start < -SrcLen )
		start	= 0;
	else if( start < 0 )
		start  += SrcLen;

	if( start >= SrcLen )
		return NULL;	/* Would it be better returning an empty s_string??? */

	if( start + len > SrcLen )
		len		= SrcLen - start;

	return s_strndup_c( src + start, len );
	}
/*=========================================================================*//**
<pre>
s_extract_e
--------------------------------------------------------------------------------
            ss  se               ss  se           ss = srcstart
        +---+---+---+        +---+---+---+        se = srcend
src:    | A | B | C |        | A | B | C |
        +---+---+---+        +---+---+---+
                        =>
                             +---+
dst:      ?                  | B |
                             +---+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_extract_e( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, ssize_t end )
	{
	ssize_t	SrcLen;

	if( dst == NULL || dst->MustBeZero != 0 || !dst->Writable )
		return -1;	/* Would it be better returning zero??? */

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == src )
		return -1;

	if( src == NULL || src->MustBeZero != 0 || ( SrcLen = s_strlen( src )) == 0 || start >= SrcLen )
		return s_strcpy_c( dst, "" );

	if( start < -SrcLen )
		start	= 0;
	else if( start < 0 )
		start  += SrcLen;

	if( end > SrcLen )
		end	= SrcLen - 1;
	else if( end < -SrcLen )
		end		= 0;
	else if( end < 0 )
		end	   += SrcLen;

	if( start > end )
		return s_strcpy_c( dst, "" );

	return s_strlcpy_c( dst, s_constcstr( src, start ), end - start + 1 );
	}
/*=========================================================================*//**
<pre>
s_extract_ec
--------------------------------------------------------------------------------
            ss  se               ss  se           ss = srcstart
        +---+---+---+        +---+---+---+        se = srcend
src:    | A | B | C |        | A | B | C |
        +---+---+---+        +---+---+---+
                        =>
                             +---+
dst:      ?                  | B |
                             +---+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_extract_ec( s_string_t * restrict dst, const char * restrict src, ssize_t start, ssize_t end )
	{
	ssize_t	SrcLen;

	if( dst == NULL || dst->MustBeZero != 0 || !dst->Writable )
		return -1;	/* Would it be better returning zero??? */

	if( src == NULL || ( SrcLen = strlen( src )) == 0 || start >= SrcLen )
		return s_strcpy_c( dst, "" );

	if( start < -SrcLen )
		start	= 0;
	else if( start < 0 )
		start  += SrcLen;

	if( end >= SrcLen )
		end		= SrcLen - 1;
	else if( end < -SrcLen )
		end		= 0;
	else if( end < 0 )
		end	   += SrcLen;

	if( start > end )
		return s_strcpy_c( dst, "" );

	return s_strlcpy_c( dst, src + start, end - start + 1 );
	}
/*=========================================================================*//**
<pre>
s_extract_l
--------------------------------------------------------------------------------
            ss                       ss               ss = srcstart
        +---+-------+---+        +---+-------+---+
src:    | A |   B   | C |        | A |   B   | C |
        +---+-------+---+        +---+-------+---+
            +--len--+                +--len--+
                            =>
                                 +--len--+
                                 +-------+
dst:      ?                      |   B   |
                                 +-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_extract_l( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, ssize_t len )
	{
	ssize_t	SrcLen;

	if( dst == NULL || dst->MustBeZero != 0 || !dst->Writable )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == src )
		return -1;

	if( len <= 0 || src == NULL || src->MustBeZero != 0 || ( SrcLen = s_strlen( src )) == 0 || start >= SrcLen )
		return s_strcpy_c( dst, "" );	/* Would it be better returning zero??? */

	if( start < -SrcLen )
		start	= 0;
	else if( start < 0 )
		start  += SrcLen;

	if( start + len > SrcLen )
		len		= SrcLen - start;

	return s_strlcpy( dst, src, start, len );
	}
/*=========================================================================*//**
<pre>
s_extract_lc
--------------------------------------------------------------------------------
            ss                       ss               ss = srcstart
        +---+-------+---+        +---+-------+---+
src:    | A |   B   | C |        | A |   B   | C |
        +---+-------+---+        +---+-------+---+
            +--len--+                +--len--+
                            =>
                                 +--len--+
                                 +-------+
dst:      ?                      |   B   |
                                 +-------+

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_extract_lc( s_string_t * restrict dst, const char * restrict src, ssize_t start, ssize_t len )
	{
	ssize_t	SrcLen;

	if( dst == NULL || dst->MustBeZero != 0 || !dst->Writable )
		return -1;	/* Would it be better returning zero??? */

	if( len <= 0 || src == NULL || ( SrcLen = strlen( src )) == 0 || start >= SrcLen )
		return s_strcpy_c( dst, "" );

	if( start < -SrcLen )
		start	= 0;
	else if( start < 0 )
		start  += SrcLen;

	if( start + len > SrcLen )
		len		= SrcLen - start;

	return s_strlcpy_c( dst, src, len );
	}
/*============================================================================*/
/*============================================================================*/
static ssize_t s_fill_l_calculate( const s_string_t * restrict dst, ssize_t *dstlen, ssize_t *start, ssize_t len )
	{
	ssize_t	StrMaxLen;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 || dstlen == NULL || start == NULL || len == 0 )
		return -1;

	*dstlen		= s_strlen( dst );
	StrMaxLen	= s_strmaxlen( dst );

	if( *start >= StrMaxLen )
		return -1;

	if( *start < 0 )
		*start += *dstlen;

	if( len < 0 )
		len	   += StrMaxLen - *start + 1;

	if( *start < 0 )
		{
		len	   += *start;
		if( len <= 0 )
			return -1;
		*start	= 0;
		}
	else if( *start > *dstlen )
		{
		if( len <= 0 )
			return -1;
		len	   += *start - *dstlen;
		*start	= *dstlen;
		}

	if( len <= 0 )
		return -1;

	if( *start + len > StrMaxLen )
		len		= StrMaxLen - *start;
/*
	if(( Ptr = s_cstr( dst )) == NULL )
		return -1;

	memset( &Ptr[*start], filler, len );


	if( *start + len > dstlen )
		{
		Ptr[*start+len]	= '\0';
		SetUsedLen( dst, start + len );
		}
*/
	return len;
	}
/*=========================================================================*//**
<pre>
s_fill_l
--------------------------------------------------------------------------------
src:    'c'

            +----len----+       =>           +----len----+
        +---+-----------+---+            +---+----   ----+---+    ds = dststart
dst:    | A |     B     | D |            | A |cccc...cccc| D |
        +---+-----------+---+            +---+----   ----+---+
            ds                               ds

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*=========================================================================*//**
s_fill_l tested OK!
*//*==========================================================================*/
ssize_t s_fill_l( s_string_t * restrict dst, ssize_t dststart, ssize_t len, int filler )
	{
	char	*DstPtr;
	ssize_t	DstLen;

	if( filler == '\0' )
		return -1;

	if(( len = s_fill_l_calculate( dst, &DstLen, &dststart, len )) <= 0 )
		return len;

	if(( DstPtr = s_cstr( dst )) == NULL )
		return -1;

	memset( &DstPtr[dststart], filler, len );

	if( dststart + len > DstLen )
		{
		DstPtr[dststart+len]	= '\0';
		SetUsedLen( dst, dststart + len );
		}

	return len;
	}
/*=========================================================================*//**
<pre>
s_fill_ls
--------------------------------------------------------------------------------
            ss                               ss                  ss = srcstart
        +---+---+                        +---+---+
src:    | A | B |                        | A | B |
        +---+---+                        +---+---+
                                 =>
            +----len----+                    +----len----+
        +---+-----------+---+            +---+---+   +---+---+   ds = dststart
dst:    | C |     D     | E |            | C | B |...| B | E |
        +---+-----------+---+            +---+---+   +---+---+
            ds                               ds

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_fill_ls( s_string_t * restrict dst, ssize_t dststart, ssize_t len, const s_string_t * restrict src, ssize_t srcstart )
	{
	char		*DstPtr;
	const char	*SrcPtr;
	ssize_t		DstLen, SrcLen, RemainingLen, FragmentLen;

	if( src == NULL || src->MustBeZero != 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == src )
		return -1;

	if(( len = s_fill_l_calculate( dst, &DstLen, &dststart, len )) <= 0 )
		return len;

	if(( DstPtr = s_cstr( dst )) == NULL )
		return -1;

	SrcLen	= s_strlen( src );

	if( SrcLen <= 0 || srcstart < -SrcLen || srcstart >= SrcLen )
		return -1;
	if( srcstart < 0 )
		srcstart   += SrcLen;
	SrcLen -= srcstart;

	SrcPtr = s_constcstr( src, 0 );

	for( RemainingLen = len; RemainingLen > 0; RemainingLen -= FragmentLen, dststart += FragmentLen )
		{
		FragmentLen	= ssmin( RemainingLen, SrcLen );
		memcpy( &DstPtr[dststart], SrcPtr, FragmentLen );
		}

	if( dststart >= DstLen )
		{
		DstPtr[dststart]	= '\0';
		SetUsedLen( dst, dststart );
		}

	return len;
	}
/*=========================================================================*//**
<pre>
s_fill_lc
--------------------------------------------------------------------------------
        +---+                            +---+
src:    | A |                            | A |
        +---+                            +---+
                                 =>
            +----len----+                    +----len----+
        +---+-----------+---+            +---+---+   +---+---+   ds = dststart
dst:    | C |     D     | E |            | C | A |...| A | E |
        +---+-----------+---+            +---+---+   +---+---+
            ds                               ds

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_fill_lc( s_string_t * restrict dst, ssize_t dststart, ssize_t len, const char * restrict src )
	{
	char		*DstPtr;
	ssize_t		DstLen, SrcLen, RemainingLen, FragmentLen;

	if( src == NULL )
		return -1;

	if(( len = s_fill_l_calculate( dst, &DstLen, &dststart, len )) <= 0 )
		return len;

	if(( DstPtr = s_cstr( dst )) == NULL )
		return -1;

	SrcLen	= strlen( src );

	if( SrcLen <= 0 )
		return -1;

	for( RemainingLen = len; RemainingLen > 0; RemainingLen -= FragmentLen, dststart += FragmentLen )
		{
		FragmentLen	= ssmin( RemainingLen, SrcLen );
		memcpy( &DstPtr[dststart], src, FragmentLen );
		}

	if( dststart >= DstLen )
		{
		DstPtr[dststart]	= '\0';
		SetUsedLen( dst, dststart );
		}

	return len;
	}
/*=========================================================================*//**
<pre>
s_replace_e
--------------------------------------------------------------------------------
            ss                   ss               ss = srcstart
        +---+---+---+        +---+---+---+
src:    | A | B | C |        | A | B | C |
        +---+---+---+        +---+---+---+
                        =>
        +---+---+---+        +---+---+---+        ds = dststart
dst:    | D | E | F |        | D | B | F |        de = dstend
        +---+---+---+        +---+---+---+
            ds  de               ds  de

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*=========================================================================*//**
s_replace_e tested OK!
*//*==========================================================================*/
ssize_t s_replace_e( s_string_t * restrict dst, ssize_t start, ssize_t end, const s_string_t * restrict src, ssize_t srcstart, int filler )
	{
	ssize_t		SrcLen, DstLen, DstMaxLen, Len, TransfLen;
	ssize_t 	LeftFill	= 0, RightFill	= 0;
	char		*DstPtr;
	const char	*SrcPtr;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 || src == NULL || src->MustBeZero != 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == src )
		return -1;

	DstMaxLen	= s_strmaxlen( dst );

	if( start >= DstMaxLen )
		return 0;

	DstLen		= s_strlen( dst );

	if( end < -DstLen )
		return 0;

	if( end < 0 )
		end	   += DstLen;
	else if( end >= DstMaxLen )
		end		= DstMaxLen - 1;

	if( start < 0 )
		start  += DstLen;

	if( end < start )
		return -1;

	SrcLen		= s_strlen( src );

	if( srcstart < 0 )
		srcstart   += SrcLen;

	Len			= end - start + 1;

	if( start < 0 )
		{
		Len			= end - 0 + 1;
		srcstart   -= start;
		start		= 0;
		}

	TransfLen		= Len;

	if( srcstart < -Len || srcstart >= SrcLen )
		{
		if( filler != '\0' )
			LeftFill	= Len;
		srcstart	= SrcLen;
		TransfLen	= 0;
		}
	else if( srcstart < 0 )
		{
		if( filler != '\0' )
			LeftFill	= -srcstart;
		else
			{
			start	   -= srcstart;
			Len		   += srcstart;
			}
		TransfLen  += srcstart;
		srcstart	= 0;
		}

	if( Len - LeftFill > SrcLen - srcstart )
		{
		TransfLen	= SrcLen - srcstart;
		if( filler != '\0' )
			RightFill	= Len - LeftFill - TransfLen;
		else
			Len		    = LeftFill + TransfLen;;
		}

	/* There are one or more gaps to fill, but the filler is zero... */
	if( filler == 0 && ( LeftFill > 0 || RightFill > 0 || start > DstLen ))
		/* ...the operation is not possible. */
		return -1;

	if(( DstPtr = s_cstr( dst )) == NULL )
		return -1;

	SrcPtr = s_constcstr( src, 0 );

	/* There s a gap between the current end of the string and the start position... */
	if( start > DstLen )
		/* ...let's fill the gap with the 'filler'. */
		memset( &DstPtr[DstLen], filler, start - DstLen );

	/* There is a gap on the left of the destination... */
	if( LeftFill > 0 )
		/* ...let's fill the gap with the 'filler'. */
		memset( &DstPtr[start], filler, LeftFill );

	if( TransfLen > 0 )
		memcpy( &DstPtr[start+LeftFill], &SrcPtr[srcstart], TransfLen );

	if( RightFill > 0 )
		memset( &DstPtr[start+LeftFill+TransfLen], filler, RightFill );

	/* Let's calculate how many characters were changed in the destination. */
	LeftFill		= 0;
	if( start > DstLen )
		LeftFill	= start - DstLen;

	if( start + Len > DstLen )
		DstLen		= start + Len;

	DstPtr[DstLen]	= '\0';

	SetUsedLen( dst, DstLen );

	return Len + LeftFill;

#if 0
	if( end >= DstMaxLen )
		end		= DstMaxLen - 1;
	else if( end < 0 )
		end	   += DstLen;

	if( start < 0 )
		start  += DstLen;

	if( end < start )
		return -1;

	SrcLen		= s_strlen( src );

	if( srcstart < 0 )
		srcstart   += SrcLen;

	Len	= end - start + 1;

	if( start < 0 )
		{
		Len			= end - 0 + 1;
		srcstart   -= start;
		start		= 0;
		}

	if( srcstart < 0 )
		{
		LeftFill	= ssmin( LeftFill - srcstart, Len );
		srcstart	= 0;
		}

	if( srcstart > SrcLen )
		srcstart	= SrcLen;

	if( srcstart + Len >= SrcLen )
		RightFill	= SrcLen - srcstart;

	/* There are one or more gaps to fill, but the filler is zero... */
	if( filler == 0 && ( LeftFill > 0 || RightFill > 0 || start > DstLen ))
		/* ...the operation is not possible. */
		return -1;

	if(( DstPtr = s_cstr( dst )) == NULL )
		return -1;

	s_constcstr( src, 0 );

	if( start > DstLen )
		memset( &DstPtr[DstLen], filler, start - DstLen );

	if( LeftFill > 0 )
		{
		memset( &DstPtr[start], filler, LeftFill );
		start  += LeftFill;
		}

	memcpy( &DstPtr[start], &SrcPtr[srcstart], Len - LeftFill - RightFill );
	start  += Len - LeftFill - RightFill;

	if( RightFill > 0 )
		memset( &DstPtr[start], filler, RightFill );

	return Len;
#endif
	}
/*=========================================================================*//**
<pre>
s_replace*
--------------------------------------------------------------------------------
        +---+---+            +---+---+
src:    | A | B |            | A | B |
        +---+---+            +---+---+
                             +---+     <- de - ds + 1
                        =>
        +---+---+---+        +---+---+---+        ds = dststart
dst:    | C | D | E |        | C | A | E |        de = dstend
        +---+---+---+        +---+---+---+
            ds  de               ds  de

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_replace_ec( s_string_t * restrict dst, ssize_t start, ssize_t end, const char * restrict src, int filler )
	{
	ssize_t	SrcLen, DstLen, DstMaxLen, Len, SrcStart;
	char	*DstPtr;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable || src == NULL )
		return -1;

	if( start >= ( DstMaxLen = s_strmaxlen( dst )))
		return -1;

	if( end < -( DstLen = s_strlen( dst )))
		return -1;

	SrcStart	= 0;

	if( start < -DstLen )
		{
		SrcStart= -( start + DstLen );
		start	= 0;
		}
	else if( start < 0 )
		start  += DstLen;

	if( end >= DstMaxLen )
		end		= DstMaxLen - 1;
	else if( end < 0 )
		end	   += DstLen;

	SrcLen = strlen( src );

	if( start > end || SrcStart >= SrcLen )
		return -1;

	Len			= ssmin( end - start + 1, SrcLen - SrcStart );

	DstPtr		= s_cstr( dst );

	if( start > DstLen )
		{
		if( filler == 0 )
			return -1;
		else
			memset( DstPtr, filler, start - DstLen );
		}

	memcpy( DstPtr + start, src + SrcStart, Len );

	if( start + Len > DstLen )
		{
		*DstPtr	= '\0';
		SetUsedLen( dst, start + Len );
		}

	return Len;
	}
/*=========================================================================*//**
<pre>
s_replace_l
--------------------------------------------------------------------------------
            ss                       ss                   ss = srcstart
        +---+-------+---+        +---+-------+---+
src:    | A |   B   | C |        | A |   B   | C |
        +---+-------+---+        +---+-------+---+
			+--len--+                +--len--+

                            =>

			+--len--+                +--len--+
        +---+-------+---+        +---+-------+---+        ds = dststart
dst:    | D |   E   | F |        | D |   B   | F |
        +---+-------+---+        +---+-------+---+
            ds                       ds

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_replace_l( s_string_t * restrict dst, ssize_t start, ssize_t len, const s_string_t * restrict src, ssize_t srcstart, int filler )
	{
	ssize_t	SrcLen, DstLen, DstMaxLen, SrcStart;
	char	*DstPtr;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 || src == NULL || src->MustBeZero != 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == src )
		return -1;

	if( len < 0 )
		return 0;

	if( start >= ( DstMaxLen = s_strmaxlen( dst )))
		return 0;

	if( start + len > DstMaxLen )
		len		= DstMaxLen - start;

	DstLen		= s_strlen( dst );

	SrcStart	= 0;

	if( start < -DstLen )
		{
		SrcStart= -( start + DstLen );
		start	= 0;
		}
	else if( start < 0 )
		start  += DstLen;

	SrcLen = s_strlen( src );

	if( SrcStart >= SrcLen )
		return 0;

	len			= ssmin( len, SrcLen - SrcStart );
	DstPtr		= s_cstr( dst );

	if( start > DstLen )
		{
		if( filler == 0 )
			return -1;
		else
			memset( DstPtr, filler, start - DstLen );
		}

	memcpy( DstPtr + start, s_constcstr( src, SrcStart ), len );

	if( start + len > DstLen )
		{
		*DstPtr	= '\0';
		SetUsedLen( dst, start + len );
		}

	return len;
	}
/*=========================================================================*//**
<pre>
s_replace_lc
--------------------------------------------------------------------------------
        +-------+---+            +-------+---+
src:    |   A   | B |            |   A   | B |
        +-------+---+            +-------+---+
        +--len--+                +--len--+

                            =>

			+--len--+                +--len--+
        +---+-------+---+        +---+-------+---+        ds = dststart
dst:    | C |   D   | E |        | C |   A   | E |
        +---+-------+---+        +---+-------+---+
            ds                       ds

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_replace_lc( s_string_t * restrict dst, ssize_t start, ssize_t len, const char * restrict src, int filler )
	{
	ssize_t	SrcLen, DstLen, DstMaxLen, SrcStart;
	char	*DstPtr;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 || src == NULL )
		return -1;

	if( len < 0 )
		return 0;

	if( start >= ( DstMaxLen = s_strmaxlen( dst )))
		return 0;

	if( start + len > DstMaxLen )
		len		= DstMaxLen - start;

	DstLen		= s_strlen( dst );

	SrcStart	= 0;

	if( start < -DstLen )
		{
		SrcStart= -( start + DstLen );
		start	= 0;
		}
	else if( start < 0 )
		start  += DstLen;

	SrcLen = strlen( src );

	if( SrcStart >= SrcLen )
		return 0;

	len			= ssmin( len, SrcLen - SrcStart );
	DstPtr		= s_cstr( dst );

	if( start > DstLen )
		{
		if( filler == 0 )
			return -1;
		else
			memset( DstPtr, filler, start - DstLen );
		}

	memcpy( DstPtr + start, src + SrcStart, len );

	if( start + len > DstLen )
		{
		*DstPtr	= '\0';
		SetUsedLen( dst, start + len );
		}

	return len;
	}
/*============================================================================*/
ssize_t s_strlwr_e( s_string_t * restrict str, ssize_t start, ssize_t end )
	{
	char	*Ptr;
	ssize_t	UsedLen;
	ssize_t	Index, Count;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	UsedLen	= s_strlen( str );

	if( UsedLen == 0 )
		return 0;

	if( start < -UsedLen || start >= UsedLen )
		return -1;

	if( start < 0 )
		start  += UsedLen;

	if( end < -UsedLen || end >= UsedLen )
		return -1;

	if( end < 0 )
		end  += UsedLen;

	if( end < start )
		return -1;

	if(( Ptr = s_cstr( str )) == NULL )
		return -1;

	for( Count = 0, Index = start; Index <= end; Index++ )
		if( isupper( Ptr[Index] ))
			{
			Count++;
			Ptr[Index]	= tolower( Ptr[Index] );
			}

	return Count;
	}
/*============================================================================*/
ssize_t s_strlwr_l( s_string_t * restrict str, ssize_t start, ssize_t len )
	{
	char	*Ptr;
	ssize_t	UsedLen;
	ssize_t	Index, Count;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	UsedLen	= s_strlen( str );

	if( UsedLen == 0 || len == 0 )
		return 0;

	if( start >= UsedLen || len < 0 )
		return -1;

	if( start < 0 )
		start  += UsedLen;

	if( start < 0 )
		{
		if(( len += start ) <= 0 )
			return 0;
		start	= 0;
		}

	if( start + len > UsedLen )
		len		= UsedLen - start;

	if(( Ptr = s_cstr( str )) == NULL )
		return -1;

	for( Count = 0, Index = start; Index < start + len; Index++ )
		if( isupper( Ptr[Index] ))
			{
			Count++;
			Ptr[Index]	= tolower( Ptr[Index] );
			}

	return Count;
	}
/*============================================================================*/
ssize_t s_strupr_e( s_string_t * restrict str, ssize_t start, ssize_t end )
	{
	char	*Ptr;
	ssize_t	UsedLen;
	ssize_t	Index, Count;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	UsedLen	= s_strlen( str );

	if( UsedLen == 0 )
		return 0;

	if( start < -UsedLen || start >= UsedLen )
		return -1;

	if( start < 0 )
		start  += UsedLen;

	if( end < -UsedLen || end >= UsedLen )
		return -1;

	if( end < 0 )
		end  += UsedLen;

	if( end < start )
		return -1;

	if(( Ptr = s_cstr( str )) == NULL )
		return -1;

	for( Count = 0, Index = start; Index <= end; Index++ )
		if( islower( Ptr[Index] ))
			{
			Count++;
			Ptr[Index]	= toupper( Ptr[Index] );
			}

	return Count;
	}
/*============================================================================*/
ssize_t s_strupr_l( s_string_t * restrict str, ssize_t start, ssize_t len )
	{
	char	*Ptr;
	ssize_t	UsedLen;
	ssize_t	Index, Count;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	UsedLen	= s_strlen( str );

	if( UsedLen == 0 || len == 0 )
		return 0;

	if( start >= UsedLen || len < 0 )
		return -1;

	if( start < 0 )
		start  += UsedLen;

	if( start < 0 )
		{
		if(( len += start ) <= 0 )
			return 0;
		start	= 0;
		}

	if( start + len > UsedLen )
		len		= UsedLen - start;

	if(( Ptr = s_cstr( str )) == NULL )
		return -1;

	for( Count = 0, Index = start; Index < start + len; Index++ )
		if( islower( Ptr[Index] ))
			{
			Count++;
			Ptr[Index]	= toupper( Ptr[Index] );
			}

	return Count;
	}
/*=========================================================================*//**
<pre>
s_ltrim
--------------------------------------------------------------------------------
charset: x..z


        +---+-----+---+                    +---+---+    st = start
str:    | A | cs* | B |                =>  | A | B |    cs*= characters from charset
        +---+-----+---+                    +---+---+
            st                                 st

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
#if 0
/*=========================================================================*//**
s_ltrim Tested OK!!!
*//*==========================================================================*/
#endif
ssize_t s_ltrim_s( s_string_t * restrict str, ssize_t start, const s_string_t * restrict delim, ssize_t delimstart )
	{
	char	*Ptr;
	ssize_t	Len, Index;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( str == delim )
		return -1;

	Len	= s_strlen( str );

	if( Len == 0 || start >= Len )
		return 0;

	if( start < -Len )
		start	= 0;
	else if( start < 0 )
		start  += Len;

	if(( Ptr = s_cstr( str )) == NULL )
		return -1;

	if( delim == NULL )
		{
		for( Index = start; Index < Len && isspace( Ptr[Index] ); Index++ )
			{}
		}
	else
		{
		const char	*DelimPtr;
		ssize_t		DelimLen;

		if( delim->MustBeZero != 0 )
			return -1;

		if(( DelimLen = s_strlen( delim )) <= 0 )
			return -1;

		if( delimstart < -DelimLen || delimstart >= DelimLen )
			return -1;

		if( delimstart < 0 )
			delimstart += DelimLen;

		DelimLen   -= delimstart;

		DelimPtr	= s_constcstr( delim, delimstart );

		for( Index = start; Index < Len; Index++ )
			{
			ssize_t	j;

			for( j = 0; j < DelimLen && Ptr[Index] != DelimPtr[j] ; j++ )
				{}
			if( j >= DelimLen )
				break;
			}
		}

	if( Index == start )
		return 0;

	memmove( &Ptr[start], &Ptr[Index], Len - Index );

	Ptr[start+Len-Index]	= '\0';

	SetUsedLen( str, start+Len-Index );

	return Index - start;
	}
/*=========================================================================*//**
<pre>
s_ltrim_c
--------------------------------------------------------------------------------
charset: x..z


        +---+-----+---+                    +---+---+    st = start
str:    | A | cs* | B |                =>  | A | B |    cs*= characters from charset
        +---+-----+---+                    +---+---+
            st                                 st

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
#if 0
/*=========================================================================*//**
s_ltrim_c Tested OK!!!
*//*==========================================================================*/
#endif
ssize_t s_ltrim_c( s_string_t * restrict str, ssize_t start, const char * restrict delim )
	{
	char	*Ptr;
	ssize_t	Len, Index;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	Len	= s_strlen( str );

	if( Len == 0 || start >= Len )
		return 0;

	if( start < -Len )
		start	= 0;
	else if( start < 0 )
		start  += Len;

	if(( Ptr = s_cstr( str )) == NULL )
		return -1;

	if( delim == NULL )
		{
		for( Index = start; Index < Len && isspace( Ptr[Index] ); Index++ )
			{}
		}
	else
		{
		ssize_t		DelimLen;

		if(( DelimLen = strlen( delim )) == 0 )
			return -1;

		for( Index = start; Index < Len; Index++ )
			{
			ssize_t	j;

			for( j = 0; j < DelimLen && Ptr[Index] != delim[j] ; j++ )
				{}
			if( j >= DelimLen )
				break;
			}
		}

	if( Index == start )
		return 0;

	memmove( &Ptr[start], &Ptr[Index], Len - Index );

	Ptr[start+Len-Index]	= '\0';

	SetUsedLen( str, start+Len-Index );

	return Index - start;
	}
/*=========================================================================*//**
<pre>
s_rtrim
--------------------------------------------------------------------------------
charset: x..z


        +---+-----+---+                    +---+---+    st = start
str:    | A | cs* | B |                =>  | A | B |    cs*= characters from charset
        +---+-----+---+                    +---+---+
                  st                           st

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*=========================================================================*//**
s_rtrim Tested OK!!!
*//*==========================================================================*/
ssize_t s_rtrim_s( s_string_t * restrict str, ssize_t start, const s_string_t * restrict delim, ssize_t delimstart )
	{
	char	*Ptr;
	ssize_t	Len, Index;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( str == delim )
		return -1;

	Len	= s_strlen( str );

	if( Len == 0 || start < -Len )
		return 0;

	if( start >= Len )
		start	= Len - 1;
	else if( start < 0 )
		start  += Len;

	if(( Ptr = s_cstr( str )) == NULL )
		return -1;

	if( delim == NULL )
		{
		for( Index = Len - 1; Index >= start && isspace( Ptr[Index] ); Index-- )
			{}
		}
	else
		{
		const char	*DelimPtr;
		ssize_t		DelimLen;

		if( delim->MustBeZero != 0 )
			return -1;

		if(( DelimLen = s_strlen( delim )) <= 0 )
			return -1;

		if( delimstart < -DelimLen || delimstart >= DelimLen )
			return -1;

		if( delimstart < 0 )
			delimstart += DelimLen;

		DelimLen   -= delimstart;

		DelimPtr = s_constcstr( delim, delimstart );

		for( Index = Len - 1; Index >= start; Index-- )
			{
			ssize_t	j;

			for( j = 0; j < DelimLen && Ptr[Index] != DelimPtr[j] ; j++ )
				{}
			if( j >= DelimLen )
				break;
			}
		}

	if( Index == Len - 1 )
		return 0;

	Ptr[Index+1]	= '\0';

	SetUsedLen( str, Index + 1 );

	return Len - Index - 1;
	}
/*=========================================================================*//**
<pre>
s_rtrim
--------------------------------------------------------------------------------
charset: x..z


        +---+-----+---+                    +---+---+    st = start
str:    | A | cs* | B |                =>  | A | B |    cs*= characters from charset
        +---+-----+---+                    +---+---+
                  st                           st

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*=========================================================================*//**
s_rtrim_c Tested OK!!!
*//*==========================================================================*/
ssize_t s_rtrim_c( s_string_t * restrict str, ssize_t start, const char * restrict delim )
	{
	char	*Ptr;
	ssize_t	Len, Index;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	Len	= s_strlen( str );

	if( Len == 0 || start < -Len )
		return 0;

	if( start >= Len )
		start	= Len - 1;
	else if( start < 0 )
		start  += Len;

	if(( Ptr = s_cstr( str )) == NULL )
		return -1;

	if( delim == NULL )
		{
		for( Index = Len - 1; Index >= start && isspace( Ptr[Index] ); Index-- )
			{}
		}
	else
		{
		ssize_t		DelimLen;

		if(( DelimLen = strlen( delim )) <= 0 )
			return -1;

		for( Index = Len - 1; Index >= start; Index-- )
			{
			ssize_t	j;

			for( j = 0; j < DelimLen && Ptr[Index] != delim[j] ; j++ )
				{}
			if( j >= DelimLen )
				break;
			}
		}

	if( Index == Len - 1 )
		return 0;

	Ptr[Index+1]	= '\0';

	SetUsedLen( str, Index + 1 );

	return Len - Index - 1;
	}
/*=========================================================================*//**
<pre>
s_alltrim_e
--------------------------------------------------------------------------------
delim:          x..z
replacement:    '#'

        +---+-----+---+-----+---+-----+---+        +---+---+-+---+---+    s  = start
str:    | A | cs* | B | cs* | C | cs* | D |    =>  | A | B |#| C | D |    e  = end
        +---+-----+---+-----+---+-----+---+        +---+---+-+---+---+    cs*= characters from delim
            s                         e

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_alltrim_e( s_string_t * restrict str, ssize_t start, ssize_t end, const s_string_t * restrict delim, ssize_t delimstart, int replacement )
	{
	char	*Ptr;
	ssize_t	Len, Index, IndexPut;
	int		PreviousWasSpace	= -1;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( str == delim )
		return -1;

	Len	= s_strlen( str );

	if( start >= Len || end < -Len )
		return -1;

	if( start < 0 && ( start += Len ) < 0 )
		start	= 0;

	if( end < 0 )
		end    += Len;
	else if( end > Len )
		end		= Len;

	if( end < start )
		return -1;

	Ptr	= s_cstr( str );

	if( delim == NULL )
		{
		for( IndexPut = Index = start; Index <= end; )
			{
			if( isspace( Ptr[Index] ))
				{
				if( !PreviousWasSpace )
					{
					Ptr[IndexPut++]		= replacement != '\0' ? replacement : Ptr[Index];
					PreviousWasSpace	= 1;
					}
				Index++;
				}
			else
				{
				if( IndexPut != Index )
					Ptr[IndexPut]	= Ptr[Index];
				IndexPut++;
				Index++;
				PreviousWasSpace	= 0;
				}
			}
		}
	else
		{
		const char	*DelimPtr;
		ssize_t		DelimLen;

		if( delim->MustBeZero != 0 )
			return -1;

		if(( DelimLen = s_strlen( delim )) <= 0 )
			return -1;

		if( delimstart < -DelimLen || delimstart >= DelimLen )
			return -1;

		if( delimstart < 0 )
			delimstart += DelimLen;

		DelimPtr = s_constcstr( delim, 0 );

		for( IndexPut = Index = start; Index <= end; )
			{
			if( strchr( &DelimPtr[delimstart], Ptr[Index] ) != NULL )
				{
				if( !PreviousWasSpace )
					{
					Ptr[IndexPut++]		= replacement != '\0' ? replacement : Ptr[Index];
					PreviousWasSpace	= 1;
					}
				Index++;
				}
			else
				{
				if( IndexPut != Index )
					Ptr[IndexPut]	= Ptr[Index];
				IndexPut++;
				Index++;
				PreviousWasSpace	= 0;
				}
			}
		}

	if( IndexPut > 0 && PreviousWasSpace > 0 )
		IndexPut--;

	while( Index < Len )
		Ptr[IndexPut++]	= Ptr[Index++];

	if( IndexPut != Len )
		{
		Ptr[IndexPut]	= '\0';
		SetUsedLen( str, IndexPut );
		}

	return Len - IndexPut;
	}
/*=========================================================================*//**
<pre>
s_alltrim
--------------------------------------------------------------------------------
delim:          x..z
replacement:    '#'

        +---+-----+---+-----+---+-----+---+        +---+---+-+---+---+    s  = start
str:    | A | cs* | B | cs* | C | cs* | D |    =>  | A | B |#| C | D |    e  = end
        +---+-----+---+-----+---+-----+---+        +---+---+-+---+---+    cs*= characters from delim
            s                         e

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_alltrim_ec( s_string_t * restrict str, ssize_t start, ssize_t end, const char * restrict delim, int replacement )
	{
	char	*Ptr;
	ssize_t	Len, Index, IndexPut;
	int		PreviousWasSpace	= -1;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	Len	= s_strlen( str );

	if( start >= Len || end < -Len )
		return -1;

	if( start < 0 && ( start += Len ) < 0 )
		start	= 0;

	if( end < 0 )
		end    += Len;
	else if( end > Len )
		end		= Len;

	if( end < start )
		return -1;

	Ptr	= s_cstr( str );

	if( delim == NULL )
		{
		for( IndexPut = Index = start; Index <= end; )
			{
			if( isspace( Ptr[Index] ))
				{
				if( !PreviousWasSpace )
					{
					Ptr[IndexPut++]		= replacement != '\0' ? replacement : Ptr[Index];
					PreviousWasSpace	= 1;
					}
				Index++;
				}
			else
				{
				if( IndexPut != Index )
					Ptr[IndexPut]	= Ptr[Index];
				IndexPut++;
				Index++;
				PreviousWasSpace	= 0;
				}
			}
		}
	else
		{
		ssize_t		DelimLen;

		if(( DelimLen = strlen( delim )) <= 0 )
			return -1;

		for( IndexPut = Index = start; Index <= end; )
			{
			if( strchr( delim, Ptr[Index] ) != NULL )
				{
				if( !PreviousWasSpace )
					{
					Ptr[IndexPut++]		= replacement != '\0' ? replacement : Ptr[Index];
					PreviousWasSpace	= 1;
					}
				Index++;
				}
			else
				{
				if( IndexPut != Index )
					Ptr[IndexPut]	= Ptr[Index];
				IndexPut++;
				Index++;
				PreviousWasSpace	= 0;
				}
			}
		}

	if( IndexPut > 0 && PreviousWasSpace > 0 )
		IndexPut--;

	while( Index < Len )
		Ptr[IndexPut++]	= Ptr[Index++];

	if( IndexPut != Len )
		{
		Ptr[IndexPut]	= '\0';
		SetUsedLen( str, IndexPut );
		}

	return Len - IndexPut;
	}
/*=========================================================================*//**
<pre>
s_remove_e
--------------------------------------------------------------------------------
charset:    " *-,."

        +----------------------------------+        +--------------------------+    s  = start
str:    |+Some Text. More Text, *1234*XYZD*|    =>  |+SomeTextMoreText1234XYZD*|    e  = end
        +---+--------------------------+---+        +---+----------------------+
            s                          e                s

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_remove_e( s_string_t * restrict str, ssize_t start, ssize_t end, const s_string_t * restrict delim, ssize_t delimstart )
	{
	char	*Ptr;
	ssize_t	Len, Index, IndexPut;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( str == delim )
		return -1;

	Len	= s_strlen( str );

	if( start >= Len || end < -Len )
		return -1;

	if( start < 0 && ( start += Len ) < 0 )
		start	= 0;

	if( end < 0 )
		end    += Len;
	else if( end > Len )
		end		= Len;

	if( end < start )
		return -1;

	Ptr	= s_cstr( str );

	if( delim == NULL )
		{
		for( IndexPut = Index = start; Index <= end; )
			{
			if( isspace( Ptr[Index] ))
				Index++;
			else
				{
				if( IndexPut != Index )
					Ptr[IndexPut]	= Ptr[Index];
				IndexPut++;
				Index++;
				}
			}
		}
	else
		{
		const char	*DelimPtr;
		ssize_t		DelimLen;

		if( delim->MustBeZero != 0 )
			return -1;

		if(( DelimLen = s_strlen( delim )) <= 0 )
			return -1;

		if( delimstart < -DelimLen || delimstart >= DelimLen )
			return -1;

		if( delimstart < 0 )
			delimstart += DelimLen;

		DelimPtr = s_constcstr( delim, 0 );

		for( IndexPut = Index = start; Index <= end; )
			{
			if( strchr( &DelimPtr[delimstart], Ptr[Index] ) != NULL )
				Index++;
			else
				{
				if( IndexPut != Index )
					Ptr[IndexPut]	= Ptr[Index];
				IndexPut++;
				Index++;
				}
			}
		}

	while( Index < Len )
		Ptr[IndexPut++]	= Ptr[Index++];

	if( IndexPut != Len )
		{
		Ptr[IndexPut]	= '\0';
		SetUsedLen( str, IndexPut );
		}

	return Len - IndexPut;
	}
/*=========================================================================*//**
<pre>
s_remove_ec
--------------------------------------------------------------------------------
charset:    " *-,."

        +----------------------------------+        +--------------------------+    s  = start
str:    |+Some Text. More Text, *1234*XYZD*|    =>  |+SomeTextMoreText1234XYZD*|    e  = end
        +---+--------------------------+---+        +---+----------------------+
            s                          e                s

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
ssize_t s_remove_ec( s_string_t * restrict str, ssize_t start, ssize_t end, const char * restrict delim )
	{
	char	*Ptr;
	ssize_t	Len, Index, IndexPut;

	if( str == NULL || str->MustBeZero != 0 || str->Writable == 0 )
		return -1;

	Len	= s_strlen( str );

	if( start >= Len || end < -Len )
		return -1;

	if( start < 0 && ( start += Len ) < 0 )
		start	= 0;

	if( end < 0 )
		end    += Len;
	else if( end > Len )
		end		= Len;

	if( end < start )
		return -1;

	Ptr	= s_cstr( str );

	if( delim == NULL )
		{
		for( IndexPut = Index = start; Index <= end; )
			{
			if( isspace( Ptr[Index] ))
				Index++;
			else
				{
				if( IndexPut != Index )
					Ptr[IndexPut]	= Ptr[Index];
				IndexPut++;
				Index++;
				}
			}
		}
	else
		{
		ssize_t		DelimLen;

		if(( DelimLen = strlen( delim )) <= 0 )
			return -1;

		for( IndexPut = Index = start; Index <= end; )
			{
			if( strchr( delim, Ptr[Index] ) != NULL )
				Index++;
			else
				{
				if( IndexPut != Index )
					Ptr[IndexPut]	= Ptr[Index];
				IndexPut++;
				Index++;
				}
			}
		}

	while( Index < Len )
		Ptr[IndexPut++]	= Ptr[Index++];

	if( IndexPut != Len )
		{
		Ptr[IndexPut]	= '\0';
		SetUsedLen( str, IndexPut );
		}

	return Len - IndexPut;
	}
/*=========================================================================*//**
<pre>
s_insert_l
--------------------------------------------------------------------------------
            ss                       ss                   ss = srcstart
        +---+-------+---+        +---+-------+---+        se = srcend
src:    | A |   B   | C |        | A |   B   | C |
        +---+-------+---+        +---+-------+---+
            +--len--+                +--len--+
                            =>
                                     +--len--+
        +---+---+                +---+-------+---+        ds = dststart
dst:    | D | E |                | D |   B   | E |
        +---+---+                +---+-------+---+
            ds                       ds

--------------------------------------------------------------------------------
</pre>
*//*==========================================================================*/
/*============================================================================*/
/*TODO*/
ssize_t s_insert_l( s_string_t * restrict dst, ssize_t dststart, const s_string_t * restrict src, ssize_t srcstart, ssize_t len, int filler )
	{
	ssize_t	SrcLen, DstLen, DstMaxLen;
	ssize_t	Offset;
	char	*DstPtr;

	if( dst == NULL || dst->MustBeZero != 0 || dst->Writable == 0 || src == NULL || src->MustBeZero != 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == src )
		return -1;

	if( len <= 0 )
		return 0;

	if( dststart >= ( DstMaxLen = s_strmaxlen( dst )))
		return 0;

	if( srcstart >= ( SrcLen = s_strlen( src )))
		return 0;

	if( dststart + len > DstMaxLen )
		len			= DstMaxLen - dststart;

	if( srcstart + len > SrcLen )
		len			= SrcLen - srcstart;

	DstLen			= s_strlen( dst );

	Offset	= ssmin( dststart + DstLen, srcstart + SrcLen );
	if( Offset < 0 )
		len		   += Offset;

	if( dststart < -DstLen )
		dststart	= Offset -( dststart + DstLen );
	else if( dststart < 0 )
		dststart   += DstLen;

	if( srcstart < -SrcLen )
		{
		dststart   += -( srcstart + SrcLen );
		srcstart	= 0;
		}
	else if( srcstart < 0 )
		srcstart   += SrcLen;

	if( srcstart >= SrcLen || dststart >= DstLen )
		return 0;

	len				= ssmin( len, SrcLen - srcstart );
	DstPtr			= s_cstr( dst );

	if( dststart > DstLen )
		{
		if( filler == 0 )
			return -1;
		else
			memset( DstPtr, filler, dststart - DstLen );
		}

	memcpy( DstPtr + dststart, s_constcstr( src, srcstart ), len );

	if( dststart + len > DstLen )
		{
		*DstPtr	= '\0';
		SetUsedLen( dst, dststart + len );
		}

	return len;
	}
/*============================================================================*/
/* I'm tempted to remove this function, because the format string cannot be checked by the compiler. */
/*============================================================================*/
int s_sprintf( s_string_t * restrict dst, ssize_t dststart, int filler, const s_string_t * restrict fmt, ssize_t fmtstart, ... )
	{
	va_list		v;
	int			n;
	char		*DstPtr;
	const char	*FmtPtr;
	ssize_t		MaxLen, UsedLen, FmtLen;

	if( dst == NULL || dst->Writable == 0 || dst->MustBeZero != 0 || fmt == NULL || fmt->MustBeZero != 0 )
		return -1;

	/* 'restrict' is not enough, damn compiler just emits a warning... */
	if( dst == fmt )
		return -1;

	if(( MaxLen = s_strmaxlen( dst )) <= 0 )
		return -1;

	UsedLen	= s_strlen( dst );

	if( dststart < -UsedLen || dststart >= MaxLen || ( dststart > UsedLen && filler == 0 ))
		return -1;

	if( dststart < 0 )
		dststart   += UsedLen;

	FmtLen	= s_strlen( fmt );

	if( fmtstart < -FmtLen || fmtstart >= FmtLen )
		return -1;

	if( fmtstart < 0 )
		fmtstart   += FmtLen;

	if(( DstPtr = s_cstr( dst )) == NULL )
		return -1;

	FmtPtr = s_constcstr( fmt, fmtstart );

	va_start( v, fmtstart );

	n	= vsnprintf( DstPtr, MaxLen - dststart + 1, FmtPtr, v );

	va_end( v );

	if( n >= 0 )
		{
		if( dststart > UsedLen )
			memset( &DstPtr[UsedLen], filler, dststart - UsedLen );

		UsedLen	= ssmin( dststart + n, MaxLen );
		SetUsedLen( dst, UsedLen );
		}

	return n;
	}
/*============================================================================*/
int
#if			!defined DOXYGEN
__attribute__((format(printf, 4, 5)))
#endif	/*	!defined DOXYGEN */
s_sprintf_c( s_string_t * restrict dst, ssize_t dststart, int filler, const char * restrict fmt, ... )
	{
	va_list	v;
	int		n;
	char	*DstPtr;
	ssize_t	MaxLen, UsedLen;

	if( dst == NULL || dst->Writable == 0 || dst->MustBeZero != 0 || fmt == NULL )
		return -1;

	if(( MaxLen = s_strmaxlen( dst )) <= 0 )
		return -1;

	UsedLen	= s_strlen( dst );

	if( dststart < -UsedLen || dststart >= MaxLen || ( dststart > UsedLen && filler == 0 ))
		return -1;

	if( dststart < 0 )
		dststart  += UsedLen;

	if(( DstPtr = s_cstr( dst )) == NULL )
		return -1;

	va_start( v, fmt );

	n	= vsnprintf( DstPtr, MaxLen - dststart + 1, fmt, v );

	va_end( v );

	if( n >= 0 )
		{
		if( dststart > UsedLen )
			memset( &DstPtr[UsedLen], filler, dststart - UsedLen );

		UsedLen	= ssmin( dststart + n, MaxLen );
		SetUsedLen( dst, UsedLen );
		}

	return n;
	}
/*============================================================================*/
#if			!defined DOXYGEN
/*============================================================================*/
ssize_t _s_calcsize( ssize_t len )
	{
	int Bytes;

	if( len < ( 1ull <<  8 ) - 1 )
		Bytes	= 1;
	else if( len < ( 1ull << 16 ) - 1 )
		Bytes	= 2;
	else if( len < ( 1ull << 32 ) - 1 )
		Bytes	= 4;
	else
		Bytes	= 8;

	/* One byte for the descriptor + 2 bytes for each size counter + len bytes for the string + 1 byte for the ending NULL character. */
	return 1 + 2 * Bytes + len + 1;
	}
/*============================================================================*/
void _s_string_init( s_string_t *str, ssize_t len, int area )
	{
	int		Log2Bytes;
	char	*Ptr;

	if( len < ( 1ull <<  8 ) - 1 )
		Log2Bytes	= 0;
	else if( len < ( 1ull << 16 ) - 1 )
		Log2Bytes	= 1;
	else if( len < ( 1ull << 32 ) - 1 )
		Log2Bytes	= 2;
	else
		Log2Bytes	= 3;

	str->MustBeZero = 0;
	str->Sizes		= 1;	/* We will have two size counters (maximum and used). */
	str->Bits		= Log2Bytes;
	str->Area		= area; /* The memory area that the string is allocated (0=bss, 1=stack, 2=heap). */
	str->Writable	= 1;	/* The string is writable (at least for now). */

	SetMaxLen( str, len );
	SetUsedLen( str, 0 );

	if(( Ptr = s_cstr( str )) != NULL )
		*Ptr		= '\0';
	}
/*============================================================================*/
void _s_string_array_init( s_string_t * const str[], uint8_t *buffer, ssize_t count, ssize_t len, int area )
	{
	int				Log2Bytes;
	ssize_t			i;
	const ssize_t	Size	= _s_calcsize( len );

	if( len < ( 1ull <<  8 ) - 1 )
		Log2Bytes	= 0;
	else if( len < ( 1ull << 16 ) - 1 )
		Log2Bytes	= 1;
	else if( len < ( 1ull << 32 ) - 1 )
		Log2Bytes	= 2;
	else
		Log2Bytes	= 3;

	for( i = 0; i < count; i++ )
		{
		char	*Ptr;

		*(s_string_t**)( &str[i] )	= (s_string_t*)( buffer + i * Size );
		str[i]->MustBeZero			= 0;
		str[i]->Sizes				= 1;			/* We will have two size counters (maximum and used). */
		str[i]->Bits				= Log2Bytes;
		str[i]->Area				= area; 		/* The memory area that the string is allocated (0=bss, 1=stack, 2=heap). */
		str[i]->Writable			= 1;			/* The string is writable (at least for now). */

		SetMaxLen( str[i], len );
		SetUsedLen( str[i], 0 );

		if(( Ptr = s_cstr( str[i] )) != NULL )
			*Ptr					= '\0';
		}
	}
/*============================================================================*/
/* Create a dummy variable in the section "s_string_init". This section holds
the initialization information for all the static s_strings. If we do not create
the dummy variable and no other static s_string exists, the section will not be
created. */
static const void __attribute__((section("s_string_init"),used)) *s_string_init_dummy	= (void*)0x123456789abcdef0;

/* This symbol will be created by the linker with the address to the start of
section "s_string_init". If the section doesn't exist, this symbol will not be
created, resulting in a linker error. */
extern const void *__start_s_string_init;

/* This symbol will be created by the linker with the address to the end of
section "s_string_init". If the section doesn't exist, this symbol will not be
created, resulting in a linker error. */
extern const void *__stop_s_string_init;
/*============================================================================*/
/* This function will be called before function 'main', to initialize all static
s_string objects. */

__attribute__((constructor)) void __s_string_global_init( void )
	{
	const void	**p;

	for( p = &__start_s_string_init; p < &__stop_s_string_init; p++ )
		{
#if			__SIZEOF_POINTER__ > 4
		uint64_t	Temp	= (uint64_t)*p;
		uint64_t	Length	= Temp >> 8;
#else	/*	__SIZEOF_POINTER__ > 4 */
		uint32_t	Temp	= (uint32_t)*p;
		uint32_t	Length	= Temp >> 8;
#endif	/*	__SIZEOF_POINTER__ > 4 */

		switch( (uint8_t)Temp )
			{
			default:
				break;

				if( 0 )
					{
			case 0x21:
					Length	= Length > ( 1uLL <<  8 ) - 2 ? ( 1uLL <<  8 ) - 2 : Length;
					}
				else if( 0 )
					{
			case 0x23:
					Length	= Length > ( 1uLL << 16 ) - 2 ? ( 1uLL << 16 ) - 2 : Length;
					}
				else if( 0 )
					{
			case 0x25:
					Length	= Length > ( 1uLL << 32 ) - 2 ? ( 1uLL << 32 ) - 2 : Length;
					}
				{
				s_string_t	*Str	= (s_string_t*)*++p;
				_s_string_init( Str, Length, 0 );
				s_strcpy_c( Str, *++p );
				break;
				}

				if( 0 )
					{
			case 0x22:
					Length	= Length > ( 1uLL <<  8 ) - 2 ? ( 1uLL <<  8 ) - 2 : Length;
					}
				else if( 0 )
					{
			case 0x24:
					Length	= Length > ( 1uLL << 16 ) - 2 ? ( 1uLL << 16 ) - 2 : Length;
					}
				else if( 0 )
					{
			case 0x26:
					Length	= Length > ( 1uLL << 32 ) - 2 ? ( 1uLL << 32 ) - 2 : Length;
					}
				{
				s_string_t	*Str	= (s_string_t*)*++p;
				_s_string_init( Str, Length, 0 );
				s_strcpy_c( Str, "" );
				break;
				}

				if( 0 )
					{
			case 0x2a:
					Length	= Length > ( 1uLL <<  8 ) - 2 ? ( 1uLL <<  8 ) - 2 : Length;
					}
				else if( 0 )
					{
			case 0x2c:
					Length	= Length > ( 1uLL << 16 ) - 2 ? ( 1uLL << 16 ) - 2 : Length;
					}
				else if( 0 )
					{
			case 0x2e:
					Length	= Length > ( 1uLL << 32 ) - 2 ? ( 1uLL << 32 ) - 2 : Length;
					}
				{
				uint8_t		*Buffer	= (uint8_t*)*++p;
				s_string_t	**Array	= (s_string_t**)*++p;
				ssize_t		Count	= (ssize_t)*++p;

				_s_string_array_init( Array, Buffer, Count, Length, 0 );
				break;
				}


				if( 0 )
					{
			case 0x31:
					Length	= Length > ( 1uLL <<  8 ) - 2 ? ( 1uLL <<  8 ) - 2 : Length;
					}
				else if( 0 )
					{
			case 0x33:
					Length	= Length > ( 1uLL << 16 ) - 2 ? ( 1uLL << 16 ) - 2 : Length;
					}
				else if( 0 )
					{
			case 0x35:
					Length	= Length > ( 1uLL << 32 ) - 2 ? ( 1uLL << 32 ) - 2 : Length;
					}
				{
				s_string_t	*Str	= (s_string_t*)*++p;
				_s_string_init( Str, Length, 0 );
				s_strcpy_c( Str, *++p );
				Str->Writable	= 0;
				break;
				}
			}
		}
	}
/*============================================================================*/
#endif	/*	!defined DOXYGEN */
/*============================================================================*/
