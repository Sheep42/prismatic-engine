#ifndef UTILS_H
#define UTILS_H

#include <stdint.h>
typedef struct PrismUtils {
	// Linear interpolation implementation
	// 
	// float start 
	// 
	// float end
	// 
	// float time
	float ( *lerp )( float, float, float );
	// Implementation of pow, for use with 8-bit unsigned ints
	// 
	// Caution: This function does not internally protect against overflow 
	// or underflow
	// 
	// uint8_t base
	// 
	// uint8_t exponent
	uint8_t ( *uint8_pow )( uint8_t, uint8_t );
} PrismUtils;


extern const PrismUtils* prismaticUtils;

#endif