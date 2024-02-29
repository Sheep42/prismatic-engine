#include <stdlib.h>
#include <stdio.h>

#include "../prismatic/prismatic.h"

static void update( float delta );

// update is your game's entry point to the engine
static void update( float delta ) {
	
    string three = newString( "three" );
    prismaticLogger->info( "Hello Logger" );
    prismaticLogger->debug( "Hello Logger" );
    prismaticLogger->error( "Hello Logger" );
    prismaticLogger->infof( "1: %d 2: %s 3: %s", 1, "two", three );
    deleteString( three );

}


// Internal engine setup - Here be dragons
Game* initGame() {

    Game* g = malloc( sizeof( Game ) );

    if (g == NULL) {
        return NULL;
    }

    g->update = update;

	return g;

}