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
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#include "s_string.h"
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
size_t s_strmaxlen( const s_string_t * restrict str )
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
		case 3:
			return (( (size_t)str->PayLoad[Offset+0] <<  0 ) | ( (size_t)str->PayLoad[Offset+1] <<  8 ) |
					( (size_t)str->PayLoad[Offset+2] << 16 ) | ( (size_t)str->PayLoad[Offset+3] << 24 ) |
					( (size_t)str->PayLoad[Offset+4] << 32 ) | ( (size_t)str->PayLoad[Offset+5] << 40 ) |
					( (size_t)str->PayLoad[Offset+6] << 48 ) | ( (size_t)str->PayLoad[Offset+7] << 56 ));
		}

	/* Should never reach here... */
	return 0;
	}
/*============================================================================*/
static int SetMaxLen( s_string_t * restrict str, size_t MaxLength )
	{
	size_t	Offset;

	if( str == NULL )
		/* ...return an error result. */
		return -1;

	/* The 'must be zero' field in this first implementation MUST BE ZERO... */
	if( str->MustBeZero != 0 )
		/* ...return an error result. */
		return -1;

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
		}

	/* Should never reach here... */
	return 0;
	}
/*============================================================================*/
size_t s_strlen( const s_string_t * restrict str )
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
		case 3:
			return ( (size_t)str->PayLoad[Offset+0] <<  0 ) | ( (size_t)str->PayLoad[Offset+1] <<  8 ) |
				   ( (size_t)str->PayLoad[Offset+2] << 16 ) | ( (size_t)str->PayLoad[Offset+3] << 24 ) |
				   ( (size_t)str->PayLoad[Offset+4] << 32 ) | ( (size_t)str->PayLoad[Offset+5] << 40 ) |
				   ( (size_t)str->PayLoad[Offset+6] << 48 ) | ( (size_t)str->PayLoad[Offset+7] << 56 );
		}

	/* Should never reach here... */
	return 0;
	}
/*============================================================================*/
size_t s_strnlen( const s_string_t * restrict str, size_t len )
	{
    return ssmin( len, s_strlen( str ));
	}
/*============================================================================*/
static int SetUsedLen( s_string_t * restrict str, size_t NewLength )
	{
	size_t	Offset;

	if( str == NULL )
		/* ...return an error result. */
		return -1;

	/* The 'must be zero' field in this first implementation MUST BE ZERO... */
	if( str->MustBeZero != 0 )
		/* ...return an error result. */
		return -1;

    if( str->Writable == 0 )
        return -1;

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
		}

	/* Should never reach here... */
	return 0;
	}
/*============================================================================*/
static char *s_cstr( s_string_t * restrict str )
	{
	size_t  NumCounters;
	size_t  Offset;

	if( str == NULL )
		return NULL;

	if( str->MustBeZero != 0 )
		return NULL;

	NumCounters	= 1 + str->Sizes;
	Offset		= ( str->Area == 3 ? sizeof( size_t ) : 0 ) + NumCounters * ( 1 << str->Bits );

	return (char*)&str->PayLoad[Offset];
	}
/*============================================================================*/
const char *s_constcstr( const s_string_t * restrict str )
	{
	size_t  NumCounters;
	size_t  Offset;

	if( str == NULL )
		return NULL;

	if( str->MustBeZero != 0 )
		return NULL;

	NumCounters	= 1 + str->Sizes;
	Offset		= ( str->Area == 3 ? sizeof( size_t ) : 0 ) + NumCounters * ( 1 << str->Bits );

	return (char*)&str->PayLoad[Offset];
	}
/*============================================================================*/
int s_charat( const s_string_t * restrict str, ssize_t index )
    {
    size_t      Len;
    const char  *Ptr;

    if( str == NULL )
        return -1;

    Len = s_strlen( str );
    if( index < -(ssize_t)Len || index >= (ssize_t)Len )
        return -1;

    if( index < 0 )
        index  += Len;

    Ptr = s_constcstr( str );
    if( Ptr == NULL )
        return -1;

    return (int)Ptr[index];
    }
/*============================================================================*/
ssize_t s_setcharat( s_string_t * restrict dst, ssize_t index, int value, int filler )
    {
    size_t  UsedLen;
    char    *Ptr;

    if( dst == NULL )
        return -1;

    if( dst->Writable == 0 )
        return -1;

    /* We can change a character beyond the current used length, as long as it
       is not beyond the maximum length. */
    if( index >= (ssize_t)s_strmaxlen( dst ))
        return -1;

    UsedLen = s_strlen( dst );

    /* We can have a negative index for indexing from the end of the string,
       but the index cannot be beyond the current used length. That is, -1
       is the last character. */
    if( index < -(ssize_t)UsedLen )
        return -1;

    /* 'index' is negative... */
    if( index < 0 )
        /* ...we must revert it, counting from the end to point to the real index. */
        index  += UsedLen;

    Ptr     = s_cstr( dst );
    if( Ptr == NULL )
        return -1;

    Ptr[index]  = (uint8_t)value;

    if( index > UsedLen )
        memset( &Ptr[UsedLen], filler, index - UsedLen );

    if( index >= UsedLen )
        {
        UsedLen = index + 1;
        SetUsedLen( dst, UsedLen );
        Ptr[index + 1]  = '\0';
        }

    return UsedLen;
    }
/*============================================================================*/
ssize_t s_appendchar( s_string_t * restrict dst, int value )
    {
    size_t  UsedLen;
    char    *Ptr;

    if( dst == NULL )
        return -1;

    if( dst->Writable == 0 )
        return -1;

    UsedLen = s_strlen( dst );

    /* We cannot grow the string beyond its maximum length. */
    if( UsedLen >= s_strmaxlen( dst ))
        return -1;

    Ptr         = s_cstr( dst );
    if( Ptr == NULL )
        return -1;

    Ptr[UsedLen]  = (uint8_t)value;

    SetUsedLen( dst, UsedLen + 1 );
    Ptr[UsedLen + 1]  = '\0';

    return UsedLen + 1;
    }
/*============================================================================*/
ssize_t s_truncate( s_string_t * restrict dst, size_t len )
    {
    size_t  UsedLen;
    char    *Ptr;

    if( dst == NULL )
        return -1;

    if( dst->Writable == 0 )
        return -1;

    UsedLen = s_strlen( dst );

    /* The new length is longer than the current length... */
    if( len >= UsedLen )
        /* ...nothing to be done. */
        return UsedLen;

    Ptr     = s_cstr( dst );
    if( Ptr == NULL )
        return -1;

    Ptr[len]    = '\0';

    SetUsedLen( dst, len );

    return len;
    }
/*============================================================================*/
int s_strcat( s_string_t * restrict dst, const s_string_t * restrict src )
	{
	size_t		DstMaxLen, DstUsedLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
    if( dst == NULL )
        /* ...let's return zero, as an inexistent string cannot have a size other than zero. */
        return 0;

    if( dst->Writable == 0 )
        return -1;

    DstUsedLen	= s_strlen( dst );

	/* The source is a null pointer... */
	if( src == NULL )
		/* ...so the destination string will not be modified. */
		return DstUsedLen;

	SrcUsedLen	= s_strlen( src );

	/* The source string is empty... */
	if( SrcUsedLen == 0 )
		/* ...so again the destination string will not be modified. */
		return DstUsedLen;

	DstMaxLen	= s_strmaxlen( dst );

	DstPtr		= s_cstr( dst );
	SrcPtr		= s_constcstr( src );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( DstMaxLen - DstUsedLen, SrcUsedLen );

	memcpy( DstPtr + DstUsedLen, SrcPtr, BytesToCopy );

	DstUsedLen += BytesToCopy;

	/* Terminate the new resulting string. */
	DstPtr[DstUsedLen]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, DstUsedLen );

	return DstUsedLen;
	}
/*============================================================================*/
int s_strcat_c( s_string_t * restrict dst, const char * restrict src )
	{
	size_t		DstMaxLen, DstUsedLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
	if( dst == NULL )
		/* ...let's return zero, as an inexistent string cannot have a size other than zero. */
		return 0;

    if( dst->Writable == 0 )
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
/*============================================================================*/
int s_strlcat( s_string_t * restrict dst, const s_string_t * restrict src, size_t len )
	{
	size_t		ResultLen, DstUsedLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
	if( dst == NULL )
		/* ...let's return zero, as an inexistent string cannot have a size other than zero. */
		return 0;

    if( dst->Writable == 0 )
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

	SrcUsedLen	= s_strlen( src );

	/* The source string is empty... */
	if( SrcUsedLen == 0 )
		/* ...so again the destination string will not be modified. */
		return DstUsedLen;

	ResultLen	= ssmin( len, s_strmaxlen( dst ));

	DstPtr		= s_cstr( dst );
	SrcPtr		= s_constcstr( src );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( ResultLen - DstUsedLen, SrcUsedLen );

	memcpy( DstPtr + DstUsedLen, SrcPtr, BytesToCopy );

	DstUsedLen += BytesToCopy;

	/* Terminate the new resulting string. */
	DstPtr[DstUsedLen]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, DstUsedLen );

	return DstUsedLen;
	}
/*============================================================================*/
int s_strlcat_c( s_string_t * restrict dst, const char * restrict src, size_t len )
	{
	size_t		ResultLen, DstUsedLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
	if( dst == NULL )
		/* ...let's return zero, as an inexistent string cannot have a size other than zero. */
		return 0;

    if( dst->Writable == 0 )
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
/*============================================================================*/
int s_strncat( s_string_t * restrict dst, const s_string_t * restrict src, size_t len )
	{
	size_t		DstMaxLen, DstUsedLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
	if( dst == NULL )
		/* ...let's return zero, as an inexistent string cannot have a size other than zero. */
		return 0;

    if( dst->Writable == 0 )
        return -1;

    DstUsedLen	= s_strlen( dst );

	/* The number of bytes to copy is zero... */
    if( len == 0 )
        /* ...the destination will not be modified. */
        return DstUsedLen;

    /* The source is a null pointer or it is an empty string... */
    if( src == NULL || ( SrcUsedLen	= s_strlen( src )) == 0 )
        /* ...so the destination string will not be modified too. */
        return DstUsedLen;

    DstMaxLen	= s_strmaxlen( dst );

	DstPtr		= s_cstr( dst );
	SrcPtr		= s_constcstr( src );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( DstMaxLen - DstUsedLen, ssmin( SrcUsedLen, len ));

	memcpy( DstPtr + DstUsedLen, SrcPtr, BytesToCopy );

	DstUsedLen += BytesToCopy;

	/* Terminate the new resulting string. */
	DstPtr[DstUsedLen]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, DstUsedLen );

	return DstUsedLen;
	}
/*============================================================================*/
int s_strncat_c( s_string_t * restrict dst, const char * restrict src, size_t len )
	{
	size_t		DstMaxLen, DstUsedLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
	if( dst == NULL )
		/* ...let's return zero, as an inexistent string cannot have a size other than zero. */
		return 0;

    if( dst->Writable == 0 )
        return -1;

    DstUsedLen	= s_strlen( dst );

	/* The number of bytes to copy is zero... */
    if( len == 0 )
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
/*============================================================================*/
int s_strcpy( s_string_t * restrict dst, const s_string_t * restrict src )
	{
	size_t		DstMaxLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
	if( dst == NULL )
		/* ...let's return zero, as an inexistent string cannot have a size other than zero. */
		return 0;

    if( dst->Writable == 0 )
        return -1;

    DstPtr		= s_cstr( dst );

	/* The source is a null pointer or it is an empty string... */
	if( src == NULL || ( SrcUsedLen	= s_strlen( src )) == 0 )
		/* ...so the destination string will be emptied. */
		{
		DstPtr[0]	= '\0';
		SetUsedLen( dst, 0 );
		return 0;
		}

	DstMaxLen	= s_strmaxlen( dst );
	SrcPtr		= s_constcstr( src );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( DstMaxLen, SrcUsedLen );

	memcpy( DstPtr, SrcPtr, BytesToCopy );

	/* Terminate the new resulting string. */
	DstPtr[BytesToCopy]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, BytesToCopy );

	return BytesToCopy;
	}
/*============================================================================*/
int s_strcpy_c( s_string_t * restrict dst, const char * restrict src )
	{
	size_t		DstMaxLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
	if( dst == NULL )
		/* ...let's return zero, as an inexistent string cannot have a size other than zero. */
		return 0;

    if( dst->Writable == 0 )
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
/*============================================================================*/
int s_strlcpy( s_string_t * restrict dst, const s_string_t * restrict src, size_t len )
	{
	size_t		DstMaxLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	const char	*SrcPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
	if( dst == NULL )
		/* ...let's return zero, as an inexistent string cannot have a size other than zero. */
		return 0;

    if( dst->Writable == 0 )
        return -1;

    DstPtr		= s_cstr( dst );

	/* The source is a null pointer or it is an empty string... */
	if( len == 0 || src == NULL || ( SrcUsedLen	= s_strlen( src )) == 0 )
		/* ...so the destination string will be emptied. */
		{
		DstPtr[0]	= '\0';
		SetUsedLen( dst, 0 );
		return 0;
		}

	DstMaxLen	= s_strmaxlen( dst );
	SrcPtr		= s_constcstr( src );

	/* Calculate how many bytes will be copied. */
	BytesToCopy	= ssmin( len, ssmin( DstMaxLen, SrcUsedLen ));

	memcpy( DstPtr, SrcPtr, BytesToCopy );

	/* Terminate the new resulting string. */
	DstPtr[BytesToCopy]	= '\0';

	/* Update the new length of the string. */
	SetUsedLen( dst, BytesToCopy );

	return BytesToCopy;
	}
/*============================================================================*/
int s_strlcpy_c( s_string_t * restrict dst, const char * restrict src, size_t len )
	{
	size_t		DstMaxLen;
	size_t		SrcUsedLen;
	char		*DstPtr;
	size_t		BytesToCopy;

	/* The destination is a null pointer... */
	if( dst == NULL )
		/* ...let's return zero, as an inexistent string cannot have a size other than zero. */
		return 0;

    if( dst->Writable == 0 )
        return -1;

    DstPtr		= s_cstr( dst );

	/* The source is a null pointer or it is an empty string... */
	if( len == 0 || src == NULL || ( SrcUsedLen	= strlen( src )) == 0 )
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

int s_strncpy( s_string_t * restrict dst, const s_string_t * restrict src, size_t len )
	{
    return 0;
	}
*/
/*============================================================================*/
int s_strcmp( const s_string_t * restrict dst, const s_string_t * restrict src )
	{
	const char  *DstPtr;
	const char	*SrcPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( s_strlen( src ), s_strlen( dst )) + 1;

	SrcPtr		= s_constcstr( src );
	DstPtr		= s_constcstr( dst );

	for( Index = 0; Index < Len; Index++ )
		if( SrcPtr[Index] != DstPtr[Index] )
            return (int)( SrcPtr[Index] - DstPtr[Index] );

	return 0;
	}
/*============================================================================*/
int s_strcmp_c( const s_string_t * restrict dst, const char * restrict src )
	{
	const char	*DstPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( strlen( src ), s_strlen( dst )) + 1;

	DstPtr		= s_constcstr( dst );

	for( Index = 0; Index < Len; Index++ )
		if( src[Index] != DstPtr[Index] )
            return (int)( src[Index] - DstPtr[Index] );

	return 0;
	}
/*============================================================================*/
int c_strcmp_s( const char * restrict dst, const s_string_t * restrict src )
	{
	const char	*SrcPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( s_strlen( src ), strlen( dst )) + 1;

	SrcPtr		= s_constcstr( src );

	for( Index = 0; Index < Len; Index++ )
		if( SrcPtr[Index] != dst[Index] )
            return (int)( SrcPtr[Index] - dst[Index] );

	return 0;
	}
/*============================================================================*/
int s_stricmp( const s_string_t * restrict dst, const s_string_t * restrict src )
	{
	const char  *DstPtr;
	const char	*SrcPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( s_strlen( src ), s_strlen( dst )) + 1;

	SrcPtr		= s_constcstr( src );
	DstPtr		= s_constcstr( dst );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( SrcPtr[Index] ) != toupper( DstPtr[Index] ))
            return (int)( toupper( SrcPtr[Index] ) - toupper( DstPtr[Index] ));

	return 0;
	}
/*============================================================================*/
int s_stricmp_c( const s_string_t * restrict dst, const char * restrict src )
	{
	const char	*DstPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( strlen( src ), s_strlen( dst )) + 1;

	DstPtr		= s_constcstr( dst );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( src[Index] ) != toupper( DstPtr[Index] ))
            return (int)( toupper( src[Index] ) - toupper(  DstPtr[Index] ));

	return 0;
	}
/*============================================================================*/
int c_stricmp_s( const char * restrict dst, const s_string_t * restrict src )
	{
	const char	*SrcPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( s_strlen( src ), strlen( dst )) + 1;

	SrcPtr		= s_constcstr( src );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( SrcPtr[Index] ) != toupper( dst[Index] ))
            return (int)( toupper( SrcPtr[Index] ) - toupper( dst[Index] ));

	return 0;
	}
/*============================================================================*/
int s_strncmp( const s_string_t * restrict dst, const s_string_t * restrict src, size_t len )
	{
	const char	*DstPtr;
	const char	*SrcPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( len, ssmin( s_strlen( src ), s_strlen( dst )) + 1 );

	SrcPtr		= s_constcstr( src );
	DstPtr		= s_constcstr( dst );

	for( Index = 0; Index < Len; Index++ )
		if( SrcPtr[Index] != DstPtr[Index] )
            return (int)( SrcPtr[Index] - DstPtr[Index] );

	return 0;
	}
/*============================================================================*/
int s_strncmp_c( const s_string_t * restrict dst, const char * restrict src, size_t len )
	{
	const char	*DstPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( len, ssmin( strlen( src ), s_strlen( dst )) + 1 );

	DstPtr		= s_constcstr( dst );

	for( Index = 0; Index < Len; Index++ )
		if( src[Index] != DstPtr[Index] )
            return (int)( src[Index] - DstPtr[Index] );

	return 0;
	}
/*============================================================================*/
int c_strncmp_s( const char * restrict dst, const s_string_t * restrict src, size_t len )
	{
	const char	*SrcPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( len, ssmin( s_strlen( src ), strlen( dst )) + 1 );

	SrcPtr		= s_constcstr( src );

	for( Index = 0; Index < Len; Index++ )
		if( SrcPtr[Index] != dst[Index] )
            return (int)( SrcPtr[Index] - dst[Index] );

	return 0;
	}
/*============================================================================*/
int s_strnicmp( const s_string_t * restrict dst, const s_string_t * restrict src, size_t len )
	{
	const char	*DstPtr;
	const char	*SrcPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( len, ssmin( s_strlen( src ), s_strlen( dst )) + 1 );

	SrcPtr		= s_constcstr( src );
	DstPtr		= s_constcstr( dst );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( SrcPtr[Index] ) != toupper( DstPtr[Index] ))
            return (int)( toupper( SrcPtr[Index] ) - toupper( DstPtr[Index] ));

	return 0;
	}
/*============================================================================*/
int s_strnicmp_c( const s_string_t * restrict dst, const char * restrict src, size_t len )
	{
	const char	*DstPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( len, ssmin( strlen( src ), s_strlen( dst )) + 1 );

	DstPtr		= s_constcstr( dst );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( src[Index] ) != toupper( DstPtr[Index] ))
            return (int)( toupper( src[Index] ) - toupper( DstPtr[Index] ));

	return 0;
	}
/*============================================================================*/
int c_strnicmp_s( const char * restrict dst, const s_string_t * restrict src, size_t len )
	{
	const char	*SrcPtr;
	int			Index;
	int			Len;

	/* Both dst and src are null pointers... */
	if( dst == NULL && src == NULL )
		/* ...so both are equal. */
		return 0;
	/* dst is a null pointer...*/
	if( dst == NULL )
		/* ...so it is smaller than src. */
		return -1;
	/* src is a null pointer...*/
	if( src == NULL )
		/* ...so dst is bigger than src. */
		return +1;

	Len			= ssmin( len, ssmin( s_strlen( src ), strlen( dst )) + 1 );

	SrcPtr		= s_constcstr( src );

	for( Index = 0; Index < Len; Index++ )
		if( toupper( SrcPtr[Index] ) != toupper( dst[Index] ))
            return (int)( toupper( SrcPtr[Index] ) - toupper( dst[Index] ));

	return 0;
	}
/*============================================================================*/
ssize_t s_strchr( const s_string_t *str, int c, ssize_t start )
	{
	size_t		UsedLen;
	const char	*Ptr;
	int			Index;

	/* str is a null pointer... */
	if( str == NULL )
		/* ...nothing can be found there, let's return a 'not found' result. */
		return -1;

	UsedLen	= s_strlen( str );

	if( start < -(ssize_t)UsedLen || start >= (ssize_t)UsedLen )
		return -1;

    if( start < 0 )
        start  += UsedLen;

    Ptr		= s_constcstr( str );

	/* Search for the character in the string. */
	for( Index = start; Index < UsedLen && Ptr[Index] != c; Index++ )
		{}

	/* If the character was found, return its index in the string, otherwise return a 'not found' result. */
	return ( Index >= 0 && Index < UsedLen ) ? Index : -1;
	}
/*============================================================================*/
ssize_t s_strrchr( const s_string_t *str, int c, ssize_t start )
	{
	size_t		UsedLen;
	const char	*Ptr;
	ssize_t		Index;

	/* str is a null pointer... */
	if( str == NULL )
		/* ...nothing can be found there, let's return a 'not found' result. */
		return -1;

	UsedLen	= s_strlen( str );

	if( start < -(ssize_t)UsedLen || start >= (ssize_t)UsedLen )
		return -1;

    if( start < 0 )
        start  += UsedLen;

    Ptr		= s_constcstr( str );

	/* Search for the character in the string. */
	for( Index = start; Index >= 0 && Ptr[Index] != c; Index-- )
		{}

	/* If the character was found, return its index in the string, otherwise return a 'not found' result. */
	return ( Index >= 0 && Index < UsedLen ) ? Index : -1;
	}
/*============================================================================*/
ssize_t s_strichr( const s_string_t *str, int c, ssize_t start )
	{
	size_t		UsedLen;
	const char	*Ptr;
	int			Index;

	/* str is a null pointer... */
	if( str == NULL )
		/* ...nothing can be found there, let's return a 'not found' result. */
		return -1;

	UsedLen	= s_strlen( str );

	if( start < -(ssize_t)UsedLen || start >= (ssize_t)UsedLen )
		return -1;

    if( start < 0 )
        start  += UsedLen;

    Ptr		= s_constcstr( str );
    c       = toupper( c );

	/* Search for the character in the string. */
	for( Index = start; Index < UsedLen && toupper( Ptr[Index] ) != c; Index++ )
		{}

	/* If the character was found, return its index in the string, otherwise return a 'not found' result. */
	return ( Index >= 0 && Index < UsedLen ) ? Index : -1;
	}
/*============================================================================*/
ssize_t s_strrichr( const s_string_t *str, int c, ssize_t start )
	{
	size_t		UsedLen;
	const char	*Ptr;
	ssize_t		Index;

	/* str is a null pointer... */
	if( str == NULL )
		/* ...nothing can be found there, let's return a 'not found' result. */
		return -1;

	UsedLen	= s_strlen( str );

	if( start < -(ssize_t)UsedLen || start >= (ssize_t)UsedLen )
		return -1;

    if( start < 0 )
        start  += UsedLen;

    Ptr		= s_constcstr( str );
    c       = toupper( c );

	/* Search for the character in the string. */
	for( Index = start; Index >= 0 && toupper( Ptr[Index] ) != c; Index-- )
		{}

	/* If the character was found, return its index in the string, otherwise return a 'not found' result. */
	return ( Index >= 0 && Index < UsedLen ) ? Index : -1;
	}
/*============================================================================*/
ssize_t s_strstr( const s_string_t *str, const s_string_t *src, ssize_t start )
	{
    size_t      StrLen, SrcLen;
    size_t      Index;
    const char  *StrPtr, *SrcPtr;

    if( str == NULL || src == NULL )
        return -1;

    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* If 'src' is an empty string or it is longer than 'str', it will not be found inside 'str'... */
    if(( SrcLen = s_strlen( src )) == 0 || SrcLen > StrLen )
        return -1;

    if( start < -StrLen || start > StrLen - SrcLen || ( start < 0 && start > -(ssize_t)SrcLen ))
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr  = s_constcstr( str );
    SrcPtr  = s_constcstr( src );

    for( Index = start; Index <= StrLen - SrcLen ; Index++ )
        {
        size_t  j;
        for( j = 0; j < SrcLen && StrPtr[Index+j] == SrcPtr[j] ; j++ )
            {}
        if( j == SrcLen )
            return Index;
        }

    return -1;
	}
/*============================================================================*/
ssize_t s_strstr_c( const s_string_t *str, const char *src, ssize_t start )
	{
    size_t      StrLen, SrcLen;
    size_t      Index;
    const char  *StrPtr;

    if( str == NULL || src == NULL )
        return -1;

    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* If 'src' is an empty string or it is longer than 'str', it will not be found inside 'str'... */
    if(( SrcLen = strlen( src )) == 0 || SrcLen > StrLen )
        return -1;

    if( start < -StrLen || start > StrLen - SrcLen || ( start < 0 && start > -(ssize_t)SrcLen ))
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr  = s_constcstr( str );

    for( Index = start; Index <= StrLen - SrcLen ; Index++ )
        {
        size_t  j;
        for( j = 0; j < SrcLen && StrPtr[Index+j] == src[j] ; j++ )
            {}
        if( j == SrcLen )
            return Index;
        }

    return -1;
	}
/*============================================================================*/
ssize_t c_strstr_s( const char *str, const s_string_t *src, ssize_t start )
	{
    size_t      StrLen, SrcLen;
    size_t      Index;
    const char  *SrcPtr;

    if( str == NULL || src == NULL )
        return -1;

    if(( StrLen = strlen( str )) == 0 )
        return -1;

    /* If 'src' is an empty string or it is longer than 'str', it will not be found inside 'str'... */
    if(( SrcLen = s_strlen( src )) == 0 || SrcLen > StrLen )
        return -1;

    if( start < -StrLen || start > StrLen - SrcLen || ( start < 0 && start > -(ssize_t)SrcLen ))
        return -1;

    if( start < 0 )
        start  += StrLen;

    SrcPtr  = s_constcstr( src );

    for( Index = start; Index <= StrLen - SrcLen ; Index++ )
        {
        size_t  j;
        for( j = 0; j < SrcLen && str[Index+j] == SrcPtr[j] ; j++ )
            {}
        if( j == SrcLen )
            return Index;
        }

    return -1;
	}
/*============================================================================*/
ssize_t s_stristr( const s_string_t *str, const s_string_t *src, ssize_t start )
	{
    size_t      StrLen, SrcLen;
    size_t      Index;
    const char  *StrPtr, *SrcPtr;

    if( str == NULL || src == NULL )
        return -1;

    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* If 'src' is an empty string or it is longer than 'str', it will not be found inside 'str'... */
    if(( SrcLen = s_strlen( src )) == 0 || SrcLen > StrLen )
        return -1;

    if( start < -StrLen || start > StrLen - SrcLen || ( start < 0 && start > -(ssize_t)SrcLen ))
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr  = s_constcstr( str );
    SrcPtr  = s_constcstr( src );

    for( Index = start; Index <= StrLen - SrcLen ; Index++ )
        {
        size_t  j;
        for( j = 0; j < SrcLen && toupper( StrPtr[Index+j] ) == toupper( SrcPtr[j] ); j++ )
            {}
        if( j == SrcLen )
            return Index;
        }

    return -1;
	}
/*============================================================================*/
ssize_t s_stristr_c( const s_string_t *str, const char *src, ssize_t start )
	{
    size_t      StrLen, SrcLen;
    size_t      Index;
    const char  *StrPtr;

    if( str == NULL || src == NULL )
        return -1;

    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* If 'src' is an empty string or it is longer than 'str', it will not be found inside 'str'... */
    if(( SrcLen = strlen( src )) == 0 || SrcLen > StrLen )
        return -1;

    if( start < -StrLen || start > StrLen - SrcLen || ( start < 0 && start > -(ssize_t)SrcLen ))
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr  = s_constcstr( str );

    for( Index = start; Index <= StrLen - SrcLen ; Index++ )
        {
        size_t  j;
        for( j = 0; j < SrcLen && toupper( StrPtr[Index+j] ) == toupper( src[j] ); j++ )
            {}
        if( j == SrcLen )
            return Index;
        }

    return -1;
	}
/*============================================================================*/
ssize_t c_stristr_s( const char *str, const s_string_t *src, ssize_t start )
	{
    size_t      StrLen, SrcLen;
    size_t      Index;
    const char  *SrcPtr;

    if( str == NULL || src == NULL )
        return -1;

    if(( StrLen = strlen( str )) == 0 )
        return -1;

    /* If 'src' is an empty string or it is longer than 'str', it will not be found inside 'str'... */
    if(( SrcLen = s_strlen( src )) == 0 || SrcLen > StrLen )
        return -1;

    if( start < -StrLen || start > StrLen - SrcLen || ( start < 0 && start > -(ssize_t)SrcLen ))
        return -1;

    if( start < 0 )
        start  += StrLen;

    SrcPtr  = s_constcstr( src );

    for( Index = start; Index <= StrLen - SrcLen ; Index++ )
        {
        size_t  j;
        for( j = 0; j < SrcLen && toupper( str[Index+j] ) == toupper( SrcPtr[j] ); j++ )
            {}
        if( j == SrcLen )
            return Index;
        }

    return -1;
	}
/*============================================================================*/
ssize_t s_strpbrk( const s_string_t * restrict str, const s_string_t * restrict charset, ssize_t start )
	{
	size_t      StrLen, CharSetLen;
    size_t      Index, j;
    const char  *StrPtr, *CharSetPtr;

    if( str == NULL || charset == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'charset' is an empty string... */
    if(( CharSetLen = s_strlen( charset )) == 0 )
        return -1;

    if( start < -StrLen || start >= StrLen )
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr		= s_constcstr( str );
    CharSetPtr	= s_constcstr( charset );

	for( Index = start; Index < StrLen; Index++ )
		for( j = 0; j < CharSetLen; j++ )
			if( StrPtr[Index] == CharSetPtr[j] )
				return Index;

	return -1;
	}
/*============================================================================*/
ssize_t s_strpbrk_c( const s_string_t * restrict str, const char * restrict charset, ssize_t start )
	{
	size_t      StrLen, CharSetLen;
    size_t      Index, j;
    const char  *StrPtr;

    if( str == NULL || charset == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'charset' is an empty string... */
    if(( CharSetLen = strlen( charset )) == 0 )
        return -1;

    if( start < -StrLen || start >= StrLen )
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr		= s_constcstr( str );

	for( Index = start; Index < StrLen; Index++ )
		for( j = 0; j < CharSetLen; j++ )
			if( StrPtr[Index] == charset[j] )
				return Index;

	return -1;
	}
/*============================================================================*/
s_string_t *s_strdup( const s_string_t * restrict src )
    {
    size_t      SrcLen;
    size_t      Size;
    s_string_t  *Str;

    if( src == NULL )
        return NULL;

    SrcLen  = s_strlen( src );

    if( SrcLen == 0 )
        return NULL;

    Size    = _s_calcsize( SrcLen );

    Str     = malloc( Size );

    if( Str == NULL )
        return NULL;

    _s_string_init( Str, SrcLen, 2 );

    s_strcpy( Str, src );

    return Str;
    }
/*============================================================================*/
s_string_t *s_strdup_c( const char * restrict src )
    {
    size_t      SrcLen;
    size_t      Size;
    s_string_t  *Str;

    if( src == NULL )
        return NULL;

    SrcLen  = strlen( src );

    if( SrcLen == 0 )
        return NULL;

    Size    = _s_calcsize( SrcLen );

    Str     = malloc( Size );

    if( Str == NULL )
        return NULL;

    _s_string_init( Str, SrcLen, 2 );

    s_strcpy_c( Str, src );

    return Str;
    }
/*============================================================================*/
s_string_t *s_strndup( const s_string_t * restrict src, size_t len )
    {
    size_t      SrcLen;
    size_t      Size;
    s_string_t  *Str;

    if( src == NULL || len == 0 )
        return NULL;

    SrcLen  = ssmin( len, s_strlen( src ));

    if( SrcLen == 0 )
        return NULL;

    Size    = _s_calcsize( SrcLen );

    Str     = malloc( Size );

    if( Str == NULL )
        return NULL;

    _s_string_init( Str, SrcLen, 2 );

    s_strcpy( Str, src );

    return Str;
    }
/*============================================================================*/
s_string_t *s_strndup_c( const char * restrict src, size_t len )
    {
    size_t      SrcLen;
    size_t      Size;
    s_string_t  *Str;

    if( src == NULL || len == 0 )
        return NULL;

    SrcLen  = ssmin( len, strlen( src ));

    if( SrcLen == 0 )
        return NULL;

    Size    = _s_calcsize( SrcLen );

    Str     = malloc( Size );

    if( Str == NULL )
        return NULL;

    _s_string_init( Str, SrcLen, 2 );

    s_strcpy_c( Str, src );

    return Str;
    }
/*============================================================================*/
s_string_t *s_strldup( const s_string_t * restrict src, size_t len )
    {
    size_t      SrcLen, DstLen;
    size_t      Size;
    s_string_t  *Dst;

/*
    if( src == NULL )
        return NULL;
*/
    SrcLen  = s_strlen( src );

    if( SrcLen == 0 && len == 0 )
        return NULL;

    DstLen  = ssmax( len, SrcLen );

    Size    = _s_calcsize( DstLen );

    Dst     = malloc( Size );

    if( Dst == NULL )
        return NULL;

    _s_string_init( Dst, DstLen, 2 );

    s_strcpy( Dst, src );

    return Dst;
    }
/*============================================================================*/
s_string_t *s_strldup_c( const char * restrict src, size_t len )
    {
    size_t      SrcLen, DstLen;
    size_t      Size;
    s_string_t  *Dst;

    if( src == NULL )
        SrcLen  = 0;
    else
        SrcLen  = strlen( src );

    if( SrcLen == 0 && len == 0 )
        return NULL;

    DstLen  = ssmax( len, SrcLen );

    Size    = _s_calcsize( DstLen );

    Dst     = malloc( Size );

    if( Dst == NULL )
        return NULL;

    _s_string_init( Dst, DstLen, 2 );

    s_strcpy_c( Dst, src );

    return Dst;
    }
/*============================================================================*/
ssize_t s_strtok( const s_string_t * restrict str, const s_string_t * restrict delim, ssize_t * restrict length, ssize_t * restrict start )
    {
	size_t      StrLen, DelimLen;
    size_t      Index, j, TokenLength, TokenStart;
    const char  *StrPtr, *DelimPtr;

    if( str == NULL || delim == NULL || start == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'delim' is an empty string... */
    if(( DelimLen = s_strlen( delim )) == 0 )
        return -1;

    if( *start < -(ssize_t)StrLen || *start >= StrLen )
        return -1;

    if( *start < 0 )
        *start += StrLen;

    StrPtr      = s_constcstr( str );
    DelimPtr    = s_constcstr( delim );

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
        *start  = StrLen;

        if( length != NULL )
            *length = 0;

        return -1;
        }

    TokenStart  = Index;

    /* Search the next delimiter. */
    for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
        {
        for( j = 0; j < DelimLen && StrPtr[Index] != DelimPtr[j]; j++ )
            {}
        if( j < DelimLen )
            break;
        }

    *start  = Index;

    if( length != NULL )
        *length = TokenLength;

    return TokenStart;
    }
/*============================================================================*/
ssize_t s_strtok_c( const s_string_t * restrict str, const char * restrict delim, ssize_t * restrict length, ssize_t * restrict start )
    {
	size_t      StrLen, DelimLen;
    size_t      Index, j, TokenLength, TokenStart;
    const char  *StrPtr;

    if( str == NULL || delim == NULL || start == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'delim' is an empty string... */
    if(( DelimLen = strlen( delim )) == 0 )
        return -1;

    if( *start < -(ssize_t)StrLen || *start >= StrLen )
        return -1;

    if( *start < 0 )
        *start += StrLen;

    StrPtr      = s_constcstr( str );

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
        *start  = StrLen;

        if( length != NULL )
            *length = 0;

        return -1;
        }

    TokenStart  = Index;

    /* Search the next delimiter. */
    for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
        {
        for( j = 0; j < DelimLen && StrPtr[Index] != delim[j]; j++ )
            {}
        if( j < DelimLen )
            break;
        }

    *start  = Index;

    if( length != NULL )
        *length = TokenLength;

    return TokenStart;
    }
/*============================================================================*/
ssize_t s_strtok_s( s_string_t * restrict dst, const s_string_t * restrict str, const s_string_t * restrict delim, ssize_t * restrict start )
    {
	size_t      StrLen, DelimLen;
    size_t      Index, j, TokenLength, TokenStart;
    const char  *StrPtr, *DelimPtr;

    if( dst == NULL || str == NULL || delim == NULL || start == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'delim' is an empty string... */
    if(( DelimLen = s_strlen( delim )) == 0 )
        return -1;

    if( *start < -(ssize_t)StrLen || *start >= StrLen )
        return -1;

    if( *start < 0 )
        *start += StrLen;

    StrPtr      = s_constcstr( str );
    DelimPtr    = s_constcstr( delim );

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
        *start  = StrLen;
        return -1;
        }

    TokenStart  = Index;

    /* Search the next delimiter. */
    for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
        {
        for( j = 0; j < DelimLen && StrPtr[Index] != DelimPtr[j]; j++ )
            {}
        if( j < DelimLen )
            break;
        }

    *start  = Index;

    if( dst != NULL )
        s_strlcpy_c( dst, &StrPtr[TokenStart], TokenLength );

    return TokenStart;
    }
/*============================================================================*/
ssize_t s_strtok_sc( s_string_t * restrict dst, const s_string_t * restrict str, const char * restrict delim, ssize_t * restrict start )
    {
	size_t      StrLen, DelimLen;
    size_t      Index, j, TokenLength, TokenStart;
    const char  *StrPtr;

    if( dst == NULL || str == NULL || delim == NULL || start == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'delim' is an empty string... */
    if(( DelimLen = strlen( delim )) == 0 )
        return -1;

    if( *start < -(ssize_t)StrLen || *start >= StrLen )
        return -1;

    if( *start < 0 )
        *start += StrLen;

    StrPtr      = s_constcstr( str );

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
        *start  = StrLen;
        return -1;
        }

    TokenStart  = Index;

    /* Search the next delimiter. */
    for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
        {
        for( j = 0; j < DelimLen && StrPtr[Index] != delim[j]; j++ )
            {}
        if( j < DelimLen )
            break;
        }

    *start  = Index;
    if( dst != NULL )
        s_strlcpy_c( dst, &StrPtr[TokenStart], TokenLength );

    return TokenStart;
    }
/*============================================================================*/
s_string_t *s_strtok_m( const s_string_t * restrict str, const s_string_t * restrict delim, ssize_t * restrict start )
    {
	size_t      StrLen, DelimLen;
    size_t      Index, j, TokenLength, TokenStart;
    const char  *StrPtr, *DelimPtr;

    if( str == NULL || delim == NULL || start == NULL )
        return NULL;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return NULL;

    /* 'delim' is an empty string... */
    if(( DelimLen = s_strlen( delim )) == 0 )
        return NULL;

    if( *start < -(ssize_t)StrLen || *start >= StrLen )
        return NULL;

    if( *start < 0 )
        *start += StrLen;

    StrPtr      = s_constcstr( str );
    DelimPtr    = s_constcstr( delim );

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
        *start  = StrLen;

        return NULL;
        }

    TokenStart  = Index;

    /* Search the next delimiter. */
    for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
        {
        for( j = 0; j < DelimLen && StrPtr[Index] != DelimPtr[j]; j++ )
            {}
        if( j < DelimLen )
            break;
        }

    *start  = Index;

    return s_strndup_c( &StrPtr[TokenStart], TokenLength );
    }
/*============================================================================*/
s_string_t *s_strtok_mc( const s_string_t * restrict str, const char * restrict delim, ssize_t * restrict start )
    {
	size_t      StrLen, DelimLen;
    size_t      Index, j, TokenLength, TokenStart;
    const char  *StrPtr;

    if( str == NULL || delim == NULL || start == NULL )
        return NULL;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return NULL;

    /* 'delim' is an empty string... */
    if(( DelimLen = strlen( delim )) == 0 )
        return NULL;

    if( *start < -(ssize_t)StrLen || *start >= StrLen )
        return NULL;

    if( *start < 0 )
        *start += StrLen;

    StrPtr      = s_constcstr( str );

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
        *start  = StrLen;

        return NULL;
        }

    TokenStart  = Index;

    /* Search the next delimiter. */
    for( TokenLength = 0; Index < StrLen; Index++, TokenLength++ )
        {
        for( j = 0; j < DelimLen && StrPtr[Index] != delim[j]; j++ )
            {}
        if( j < DelimLen )
            break;
        }

    *start  = Index;

    return s_strndup_c( &StrPtr[TokenStart], TokenLength );
    }
/*============================================================================*/
ssize_t s_strspn( const s_string_t * restrict str, const s_string_t * restrict charset, int start )
    {
	size_t      StrLen, CharSetLen;
    size_t      Count, j;
    const char  *StrPtr, *CharSetPtr;

    if( str == NULL || charset == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'charset' is an empty string... */
    if(( CharSetLen = s_strlen( charset )) == 0 )
        return -1;

    if( start < -StrLen || start >= StrLen )
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr		= s_constcstr( str );
    CharSetPtr  = s_constcstr( charset );

	for( Count = 0; Count < StrLen; Count++ )
        {
		for( j = 0; j < CharSetLen && StrPtr[start+Count] != CharSetPtr[j]; j++ )
            {}
        if( j == CharSetLen )
            return Count;
        }

	return Count;
    }
/*============================================================================*/
ssize_t s_strspn_c( const s_string_t * restrict str, const char * restrict charset, int start )
    {
	size_t      StrLen, CharSetLen;
    size_t      Count, j;
    const char  *StrPtr;

    if( str == NULL || charset == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'charset' is an empty string... */
    if(( CharSetLen = strlen( charset )) == 0 )
        return -1;

    if( start < -StrLen || start >= StrLen )
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr		= s_constcstr( str );

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
ssize_t s_strcspn( const s_string_t * restrict str, const s_string_t * restrict charset, int start )
    {
	size_t      StrLen, CharSetLen;
    size_t      Count, j;
    const char  *StrPtr, *CharSetPtr;

    if( str == NULL || charset == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'charset' is an empty string... */
    if(( CharSetLen = s_strlen( charset )) == 0 )
        return -1;

    if( start < -StrLen || start >= StrLen )
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr		= s_constcstr( str );
    CharSetPtr  = s_constcstr( charset );

	for( Count = 0; Count < StrLen; Count++ )
		for( j = 0; j < CharSetLen; j++ )
            if( StrPtr[start+Count] == CharSetPtr[j] )
                return Count;

	return Count;
    }
/*============================================================================*/
ssize_t s_strcspn_c( const s_string_t * restrict str, const char * restrict charset, int start )
    {
	size_t      StrLen, CharSetLen;
    size_t      Count, j;
    const char  *StrPtr;

    if( str == NULL || charset == NULL )
        return -1;

    /* 'str' is an empty string... */
    if(( StrLen = s_strlen( str )) == 0 )
        return -1;

    /* 'charset' is an empty string... */
    if(( CharSetLen = strlen( charset )) == 0 )
        return -1;

    if( start < -StrLen || start >= StrLen )
        return -1;

    if( start < 0 )
        start  += StrLen;

    StrPtr		= s_constcstr( str );

	for( Count = 0; Count < StrLen; Count++ )
		for( j = 0; j < CharSetLen; j++ )
            if( StrPtr[start+Count] == charset[j] )
                return Count;

	return Count;
    }
/*============================================================================*/
s_string_t *s_extract_me( const s_string_t * restrict src, ssize_t start, ssize_t end )
    {
    size_t  SrcLen;

    if( src == NULL )
        return NULL;    /* Would it be better returning an empty s_string??? */

    SrcLen      = s_strlen( src );

    if( start < -(ssize_t)SrcLen )
        start   = 0;
    else if( start < 0 )
        start  += SrcLen;

    if( end < -(ssize_t)SrcLen )
        end     = 0;
    else if( end < 0 )
        end    += SrcLen;

    if( start >= SrcLen || start > end )
        return NULL;    /* Would it be better returning an empty s_string??? */

    if( end >= SrcLen )
        end     = SrcLen - 1;

    return s_strndup_c( s_constcstr( src ) + start, end - start + 1 );
    }
/*============================================================================*/
s_string_t *s_extract_mec( const char * restrict src, ssize_t start, ssize_t end )
    {
    size_t  SrcLen;

    if( src == NULL )
        return NULL;    /* Would it be better returning an empty s_string??? */

    SrcLen      = strlen( src );

    if( start < -(ssize_t)SrcLen )
        start   = 0;
    else if( start < 0 )
        start  += SrcLen;

    if( end < -(ssize_t)SrcLen )
        end     = 0;
    else if( end < 0 )
        end    += SrcLen;

    if( start >= SrcLen || start > end )
        return NULL;    /* Would it be better returning an empty s_string??? */

    if( end >= SrcLen )
        end     = SrcLen - 1;

    return s_strndup_c( src + start, end - start + 1 );
    }
/*============================================================================*/
s_string_t *s_extract_ml( const s_string_t * restrict src, ssize_t start, size_t len )
    {
    size_t  SrcLen;

    if( src == NULL || len == 0 )
        return NULL;    /* Would it be better returning an empty s_string??? */

    SrcLen      = s_strlen( src );

    if( start < -(ssize_t)SrcLen )
        start   = 0;
    else if( start < 0 )
        start  += SrcLen;

    if( start >= SrcLen )
        return NULL;    /* Would it be better returning an empty s_string??? */

    if( start + len > SrcLen )
        len     = SrcLen - start;

    return s_strndup_c( s_constcstr( src ) + start, len );
    }
/*============================================================================*/
s_string_t *s_extract_mlc( const char * restrict src, ssize_t start, size_t len )
    {
    size_t  SrcLen;

    if( src == NULL || len == 0 )
        return NULL;    /* Would it be better returning an empty s_string??? */

    SrcLen      = strlen( src );

    if( start < -(ssize_t)SrcLen )
        start   = 0;
    else if( start < 0 )
        start  += SrcLen;

    if( start >= SrcLen )
        return NULL;    /* Would it be better returning an empty s_string??? */

    if( start + len > SrcLen )
        len     = SrcLen - start;

    return s_strndup_c( src + start, len );
    }
/*============================================================================*/
ssize_t s_extract_e( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, ssize_t end )
    {
    size_t  SrcLen;

    if( dst == NULL || src == NULL )
        return -1;    /* Would it be better returning zero??? */

    if( dst->MustBeZero != 0 )
        return -1;

    if( !dst->Writable )
        return -1;

    SrcLen      = s_strlen( src );

    if( start < -(ssize_t)SrcLen )
        start   = 0;
    else if( start < 0 )
        start  += SrcLen;

    if( end < -(ssize_t)SrcLen )
        end     = 0;
    else if( end < 0 )
        end    += SrcLen;

    if( start >= SrcLen || start > end )
        return s_strcpy_c( dst, "" );

    if( end >= SrcLen )
        end     = SrcLen - 1;

    return s_strlcpy_c( dst, s_constcstr( src ) + start, end - start + 1 );
    }
/*============================================================================*/
ssize_t s_extract_ec( s_string_t * restrict dst, const char * restrict src, ssize_t start, ssize_t end )
    {
    size_t  SrcLen;

    if( dst == NULL || src == NULL )
        return -1;    /* Would it be better returning zero??? */

    if( dst->MustBeZero != 0 )
        return -1;

    if( !dst->Writable )
        return -1;

    SrcLen      = strlen( src );

    if( start < -(ssize_t)SrcLen )
        start   = 0;
    else if( start < 0 )
        start  += SrcLen;

    if( end < -(ssize_t)SrcLen )
        end     = 0;
    else if( end < 0 )
        end    += SrcLen;

    if( start >= SrcLen || start > end )
        return s_strcpy_c( dst, "" );

    if( end >= SrcLen )
        end     = SrcLen - 1;

    return s_strlcpy_c( dst, src + start, end - start + 1 );
    }
/*============================================================================*/
ssize_t s_extract_l( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, size_t len )
    {
    size_t  SrcLen;

    if( dst == NULL || src == NULL )
        return -1;    /* Would it be better returning zero??? */

    if( dst->MustBeZero != 0 )
        return -1;

    if( !dst->Writable )
        return -1;

    SrcLen      = s_strlen( src );

    if( start < -(ssize_t)SrcLen )
        start   = 0;
    else if( start < 0 )
        start  += SrcLen;

    if( start >= SrcLen )
        return s_strcpy_c( dst, "" );

    if( start + len > SrcLen )
        len     = SrcLen - start;

    return s_strlcpy( dst, src, len );
    }
/*============================================================================*/
ssize_t s_extract_lc( s_string_t * restrict dst, const char * restrict src, ssize_t start, size_t len )
    {
    size_t  SrcLen;

    if( dst == NULL || src == NULL )
        return -1;    /* Would it be better returning zero??? */

    if( dst->MustBeZero != 0 )
        return -1;

    if( !dst->Writable )
        return -1;

    SrcLen      = strlen( src );

    if( start < -(ssize_t)SrcLen )
        start   = 0;
    else if( start < 0 )
        start  += SrcLen;

    if( start >= SrcLen )
        return s_strcpy_c( dst, "" );

    if( start + len > SrcLen )
        len     = SrcLen - start;

    return s_strlcpy_c( dst, src, len );
    }
/*============================================================================*/
ssize_t s_insert_e( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, ssize_t end, int filler )
    {
    size_t  SrcLen, DstLen, DstMaxLen, Len, SrcStart;
    char    *DstPtr;

    if( dst == NULL || src == NULL )
        return -1;

    if( start >= ( DstMaxLen = s_strmaxlen( dst )))
        return -1;

    if( end < -(ssize_t)( DstLen = s_strlen( dst )))
        return -1;

    SrcStart    = 0;

    if( start < -(ssize_t)DstLen )
        {
        SrcStart= -( start + DstLen );
        start   = 0;
        }
    else if( start < 0 )
        start  += DstLen;

    if( end >= DstMaxLen )
        end     = DstMaxLen - 1;
    else if( end < 0 )
        end    += DstLen;

    SrcLen = s_strlen( src );

    if( start > end || SrcStart >= SrcLen )
        return -1;

    Len         = ssmin( end - start + 1, SrcLen - SrcStart );
    DstPtr      = s_cstr( dst );

    if( start > DstLen )
        memset( DstPtr, filler, start - DstLen );

    memcpy( DstPtr + start, s_constcstr( src ) + SrcStart, Len );

    if( start + Len > DstLen )
        {
        *DstPtr  = '\0';
        SetUsedLen( dst, start + Len );
        }

    return Len;
    }
/*============================================================================*/
ssize_t s_insert_ec( s_string_t * restrict dst, const char * restrict src, ssize_t start, ssize_t end, int filler )
    {
    size_t  SrcLen, DstLen, DstMaxLen, Len, SrcStart;
    char    *DstPtr;

    if( dst == NULL || src == NULL )
        return -1;

    if( start >= ( DstMaxLen = s_strmaxlen( dst )))
        return -1;

    if( end < -(ssize_t)( DstLen = s_strlen( dst )))
        return -1;

    SrcStart    = 0;

    if( start < -(ssize_t)DstLen )
        {
        SrcStart= -( start + DstLen );
        start   = 0;
        }
    else if( start < 0 )
        start  += DstLen;

    if( end >= DstMaxLen )
        end     = DstMaxLen - 1;
    else if( end < 0 )
        end    += DstLen;

    SrcLen = strlen( src );

    if( start > end || SrcStart >= SrcLen )
        return -1;

    Len         = ssmin( end - start + 1, SrcLen - SrcStart );

    DstPtr      = s_cstr( dst );

    if( start > DstLen )
        memset( DstPtr, filler, start - DstLen );

    memcpy( DstPtr + start, src + SrcStart, Len );

    if( start + Len > DstLen )
        {
        *DstPtr  = '\0';
        SetUsedLen( dst, start + Len );
        }

    return Len;
    }
/*============================================================================*/
ssize_t s_insert_l( s_string_t * restrict dst, const s_string_t * restrict src, ssize_t start, size_t len, int filler )
    {
    size_t  SrcLen, DstLen, DstMaxLen, SrcStart;
    char    *DstPtr;

    if( dst == NULL || src == NULL )
        return -1;

    if( start >= ( DstMaxLen = s_strmaxlen( dst )))
        return -1;

    if( start + len > DstMaxLen )
        len     = DstMaxLen - start;

    DstLen = s_strlen( dst );

    SrcStart    = 0;

    if( start < -(ssize_t)DstLen )
        {
        SrcStart= -( start + DstLen );
        start   = 0;
        }
    else if( start < 0 )
        start  += DstLen;

    SrcLen = s_strlen( src );

    if( SrcStart >= SrcLen )
        return -1;

    len         = ssmin( len, SrcLen - SrcStart );
    DstPtr      = s_cstr( dst );

    if( start > DstLen )
        memset( DstPtr, filler, start - DstLen );

    memcpy( DstPtr + start, s_constcstr( src ) + SrcStart, len );

    if( start + len > DstLen )
        {
        *DstPtr  = '\0';
        SetUsedLen( dst, start + len );
        }

    return len;
    }
/*============================================================================*/
ssize_t s_insert_lc( s_string_t * restrict dst, const char * restrict src, ssize_t start, size_t len, int filler )
    {
    size_t  SrcLen, DstLen, DstMaxLen, SrcStart;
    char    *DstPtr;

    if( dst == NULL || src == NULL )
        return -1;

    if( start >= ( DstMaxLen = s_strmaxlen( dst )))
        return -1;

    if( start + len > DstMaxLen )
        len     = DstMaxLen - start;

    DstLen = s_strlen( dst );

    SrcStart    = 0;

    if( start < -(ssize_t)DstLen )
        {
        SrcStart= -( start + DstLen );
        start   = 0;
        }
    else if( start < 0 )
        start  += DstLen;

    SrcLen = strlen( src );

    if( SrcStart >= SrcLen )
        return -1;

    len         = ssmin( len, SrcLen - SrcStart );
    DstPtr      = s_cstr( dst );

    if( start > DstLen )
        memset( DstPtr, filler, start - DstLen );

    memcpy( DstPtr + start, src + SrcStart, len );

    if( start + len > DstLen )
        {
        *DstPtr  = '\0';
        SetUsedLen( dst, start + len );
        }

    return len;
    }
/*============================================================================*/
/*TODO*/
ssize_t s_extins_l( s_string_t * restrict dst, ssize_t dststart, const s_string_t * restrict src, ssize_t srcstart, size_t len, int filler )
    {
    size_t  SrcLen, DstLen, DstMaxLen;
    ssize_t Offset;
    char    *DstPtr;

    if( dst == NULL || src == NULL )
        return -1;

    if( dststart >= ( DstMaxLen = s_strmaxlen( dst )))
        return -1;

    if( srcstart >= ( SrcLen = s_strlen( src )))
        return -1;

    if( dststart + len > DstMaxLen )
        len         = DstMaxLen - dststart;

    if( srcstart + len > SrcLen )
        len         = SrcLen - srcstart;

    DstLen  = s_strlen( dst );

    Offset  = ssmin( dststart + DstLen, srcstart + SrcLen );
    if( Offset < 0 )
        len     += Offset;

    if( dststart < -(ssize_t)DstLen )
        dststart    = Offset -( dststart + DstLen );
    else if( dststart < 0 )
        dststart   += DstLen;

    if( srcstart < -(ssize_t)SrcLen )
        {
        dststart   += -( srcstart + SrcLen );
        srcstart    = 0;
        }
    else if( srcstart < 0 )
        srcstart   += SrcLen;

    if( srcstart >= SrcLen || dststart >= DstLen )
        return -1;

    len             = ssmin( len, SrcLen - srcstart );
    DstPtr          = s_cstr( dst );

    if( dststart > (ssize_t)DstLen )
        memset( DstPtr, filler, dststart - DstLen );

    memcpy( DstPtr + dststart, s_constcstr( src ) + srcstart, len );

    if( dststart + len > DstLen )
        {
        *DstPtr  = '\0';
        SetUsedLen( dst, dststart + len );
        }

    return len;
    }
/*============================================================================*/
ssize_t s_extins_lc( s_string_t * restrict dst, ssize_t dststart, const char * restrict src, ssize_t srcstart, size_t len, int filler );
/*============================================================================*/
size_t _s_calcsize( size_t len )
    {
    int Bytes;

    if( len < ( (size_t)1 <<  8 ) - 1 )
        Bytes   = 1;
    else if( len < ( (size_t)1 << 16 ) - 1 )
        Bytes   = 2;
    else if( len < ( (size_t)1 << 32 ) - 1 )
        Bytes   = 4;
    else
        Bytes   = 8;

    /* One byte for the descriptor + 2 bytes for each size counter + len bytes for the string + 1 byte for the ending NULL character. */
    return 1 + 2 * Bytes + len + 1;
    }
/*============================================================================*/
void _s_string_init( s_string_t *str, size_t len, int area )
    {
    int     Log2Bytes;
    char    *Ptr;

    if( len < ( (size_t)1 <<  8 ) - 1 )
        Log2Bytes   = 0;
    else if( len < ( (size_t)1 << 16 ) - 1 )
        Log2Bytes   = 1;
    else if( len < ( (size_t)1 << 32 ) - 1 )
        Log2Bytes   = 2;
    else
        Log2Bytes   = 3;

    str->MustBeZero = 0;
    str->Sizes      = 1;    /* We will have two size counters (maximum and used). */
    str->Bits       = Log2Bytes;
    str->Area       = area; /* The memory area that the string is allocated (0=bss, 1=stack, 2=heap). */
    str->Writable   = 1;    /* The string is writable. */

    SetMaxLen( str, len );
    SetUsedLen( str, 0 );

    Ptr             = s_cstr( str );
    if( Ptr == NULL )
        return;

    *Ptr            = '\0';
    }
/*============================================================================*/
