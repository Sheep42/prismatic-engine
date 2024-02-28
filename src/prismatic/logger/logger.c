#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "logger.h"

static char* getCurrentTimestamp(void);

static void log( const char* str ) {
    char* timestamp = getCurrentTimestamp();
    printf( "%s %s\n", timestamp, str );
    free( timestamp );
}

static void infof( const char* fmt, ... ) {

    va_list args, finalArgs;
    va_start( args, fmt );
    va_copy( finalArgs, args );

    const char* info = "INFO:";

    char* fmtBuf = malloc( (strlen( fmt ) * sizeof(char)) + (strlen( info ) * sizeof(char)) + 2 );
    sprintf( fmtBuf, "%s %s", info, fmt );

    int parsedLen = vsnprintf( NULL, 0, fmtBuf, args );
    char* msg = malloc( (parsedLen * sizeof(char)) + 1 );

    vsprintf( msg, fmtBuf, finalArgs );
    log( msg );

    va_end( args );
    va_end( finalArgs );

    free( fmtBuf );
    free( msg );

}

static void debugf( const char* fmt, ... ) {

    va_list args, finalArgs;
    va_start( args, fmt );
    va_copy( finalArgs, args );

    const char* debug = "DEBUG:";

    char* fmtBuf = malloc( (strlen( fmt ) * sizeof(char)) + (strlen( debug ) * sizeof(char)) + 2 );
    sprintf( fmtBuf, "%s %s", debug, fmt );

    int parsedLen = vsnprintf( NULL, 0, fmtBuf, args );
    char* msg = malloc( (parsedLen * sizeof(char)) + 1 );

    vsprintf( msg, fmtBuf, finalArgs );
    log( msg );

    va_end( args );
    va_end( finalArgs );

    free( fmtBuf );
    free( msg );

}

static void info( const char* msg ) {
	infof( "%s", msg );
}

static void debug( const char* msg ) {
	debugf( "%s", msg );
}

static char* getCurrentTimestamp() {

	time_t rawtime;
	struct tm *timeinfo;
	int bufSize = 80;
    char* buffer = malloc( bufSize * sizeof( char ) );

    time( &rawtime );
    timeinfo = localtime( &rawtime );

    // fixme: buffer is cutting off timestamp
    strftime( buffer, bufSize, "%Y-%m-%d %H:%M:%S", timeinfo );
    return buffer;

}

const Logger* prismaticLogger = &(Logger){
	.info = info,
	.infof = infof,
	.debug = debug,
	.debugf = debugf,
};