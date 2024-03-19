// text.h
#ifndef TEXT_H
#define TEXT_H

#ifndef STDBOOL_INCLUDED
	#define STDBOOL_INCLUDED
	#include <stdbool.h>
#endif

typedef char* string;
typedef struct StringUtils {
	// Create a new string
	// 
	// ---
	// 
	// string str
	string ( *new )( string );

	// Delete a string
	// 
	// ---
	// 
	// string str
	void ( *delete )( string );

	// Check if two strings are equal
	// 
	// ---
	// 
	// string str1
	// 
	// string str2
	bool ( *equals )( string, string );
} StringUtils;

extern const StringUtils* prismaticString;

#endif // TEXT_H