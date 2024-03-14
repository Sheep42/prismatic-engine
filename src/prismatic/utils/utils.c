
#ifndef UTILS_INCLUDED
    #define UTILS_INCLUDED
    #include "utils.h"
#endif

static float lerp( float start, float end, float time );
static uint8_t uint8_pow( uint8_t base, uint8_t exponent );

static float lerp( float start, float end, float time ) {
    return (start + time * (end - start));
}

static uint8_t uint8_pow( uint8_t base, uint8_t exponent ) {
    
    uint8_t result = 1;

    for( uint8_t i = 0; i < exponent; i++ ) {
        result *= base;
    }

    return result;

}

const PrismUtils* prismaticUtils = &(PrismUtils) {
    .lerp = lerp,
    .uint8_pow = uint8_pow,
};