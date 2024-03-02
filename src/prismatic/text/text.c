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

const StringUtils* prismaticString = &(StringUtils) {
	.new = newString,
	.delete = deleteString,
};