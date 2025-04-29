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
static_s_string_tiny_i( SX, 128, "SX Initial value" );
static_s_string_small_i( SY, 512, "SY Initial value" );
static_s_string_large_i( SZ, 100000, "SZ Initial value" );

int main( int ArgC, char *ArgV[] )
    {
    my_struct_t MyVar;
    my_struct_t *p  = &MyVar;

    MyVar.Field1    = 1;
    /* We need to initialize each s_string field in every variable of type "my_struct_t". */
    field_s_string_init( *p, Name );
    field_s_string_init_i( MyVar, Name, "Someone's name " );
    MyVar.Field1    = -1;

    /* From now on, things look much more normal... */

    s_strcat( MyVar.Name, SX );
    s_strcat( MyVar.Name, SY );
    s_strcat( MyVar.Name, SZ );

    printf( "\n%s", s_constcstr( SX ));
    printf( "\n%s", s_constcstr( SY ));
    printf( "\n%s", s_constcstr( SZ ));
    s_strcat( SX, SY );
    printf( "\n%s", s_constcstr( SX ));
    printf( "\nPosition of \"value\" inside SX: %lld\n", s_strstr_c( SX, "value", 0 ));
    printf( "\n%s\n", s_constcstr( MyVar.Name ));
    }
/*============================================================================*/
