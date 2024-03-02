#include <stdlib.h>
#include <stdio.h>

#include "../prismatic/prismatic.h"

static void init( void );
static void update( float );
static void draw( float );
static void destroy( void );

LCDBitmap** images;
PrismSprite* s;

// Use init to hook into the Engine's initialization function. This runs 
// before the first call to update but after the engine has initialized itself
static void init() {
    prismaticLogger->info( "init" );
    
    string paths[1] = { "assets/images/entities/player/player" };
    images = prismaticSprite->loadImages( paths, 1 );

    // PrismSprite* s = prismaticSprite->newFromPath( paths[0] );
    s = prismaticSprite->newFromImages( images, 0, 0 );
    sprite->addSprite( s->sprite );
    sprite->moveTo( s->sprite, 100, 100 );
}

// update is your game's entry point to the engine
static void update( float delta ) {
    sprite->updateAndDrawSprites();
    prismaticLogger->info( "test" );
}

// draw should be used to handle screen drawing operations. It is called after 
// update
static void draw( float delta ) {

}

// destroy is called when the game is shut down, before the game itself is
// freed from memory
static void destroy() {
    prismaticLogger->info( "destroy" );
    prismaticSprite->delete( s );
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