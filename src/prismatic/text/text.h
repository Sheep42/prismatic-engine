// text.h
#ifndef TEXT_H
#define TEXT_H

#ifndef STDBOOL_INCLUDED
	#define STDBOOL_INCLUDED
	#include <stdbool.h>
#endif

typedef char* string;
typedef struct StringUtils {
	string ( *new )( string );
	void ( *delete )( string );
	bool ( *equals )( string, string );
} StringUtils;

extern const StringUtils* prismaticString;

#endif // TEXT_H