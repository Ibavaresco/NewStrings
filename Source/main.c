/*=============================================================================*/
#if			!defined DOXYGEN
/*============================================================================*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <windows.h>
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
			printf( "%3ld %3ld %3ld %s\n", (long)j, (long)i, (long)n, s_constcstr( s, 0 ));
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
			printf( "%3ld %3ld %3ld %s\n", (long)i, (long)j, (long)n, s_constcstr( s, 0 ));
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
static void Test_s_replace_de( void )
	{
#if 0
	auto_s_string_c(	Str, 42, "********************************" );
	auto_s_string_c(	Src,  8, "ABCDEF" );
	auto_s_string(		Dst, 52 );
	int					i;

	printf( "Ret ds  de  ss\n--- --- --- ---" );
	for( i = -51; i < 52; i++ )
		{
		s_strcpy( Dst, Str, 0 );
		ssize_t	n = s_replace_de( Dst, i, i + 39, Src, -7, '-' );
		printf( "\n%3d %3d %3d %3d \"%s\"", n, i, i + 39, -7, s_constcstr( Dst, 0 ));
		}
	for( i = -10; i < 53; i++ )
		{
		s_strcpy( Dst, Str, 0 );
		ssize_t	n = s_replace_de( Dst, i, i + 7, Src, -7, '-' );
		printf( "\n%3d %3d %3d %3d \"%s\"", n, i, i + 7, -7, s_constcstr( Dst, 0 ));
		}
#else
	auto_s_string_c(	Str, 32, "********************************" );
	auto_s_string_c(	Src,  6, "ABCDEF" );
	auto_s_string(		Dst, 32 );
	int					Filler	= '\0';
	int					i;

	while( 1 )
		{
		for( i = -43; i < -1; i++ )
			{
			s_strcpy( Dst, Str, 0 );
			ssize_t	n = s_replace_e( Dst, i, i + 43, Src, -9, Filler );
			printf( "\r%3ld %3d %3d %3d \"%s\"", (long)n, i, i + 7, -9, s_constcstr( Dst, 0 ));
			usleep( 100000 );
			}
		for( ; i > -43; i-- )
			{
			s_strcpy( Dst, Str, 0 );
			ssize_t	n = s_replace_e( Dst, i, i + 43, Src, -9, Filler );
			printf( "\r%3ld %3d %3d %3d \"%s\"", (long)n, i, i + 7, -9, s_constcstr( Dst, 0 ));
			usleep( 100000 );
			}
		}

#endif
	}
/*============================================================================*/
static void Test_s_ltrim( void )
	{
	auto_s_string_c(	Delim, 5, " \r\n\tL" );
	auto_s_string_c(	Str, 32, "\n  \t\rLTrim Test" );

//	printf( "Removed: %ld\n", (long)s_ltrim( Str, 0, Delim, 0 ) );
	printf( "Removed: %ld\n", (long)s_ltrim_c( Str, 0, " \r\n\tL" ) );
	printf( "*%s*", s_constcstr( Str, 0 ));
	}
/*============================================================================*/
static void Test_s_rtrim( void )
	{
	auto_s_string_c(	Delim, 5, " \r\n\t" );
	auto_s_string_c(	Str, 32, " \tLTrim Test \r\n\t   " );

//	printf( "Removed: %ld\n", (long)s_rtrim( Str, 0, Delim, 0 ) );
	printf( "Removed: %ld\n", (long)s_rtrim_c( Str, 0, " \r\n\tL" ) );
	printf( "*%s*", s_constcstr( Str, 0 ));
	}
/*============================================================================*/
static void Test_s_fill_l( void )
	{
	auto_s_string_c(	Src, 32, "****************" );
	auto_s_string(		Str, 32 );
	ssize_t				s, l;

	for( s = -33; s <= 33; s++ )
		for( l = -33; l <= 33; l++ )
			{
			s_strcpy( Str, Src, 0 );
			printf( "%3ld %3ld %3ld", (long)s, (long)l, (long)s_fill_l( Str, s, l, 'X' ));
			printf( " %s\n", s_constcstr( Str, 0 ));
			}
	}
/*============================================================================*/
global_s_string_tiny_array( Arr, 50, 200 );
global_s_string_small_array( ArrS, 50, 100 );
global_s_string_large_array( ArrL, 50, 100 );
int Canario1;
int Canario2	= 35;
/*============================================================================*/
typedef struct
	{
	int	a;
	int	b;
	field_s_string( s, 50 );
	field_s_string( t, 50 );
	} s_s_t;
/*============================================================================*/
int main( int ArgC, char *ArgV[] )
	{
	auto_s_string_c(	s, 100, "  ,. Isaac  Marino -   \tBavaresco   \r\n   " );
	auto_s_string_c(	d, 100, " \t\r\n,.-" );


	s_strcat( s, s, 0 );

	printf( "\n*%s*\n", s_constcstr( s, 0 ));

	printf( "\nCharacters removed: %ld\n", (long)s_alltrim_e( s, 1, -2, d, 0, '\0' ));

	printf( "\n*%s*\n", s_constcstr( s, 0 ));


#if 0

//	char					Temp[256];
	s_s_t					ss;

	ss.a	=   35;
	ss.b	= 3355;
	field_s_string_init_c( ss, s, "xyz" );
	field_s_string_init( ss, t );

	printf( "\nss.a = %d ss.b = %d ss.s = %s ss.t = %s\n", ss.a, ss.b, s_constcstr( ss.s, 0 ), s_constcstr( ss.t, 0 ));

	for( i = 0; i < sizeof v / sizeof v[0]; i++ )
		{
		s_sprintf_c( v[i], 0, 0, "Teste de array %d", i );
//		s_strcpy_c( v[i], Temp );
		s_strcpy( Arr[i], v[i], 0 );
		if( i > 0 )
			s_strcat( Arr[i], Arr[i-1], i );
		}

	for( i = 0; i < sizeof v / sizeof v[0]; i++ )
		printf( "v[%2d] = %s\n", i, s_constcstr( v[i], 0 ));

	for( i = 0; i < sizeof Arr / sizeof Arr[0]; i++ )
		printf( "Arr[%3d] = %s\n", i, s_constcstr( Arr[i], 0 ));

#if 0
	auto_s_string(	Argi, 256 );
	auto_s_string(	Path, 256 );
	auto_s_string(	Name, 256 );
	auto_s_string(	Ext, 256 );
	int				i;

	for( i = 0; i < ArgC; i++ )
		{
		s_strcpy_c( Argi, ArgV[i] );

		SplitPath( Argi, Path, Name, Ext );

		printf( "\n*%s*\n*%s*\n*%s*\n\n", s_constcstr( Path, 0 ), s_constcstr( Name, 0 ), s_constcstr( Ext, 0 ) );
		}
#endif

//	Test_s_delete_e();

#if 0
	Test_s_strtok();
	Test_s_strtok_c();
	Test_s_strtok_s();
	Test_s_strtok_sc();
	Test_s_strtok_m();
	Test_s_strtok_mc();
#endif

//	Test_s_replace_de();

//	Test_s_fill_l();
#endif

	return 0;
	}
/*=============================================================================*/
#endif	/*	!defined DOXYGEN */
/*============================================================================*/
