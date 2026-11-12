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
#include <stdlib.h>
#include "s_string.h"
/*============================================================================*/
typedef struct
    {
    int     Field1;
    /* We need this macro to create a field of type s_string in a structure. */
    field_s_string( Name, 120 );    /* Creates a field named "Name" capable of storing up to 120 characters. */
    int     Field2;
    } my_struct_t;
/*============================================================================*/
static_s_string_tiny_c( SX, 128, "SX Initial value" );
static_s_string_small_c( SY, 512, "SY Initial value" );
static_s_string_large_c( SZ, 100000, "SZ Initial value" );

int main( int ArgC, char *ArgV[] )
    {
    my_struct_t MyVar;
    my_struct_t MyVar2;
    my_struct_t *p  = &MyVar2;

    MyVar2.Field1   =    1;
    /* We need to initialize each s_string field in every variable of type "my_struct_t". */
    field_s_string_init( *p, Name );
    p->Field2       =    2;


    MyVar.Field1    = 1234;;
    field_s_string_init_c( MyVar, Name, "Someone's name " );
    MyVar.Field2    = 4321;

    /* From now on, things look much more normal... */

    s_strcat( MyVar.Name, SX, 0 );
    s_strcat( MyVar2.Name, SY, 0 );
    s_strcpy( p->Name, SZ, 0 );

    printf( "\nSX = \"%s\"", s_constcstr( SX, 0 ));
    printf( "\nSY = \"%s\"", s_constcstr( SY, 0 ));
    printf( "\nSZ = \"%s\"", s_constcstr( SZ, 0 ));

    s_strcat( SX, SY, 0 );

    printf( "\n\nSX = \"%s\"", s_constcstr( SX, 0 ));

    printf( "\nPosition of \"value\" inside SX: %ld\n", (long)s_strstr_c( SX, 0, "value" ));

    printf( "\nMyVar = %d, \"%s\", %d\n", MyVar.Field1, s_constcstr( MyVar.Name, 0 ), MyVar.Field2 );

    printf( "\nMyVar2 = %d, \"%s\", %d\n", p->Field1, s_constcstr( p->Name, 0 ), MyVar2.Field2 );
    }
/*============================================================================*/
