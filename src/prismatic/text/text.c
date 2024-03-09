#include <stdlib.h>
#include <string.h>
#include "text.h"

// String Helpers

static string newString( string str ) {
	return strdup( str );
}

static void deleteString( string str ) {
	free( str );
}

static bool stringEquals( string str1, string str2 ) {
	return strcmp( str1, str2 ) == 0;
}

const StringUtils* prismaticString = &(StringUtils) {
	.new = newString,
	.delete = deleteString,
	.equals = stringEquals,
};