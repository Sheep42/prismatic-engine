#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>

#include "../prismatic/prismatic.h"

static void init( void );
static void update( float );
static void draw( float );
static void destroy( void );

LCDBitmap** images;
PrismSprite* s;
PrismSprite* s2;
Scene* sc;
Scene* sc2;
SceneManager* sm;

static void spr_upd( PrismSprite* self, float delta ) {
    // self->animation->looping = false;
    prismaticAnimation->play( self->animation, delta );
    // size_t frames[3] = { 0, 3, 1 };
    // prismaticAnimation->playInOrder( self->animation, delta, frames, 3 );
    // sprites->moveBy( self->sprite, 1, 1 );
}

static void spr2_upd( PrismSprite* self, float delta ) {
    sprites->moveBy( self->sprite, 1, 1 );
}

static void sc_enter( Scene* self ) {

    prismaticScene->add( self, "spr1", s );
    prismaticScene->add( self, "spr2", s2 );
    sprites->moveTo( self->sprites[0]->sprite, 50, 50 );
    // sprites->moveTo( self->sprites[1]->sprite, 0, 0 );
    
}

static void sc2_enter( Scene* self ) {}

static void sc_upd( Scene* self, float delta ) {

}

static void sc_draw( Scene* self, float delta ) {

}

// Use init to hook into the Engine's initialization function. This runs 
// before the first call to update but after the engine has initialized itself
static void init() {
    prismaticLogger->info( "init" );

    string paths[4] = { 
        "assets/images/entities/test/anim-1",
        "assets/images/entities/test/anim-2",
        "assets/images/entities/test/anim-3",
        "assets/images/entities/test/anim-4",
    };

    images = prismaticSprite->loadImages( paths, 4 );

    s = prismaticSprite->newFromImages( images, 0, 0.5f );
    s->update = spr_upd;

    s2 = prismaticSprite->newFromPath( "assets/images/entities/player/player" );
    s2->update = spr2_upd;

    sc = prismaticScene->new( "Scene 1" );
    sc2 = prismaticScene->new( "Scene 2" );

    sc->enter = sc_enter;
    sc->update = sc_upd;
    sc->draw = sc_draw;

    sc2->enter = sc2_enter;
    sc2->update = sc_draw;
    sc2->draw = sc_draw;

    sm = prismaticSceneManager->new( sc );
    prismaticSceneManager->addScene( sm, sc2 );
}

float e;
// update is your game's entry point to the engine
static void update( float delta ) {
    sprites->updateAndDrawSprites();
    sys->drawFPS( 0, 0 );
    prismaticSceneManager->update( sm, delta );

    e += delta;

    if( e >= 5.0f ) {
        prismaticSceneManager->changeScene( sm, sc2 );
    }

    if( e >= 10.0f ) {
        prismaticSceneManager->changeScene( sm, sc );
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
    prismaticSprite->freeImages( images );
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