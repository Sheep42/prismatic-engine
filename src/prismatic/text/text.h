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
	// ----
	// 
	// string str
	void ( *delete )( string );

	// Check if two strings are equal
	// 
	// ----
	// 
	// string str1
	// 
	// string str2
	bool ( *equals )( string, string );

	// Check if a string contains another string
	//
	// ----
	//
	// string needle - The substring to search for
	//
	// string haystack - The string to search through
	bool ( *contains )( string, string );

	// Trim the trailing character of a string, if it exists
	//
	// Note that target is duplicated and the trimmed string is returned
	//
	// ----
	//
	// string target - The string to trim the trailing character from
	//
	// char ch - The character to trim
	string ( *trimLast )( string, char );

	// Concatenate a string with another string
	//
	// Both target and src should be pointers, so the caller is responsible for
	// freeing them.
	//
	// ---- 
	//
	// string* target - The string that src should be added to 
	//
	// string src - The string that should be concatenated to target
	void ( *concat )( string*, string );

	// Perform a substring operation based on a start and end position in an
	// existing string.
	// 
	// This creates a new string which the caller is responsible for freeing. 
	// The new string should be freed using prismaticString->delete()
	// 
	// ----
	// 
	// string src - The string being substringed
	// 
	// int start - The starting position. Must be between 0 and the length of
	// src
	// 
	// int end - The ending position. Must be between start and the length of 
	// src
	// 
	// return string - The resulting substring
	string ( *substr )( string, int, int );
} StringUtils;

extern const StringUtils* prismaticString;

#endif // TEXT_H