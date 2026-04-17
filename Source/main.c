/*=============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
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

	if( pFullPath == NULL || ( pPath == NULL && pName == NULL && pExt == NULL ))
		return;

	/* Search 'pFullPath' for a path separator character, starting from the end towards the beginning...*/
	if(( p = s_strrchr( pFullPath, -1, PATH_SEPARATOR_CHAR )) >= 0 )
		{
		/* ...the character was found. */
		/* Set 'pPath' with a sub-string extracted from 'pFullpath' starting at index 0 and ending at index 'p' (inclusive). */
		s_extract_e( pPath, pFullPath, 0, p );
		/* Advance 'p' so it points to the beginning of the file name, if it exists. */
		p++;
		}
#if			!defined __linux__
	/* Search 'pFullPath' for a device separator character, starting from the beginning towards the end...*/
	else if(( p = s_strchr( pFullPath, 0, ':' )) >= 0 )
		{
		/* ...the character was found. */
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
		/* Set 'pPath' with an empty string. */
		s_strcpy_c( pPath, "" );
		}

	/* Search 'pFullPath' for a dot, starting from the end towards the beginning...*/
	if(( q = s_strrchr( pFullPath, -1, '.' )) > p )
		{
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
		/* Set 'pExt' with an empty string. */
		s_strcpy_c( pExt, "" );
		}

	if( pName != NULL )	// This test is superfluous because 's_extract_e' is safe and test for NULL pointers.
		/* Set 'pName' with a sub-string extracted from 'pFullpath' starting at index 'p' and ending at index 'q' (inclusive). */
		s_extract_e( pName, pFullPath, p, q );
	}
/*============================================================================*/
static void Test_s_delete_l( void )
	{
	auto_s_string_c(	r, 32, "0123456789" );
	auto_s_string(		s, 32 );
	ssize_t				i, j, n;
	ssize_t				Len	= s_strlen( r );

	for( i = 0; i <= Len; i++ )
		for( j = -Len - 1 - i; j <= Len + 1; j++ )
			{
			s_strcpy( s, r, 0 );
			n	= s_delete_l( s, j, i );
			printf( "%3lld %3lld %3lld %s\n", j, i, n, s_constcstr( s, 0 ));
			}
	}
/*============================================================================*/
static void Test_s_delete_e( void )
	{
	auto_s_string_c(	r, 32, "0123456789" );
	auto_s_string(		s, 32 );
	ssize_t				i, j, n;
	ssize_t				Len	= s_strlen( r );

	for( i = -Len - 1; i <= Len; i++ )
		for( j = -Len - 1; j <= Len + 1; j++ )
			{
			s_strcpy( s, r, 0 );
			n	= s_delete_e( s, i, j );
			printf( "%3lld %3lld %3lld %s\n", i, j, n, s_constcstr( s, 0 ));
			}
	}
/*============================================================================*/
static_const_s_string_tiny_c(	TestStr1,  64, "Parameter=Value, Parameter2 = Value2; Parameter3 = Value3;" );
static const char				TestDelimC[]	= " =,;";
static_const_s_string_tiny_c(	TestDelim1, 4, TestDelimC );
/*============================================================================*/
static void Test_s_strtok( void )
	{
	ssize_t	p, Start, Length;

	printf( "\ns_strtok\n\n\"%s\" : \"%s\"\n\n", s_constcstr( TestStr1, 0 ), s_constcstr( TestDelim1, 0 ));

	for( p = 0; ( Start = s_strtok( TestStr1, &p, TestDelim1, 0, &Length )) >= 0; )
		{
#if 0
		auto_s_string( t, 64 );
		s_extract_l( t, r, Start, Length );
		printf( "T: %s\n", s_constcstr( t, 0 ) );
#else
		printf( "T: %.*s\tD: \'%c\'\n", (int)Length, s_constcstr( TestStr1, Start ), s_charat( TestStr1, p ));
#endif
		}
	}
/*============================================================================*/
static void Test_s_strtok_c( void )
	{
	ssize_t	p, Start, Length;

	printf( "\ns_strtok_c\n\n\"%s\" : \"%s\"\n\n", s_constcstr( TestStr1, 0 ), TestDelimC );

	for( p = 0; ( Start = s_strtok_c( TestStr1, &p, TestDelimC, &Length )) >= 0; )
		{
#if 0
		auto_s_string( t, 64 );
		s_extract_l( t, r, Start, Length );
		printf( "T: %s\n", s_constcstr( t, 0 ) );
#else
		printf( "T: %.*s\tD: \'%c\'\n", (int)Length, s_constcstr( TestStr1, Start ), s_charat( TestStr1, p ));
#endif
		}
	}
/*============================================================================*/
static void Test_s_strtok_s( void )
	{
	auto_s_string(		t, 64 );
	ssize_t				p, Start;

	printf( "\ns_strtok_s\n\n\"%s\" : \"%s\"\n\n", s_constcstr( TestStr1, 0 ), s_constcstr( TestDelim1, 0 ));

	for( p = 0; ( Start = s_strtok_s( t, TestStr1, &p, TestDelim1, 0 )) >= 0; )
		printf( "T: %s\tD: \'%c\'\n", s_constcstr( t, 0 ), s_charat( TestStr1, p ));
	}
/*============================================================================*/
static void Test_s_strtok_sc( void )
	{
	auto_s_string(		t, 64 );
	ssize_t				p, Start;

	printf( "\ns_strtok_sc\n\n\"%s\" : \"%s\"\n\n", s_constcstr( TestStr1, 0 ), TestDelimC );

	for( p = 0; ( Start = s_strtok_sc( t, TestStr1, &p, TestDelimC )) >= 0; )
		printf( "T: %s\tD: \'%c\'\n", s_constcstr( t, 0 ), s_charat( TestStr1, p ));
	}
/*============================================================================*/
static void Test_s_strtok_m( void )
	{
	s_string_t			*t;
	ssize_t				p;

	printf( "\ns_strtok_m\n\n\"%s\" : \"%s\"\n\n", s_constcstr( TestStr1, 0 ), s_constcstr( TestDelim1, 0 ));

	for( p = 0; ( t = s_strtok_m( TestStr1, &p, TestDelim1, 0 )) != NULL; )
		{
		printf( "T: %s\tD: \'%c\'\n", s_constcstr( t, 0 ), s_charat( TestStr1, p ));
		free( t );
		}
	}
/*============================================================================*/
static void Test_s_strtok_mc( void )
	{
	s_string_t			*t;
	ssize_t				p;

	printf( "\ns_strtok_mc\n\n\"%s\" : \"%s\"\n\n", s_constcstr( TestStr1, 0 ), TestDelimC );

	for( p = 0; ( t = s_strtok_mc( TestStr1, &p, TestDelimC )) != NULL; )
		{
		printf( "T: %s\tD: \'%c\'\n", s_constcstr( t, 0 ), s_charat( TestStr1, p ));
		free( t );
		}
	}
/*============================================================================*/
int main( int ArgC, char *ArgV[] )
	{
	auto_s_string(	Argi, 256 );
	auto_s_string(	Path, 256 );
	auto_s_string(	Name, 256 );
	auto_s_string(	Ext, 256 );
	int				i;

#if 0
	for( i = 0; i < ArgC; i++ )
		{
		s_strcpy_c( Argi, ArgV[i] );

		SplitPath( Argi, Path, Name, Ext );

		printf( "\n*%s*\n*%s*\n*%s*\n\n", s_constcstr( Path, 0 ), s_constcstr( Name, 0 ), s_constcstr( Ext, 0 ) );
		}
#endif

//	Test_s_delete_e();

	Test_s_strtok();
	Test_s_strtok_c();
	Test_s_strtok_s();
	Test_s_strtok_sc();
	Test_s_strtok_m();
	Test_s_strtok_mc();

	return 0;
	}
/*=============================================================================*/
