
#ifndef UTILS_INCLUDED
    #define UTILS_INCLUDED
    #include "utils.h"
#endif

static float lerp( float start, float end, float time );

static float lerp( float start, float end, float time ) {
    return (start + time * (end - start));
}

const PrismUtils* prismaticUtils = &(PrismUtils) {
    .lerp = lerp,
};