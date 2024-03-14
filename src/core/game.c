#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "time.h"
#include "test.h"
#include "../prismatic/prismatic.h"

static void init( void );
static void update( float );
static void draw( float );
static void destroy( void );

static SceneManager* sm;
static PrismTransition* transition;

// Use init to hook into the Engine's initialization function. This runs 
// before the first call to update but after the engine has initialized itself
static void init() {
    
    srand(time(NULL));

    prismaticLogger->info( "init" );
    sm = initScenes();
    transition = prismaticTransition->new( NULL, 0, 0, 0.05f, PrismTransitionType_RTLOut );

}

float e = 0.0f;
bool bgGot = false;
LCDBitmap* bg;
LCDBitmap* origBg;

// update is your game's entry point to the engine
static void update( float delta ) {
    sprites->updateAndDrawSprites();
    sys->drawFPS( 0, 0 );
    prismaticSceneManager->update( sm, delta );

    e += delta;
    if( e >= 1.0f && !bgGot ) {
        origBg = graphics->copyBitmap( graphics->getDisplayBufferBitmap() );

        Scene* sc1 = prismaticSceneManager->get( sm, "Scene 1" );
        PrismSprite* sp1 = prismaticScene->get( sc1, "spr1" );
        prismaticScene->remove( sc1, sp1 );

        bgGot = true;
        bg = graphics->copyBitmap( origBg );
        transition->image = bg;

    }

    if( bgGot ) {

        prismaticTransition->play( transition, delta );

    }

//     if( e >= 5.0f ) {
//         prismaticSceneManager->changeScene( sm, sm->scenes[1] );
//     }

//     if( e >= 10.0f ) {
//         prismaticSceneManager->changeScene( sm, sm->scenes[0] );
//         e = 0;
//     }
}

// draw should be used to handle screen drawing operations. It is called after 
// update
static void draw( float delta ) {

}

// destroy is called when the game is shut down, before the game itself is
// freed from memory
static void destroy() {
    prismaticLogger->info( "destroy" );
    prismaticSceneManager->delete( sm );
    prismaticTransition->delete( transition );
    graphics->freeBitmap( bg );
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