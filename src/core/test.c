#include "test.h"
#include "../prismatic/prismatic.h"

static LCDBitmap** images;

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
    PrismSprite* s = prismaticScene->get( self, "spr1" );
    sprites->moveTo( s->sprite, 50, 50 );
    // sprites->moveTo( self->sprites[1]->sprite, 0, 0 );
}

static void sc2_enter( Scene* self ) {}

static void sc_upd( Scene* self, float delta ) {

}

static void sc_draw( Scene* self, float delta ) {

}

static void sm_destroy( SceneManager* sceneManager ) {
    prismaticSprite->freeImages( images );
}

SceneManager* initScenes() {

    PrismSprite* s;
    PrismSprite* s2;
    Scene* sc;
    Scene* sc2;
    SceneManager* sm;

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

    prismaticScene->add( sc, "spr1", s );
    prismaticScene->add( sc2, "spr2", s2 );

    sm = prismaticSceneManager->new( sc );
    sm->destroy = sm_destroy;

    prismaticSceneManager->addScene( sm, sc2 );

    return sm;

}