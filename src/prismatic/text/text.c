#include <stdlib.h>
#include <string.h>
#include "../prismatic.h"

// String Helpers

static string newString( string str ) {
	return strdup( str );
}

static void deleteString( string str ) {
	sys->realloc( str, 0 );
}

static bool stringEquals( string str1, string str2 ) {
	return strcmp( str1, str2 ) == 0;
}

static bool stringContains( string needle, string haystack ) {
	return strstr( haystack, needle ) != NULL ? true : false;
}

static void stringConcat( string* target, string src ) {

	size_t targetLen = strlen( *target );
	*target = sys->realloc( *target, targetLen + strlen( src ) + 1 );
	strcat( *target, src );

}

static string trimLast( const string str, char ch ) {

	if( str == NULL || *str == '\0' ) {
		prismaticLogger->error( "prismaticString->trimLast: str cannot be NULL" );
		return;
	}

	if( ch == '\0' ) {
		prismaticLogger->error( "prismaticString->trimLast: Cannot trim the NULL terminator from str!" );
		return;
	}

	string trimmed = newString( str );

	size_t len = strlen( str );
	if( str[len - 1] == ch ) {
		trimmed[len - 1] = '\0';
	}

	return trimmed;

}

const StringUtils* prismaticString = &(StringUtils) {
	.new = newString,
	.delete = deleteString,
	.equals = stringEquals,
	.contains = stringContains,
	.trimLast = trimLast,
	.concat = stringConcat,
};