# NewStrings
ATTENTION: It is in a very alpha stage. Things kinda work, but anything can change, such as function argument definitions, etc.

A library with a new implementation of strings in C. Introduces a new type 's_string' to be used instead of the traditional 'array of char'.
's_string_t' is an opaque data type (that is, its definition is hidden from the programmer) and all the accesses must be done through the library's functions.

CAVEATS

There are two main points that make the use clunky:

1) It is not possible to simply declare a variable of this type, one must use a macro to create a local/automatic, global or static variable of type s_string. Other option is using the 's_strdup*' functions, that create dynamic objects in the heap.

2) It is not possible to index characters in the strings (e.g. putch( s[i] ); or s[i] = 'x';), one must use the functions 's_charat(str,index)' to read a character and 's_setcharat(str,index,value)' to change a character.

Other than that, things are pretty much the same as with the standard library...

...more or less.

First, we cannot use pointers to characters inside the strings, we must use indexes. So every function that walks a pointer through a string, will take a pointer the the s_string object and an index to the character being accessed. Also, every function that traditionally returns a pointer to a character inside a string (e.g. strchr) will return the index of the character inside the string.

Example:
<pre>
// Create a local s_string variable named 'path' capable of holding up to 255 characters, initialized
// with the string "C:\\Program Files\Notepad++\\notepad++.exe"
auto_s_string_i( path, 255, "C:\\Program Files\\Notepad++\\notepad++.exe" );

// Find the last backslash in 'path' (strrchr searchs backwards). If not found, returns -1.
ssize_t slash = s_strrchr( path, '\\', -1 ); // The -1 here means we are starting the search from the last character.
if( slash >= 0 ) { // Found...
    s_truncate( path, slash ); // Remove the filename and the backslash from the full path.
    // Obtain a C string representation of the s_string so we can print it with 'printf'.
    printf( "Path: %s\n", s_constcstr( path ));
    }
</pre>
More to come...
