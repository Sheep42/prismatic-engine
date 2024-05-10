#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>

#include "../prismatic.h"

static char* getCurrentTimestamp(void);

static void prismLog( const char* str ) {
    char* timestamp = getCurrentTimestamp();
    sys->logToConsole( "%s %s", timestamp, str );
    free( timestamp );
}

static void infof( const char* fmt, ... ) {

    va_list args, finalArgs;
    va_start( args, fmt );
    va_copy( finalArgs, args );

    const char* info = "INFO:";

    char* fmtBuf = malloc( (strlen( fmt ) * sizeof(char)) + (strlen( info ) * sizeof(char)) + 2 );
    if( fmtBuf == NULL ) {
        sys->error( "ERROR: Could not allocate space for fmtBuf" );
        return;
    }

    sprintf( fmtBuf, "%s %s", info, fmt );

    int parsedLen = vsnprintf( NULL, 0, fmtBuf, args );
    char* msg = malloc( (parsedLen * sizeof(char)) + 1 );
    if( msg == NULL ) {
        sys->error( "ERROR: Could not allocate space for msg" );
        free( fmtBuf );
        return;
    }

    vsprintf( msg, fmtBuf, finalArgs );
    prismLog( msg );

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
    if( fmtBuf == NULL ) {
        sys->error( "ERROR: Could not allocate space for fmtBuf" );
        return;
    }

    sprintf( fmtBuf, "%s %s", debug, fmt );

    int parsedLen = vsnprintf( NULL, 0, fmtBuf, args );
    char* msg = malloc( (parsedLen * sizeof(char)) + 1 );
    if( msg == NULL ) {
        sys->error( "ERROR: Could not allocate space for msg" );
        free( fmtBuf );
        return;
    }

    vsprintf( msg, fmtBuf, finalArgs );
    prismLog( msg );

    va_end( args );
    va_end( finalArgs );

    free( fmtBuf );
    free( msg );

}

static void errorf( const char* fmt, ... ) {

    va_list args, finalArgs;
    va_start( args, fmt );
    va_copy( finalArgs, args );

    const char* error = "ERROR:";

    char* fmtBuf = malloc( (strlen( fmt ) * sizeof(char)) + (strlen( error ) * sizeof(char)) + 2 );
    if( fmtBuf == NULL ) {
        sys->error( "ERROR: Could not allocate space for fmtBuf" );
        return;
    }
    sprintf( fmtBuf, "%s %s", error, fmt );

    int parsedLen = vsnprintf( NULL, 0, fmtBuf, args );
    char* msg = malloc( (parsedLen * sizeof(char)) + 1 );
    if( msg == NULL ) {
        sys->error( "ERROR: Could not allocate space for fmtBuf" );
        free( fmtBuf );
        return;
    }

    vsprintf( msg, fmtBuf, finalArgs );
    sys->error( msg );

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

static void error( const char* msg ) {
	errorf( "%s", msg );
}

static char* getCurrentTimestamp() {

	time_t rawtime;
	struct tm *timeinfo;
	int bufSize = 80;
    char* buffer = malloc( bufSize * sizeof( char ) );

    time( &rawtime );
    timeinfo = localtime( &rawtime );

    strftime( buffer, bufSize, "%Y-%m-%d %H:%M:%S", timeinfo );
    return buffer;

}

const Logger* prismaticLogger = &(Logger){
	.info = info,
	.infof = infof,
	.debug = debug,
	.debugf = debugf,
	.error = error,
	.errorf = errorf,
};