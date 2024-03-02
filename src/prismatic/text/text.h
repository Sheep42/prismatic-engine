// text.h
#ifndef TEXT_H
#define TEXT_H

typedef char* string;
typedef struct StringUtils {
	string ( *new )( string );
	void ( *delete )( string );
} StringUtils;

extern const StringUtils* prismaticString;

#endif // TEXT_H