#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "test.h"
#include "../prismatic/prismatic.h"

static void init( void );
static void update( float );
static void draw( float );
static void destroy( void );

static SceneManager* sm;

// Use init to hook into the Engine's initialization function. This runs 
// before the first call to update but after the engine has initialized itself
static void init() {
    prismaticLogger->info( "init" );
    sm = initScenes();
}

float e;
// update is your game's entry point to the engine
static void update( float delta ) {
    sprites->updateAndDrawSprites();
    sys->drawFPS( 0, 0 );
    prismaticSceneManager->update( sm, delta );

    e += delta;

    if( e >= 5.0f ) {
        prismaticSceneManager->changeScene( sm, sm->scenes[1] );
    }

    if( e >= 10.0f ) {
        prismaticSceneManager->changeScene( sm, sm->scenes[0] );
        e = 0;
    }
}

// draw should be used to handle screen drawing operations. It is called after 
// update
static void draw( float delta ) {

}

// destroy is called when the game is shut down, before the game itself is
// freed from memory
static void destroy() {
    prismaticLogger->info( "destroy" );
    
    // prismaticSprite->delete( s );
    // prismaticSprite->delete( s2 );

    prismaticSceneManager->delete( sm );
}

// Called by the Engine to bind the game to itself - if you are going to modify
// this do so with caution.
Game* newGame() {

    Game* g = calloc( 1, sizeof( Game ) );

    if (g == NULL) {
        return NULL;
    }

    g->update = update;
    g->init = init;
    g->draw = draw;
    g->destroy = destroy;

    return g;

}