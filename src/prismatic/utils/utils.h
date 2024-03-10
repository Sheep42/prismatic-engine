#ifndef UTILS_H
#define UTILS_H

typedef struct PrismUtils {
	float ( *lerp )( float, float, float );
} PrismUtils;


extern const PrismUtils* prismaticUtils;

#endif