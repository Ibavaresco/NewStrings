/*=============================================================================*/
#include <stdio.h>
/*=============================================================================*/
#include "s_string.h"
/*=============================================================================*/
#if			defined __linux__
#define	PATH_SEPARATOR_CHAR	'/'
#else	/*	defined __linux__ */
#define	PATH_SEPARATOR_CHAR	'\\'
#endif	/*	defined __linux__ */
/*============================================================================*/
void SplitPath( const s_string_t *pFullPath, s_string_t *pPath, s_string_t *pName, s_string_t *pExt )
	{
	ssize_t	p, q;

	/* Search 'pFullPath' for a path separator character, starting from the end towards the beginning...*/
	if(( p = s_strrchr( pFullPath, -1, PATH_SEPARATOR_CHAR )) >= 0 )
		{
		/* ...the character was found. */
		if( pPath != NULL )	// This test is superfluous because 's_extract_e' is safe and tests for NULL pointers.
			/* Set 'pPath' with a sub-string extracted from 'pFullpath' starting at index 0 and ending at index 'p' (inclusive). */
			s_extract_e( pPath, pFullPath, 0, p );
		/* Advance 'p' so it points to the beginning of the file name, if it exists. */
		p++;
		}
#if			!defined __linux__
	/* Search 'pFullPath' for a drive separator character, starting from the end towards the beginning...*/
	else if(( p = s_strrchr( pFullPath, -1, ':' )) >= 0 )
		{
		/* ...the character was found. */
		if( pPath != NULL )	// This test is superfluous because 's_extract_e' is safe and tests for NULL pointers.
			/* Set 'pPath' with a sub-string extracted from 'pFullpath' starting at index 0 and ending at index 'p' (inclusive). */
			s_extract_e( pPath, pFullPath, 0, p );
		/* Advance 'p' so it points to the beginning of the file name, if it exists. */
		p++;
		}
#endif	/*	!defined __linux__ */
	/* We do not have a path or drive identifier, only a file name (if any). */
	else
		{
		/* The file name starts at the beginning of the string. */
		p		= 0;
		if( pPath != NULL )	// This test is superfluous because 's_extract_e' is safe and tests for NULL pointers.
			/* Set 'pPath' with an empty string. */
			s_strcpy_c( pPath, "" );
		}

	/* Search 'pFullPath' for a dot, starting from the end towards the beginning...*/
	if(( q = s_strrchr( pFullPath, -1, '.' )) > p )
		{
		if( pExt != NULL )
			/* Set 'pExt' with a sub-string extracted from 'pFullpath' starting at index 'q' to the end of the string. */
			s_strcpy( pExt, pFullPath, q );
		/* Retrocede 'q' so the name ends before the dot. */
		q--;
		}
	/* We did not find an extension... */
	else
		{
		/* The name ends at the end of the string. */
		q		= -1;
		if( pExt != NULL )	// This test is superfluous because 's_extract_e' is safe and test for NULL pointers.
			/* Set 'pExt' with an empty string. */
			s_strcpy_c( pExt, "" );
		}

	if( pName != NULL )	// This test is superfluous because 's_extract_e' is safe and test for NULL pointers.
		/* Set 'pName' with a sub-string extracted from 'pFullpath' starting at index 'p' and ending at index 'q' (inclusive). */
		s_extract_e( pName, pFullPath, p, q );
	}
/*============================================================================*/
int main( int ArgC, char *ArgV[] )
	{
	auto_s_string( Argi, 256 );
	auto_s_string( Path, 256 );
	auto_s_string( Name, 256 );
	auto_s_string( Ext, 256 );
	int	i;

	for( i = 0; i < ArgC; i++ )
		{
		s_strcpy_c( Argi, ArgV[i] );

		SplitPath( Argi, Path, Name, Ext );

		printf( "\n*%s*\n*%s*\n*%s*\n\n", s_constcstr( Path, 0 ), s_constcstr( Name, 0 ), s_constcstr( Ext, 0 ) );
		}
	}
/*=============================================================================*/
