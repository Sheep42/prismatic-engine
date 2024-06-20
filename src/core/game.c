#include <stdlib.h>
#include <stdio.h>

#include "../prismatic/prismatic.h"
#include "scenes/scenes.h"

static void init( void );
static void update( float );
static void draw( float );
static void destroy( void );

static SceneManager* sceneManager;

// Use init to hook into the Engine's initialization function. This runs 
// before the first call to update but after the engine has initialized itself
static void init() {
    sceneManager = initScenes();
}

// update is your game's entry point to the engine
static void update( float delta ) {
    prismaticSceneManager->update( sceneManager, delta );
}

// draw should be used to handle screen drawing operations. It is called after 
// update
static void draw( float delta ) {
    sprites->updateAndDrawSprites();
    prismaticSceneManager->draw( sceneManager, delta );
    sys->drawFPS( 0, 0 );
}

// destroy is called when the game is shut down, before the game itself is
// freed from memory
static void destroy() {
    prismaticSceneManager->delete( sceneManager );
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