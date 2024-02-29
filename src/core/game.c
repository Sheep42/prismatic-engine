#include <stdlib.h>
#include <stdio.h>

#include "../prismatic/prismatic.h"

static void update( float );
static void test( float );

// update is your game's entry point to the engine
static void update( float delta ) {

    State* s = prismaticState->new( "test" );
    s->tick = test;

    StateMachine* sm = prismaticStateMachine->new( s );
    prismaticStateMachine->update( sm, delta );

}

static void test( float delta ) {

    puts( "hello" );
    prismaticLogger->infof( "%f", delta );

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