#include <stdlib.h>
#include <string.h>

#include "../prismatic.h"

#ifndef UTILS_INCLUDED
    #define UTILS_INCLUDED
    #include "utils.h"
#endif

static float lerp( float start, float end, float time );
static uint8_t uint8_pow( uint8_t base, uint8_t exponent );
static char* getPDXInfo( void );
static char* getPDXInfoValue( char* key );

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

static float randBetween( float min, float max ) {
    return min + (((float)rand() / (float)RAND_MAX) * (max - min));
}

static int randIntBetween( int min, int max ) {
    return (rand() % (max - min + 1)) + min;
}

static char* getPDXInfo() {

    FileStat stat;
    if( pd->file->stat( "pdxinfo", &stat ) ) {
        prismaticLogger->errorf( "%s", pd->file->geterr() );
        return "";
    }

    SDFile* f = pd->file->open( "pdxinfo", kFileRead );
    char* buf = sys->realloc( NULL, stat.size + 1 );
    pd->file->read( f, buf, stat.size );
    pd->file->close( f );

    return buf;

}

static char* getPDXInfoValue( char* key ) {

    FileStat stat;
    if( pd->file->stat( "pdxinfo", &stat ) ) {
        prismaticLogger->errorf( "%s", pd->file->geterr() );
        return "";
    }

    SDFile* f = pd->file->open( "pdxinfo", kFileRead );
    char* buf = sys->realloc( NULL, stat.size + 1 );
    pd->file->read( f, buf, stat.size );
    pd->file->close( f );

    char* cursor = buf;

    size_t keyLen = strlen( key );

    while( *cursor ) {
        
        char *lineEnd = strchr( cursor, '\n' );
        if( !lineEnd ) lineEnd = cursor + strlen( cursor );

        if( strncmp( cursor, key, keyLen ) == 0 && cursor[keyLen] == '=' ) {
            const char* valueStart = cursor + keyLen + 1;
            size_t valueLen = lineEnd - valueStart;
            char* result = sys->realloc( NULL, valueLen + 1 );
            
            if( !result ) {
                sys->realloc( buf, 0 );
                return NULL;
            }

            memcpy( result, valueStart, valueLen );
            result[valueLen] = '\0';

            sys->realloc( buf, 0 );

            return result;

        }

        cursor = (*lineEnd) ? lineEnd + 1 : lineEnd;
    }

    prismaticLogger->infof( "Key '%s' not found in pdxinfo!", key );
    sys->realloc( buf, 0 );

    return NULL;

}

const PrismUtils* prismaticUtils = &(PrismUtils) {
    .lerp = lerp,
    .uint8_pow = uint8_pow,
    .randBetween = randBetween,
    .randIntBetween = randIntBetween,
    .getPDXInfo = getPDXInfo,
    .getPDXInfoValue = getPDXInfoValue,
};