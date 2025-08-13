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

	// Generate a random float between min and max
	// 
	// Caution: This function will only work properly if rand has been
	// seeded using srand
	// 
	// float min
	// 
	// float max
	float (*randBetween)( float min, float max );

	// Generate a random int between min and max
	// 
	// Caution: This function will only work properly if rand has been
	// seeded using srand
	// 
	// int min
	// 
	// int max
	int (*randIntBetween)( int min, int max );
} PrismUtils;


extern const PrismUtils* prismaticUtils;

#endif