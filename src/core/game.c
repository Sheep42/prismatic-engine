#include <stdlib.h>
#include <stdio.h>

#include "../prismatic/prismatic.h"

static void update( float delta );

// update is your game's entry point to the engine
static void update( float delta ) {
	
	puts( "Hello Playdate" );

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