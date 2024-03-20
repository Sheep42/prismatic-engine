#include "customscenes.h"
#include "../prismatic/prismatic.h"

static LCDBitmap** images;
static float elapsed = 0.0f;
static LCDBitmap* bg;
static bool bgGot = false;

static SceneManager* sm;
static PrismTransition* transition;

static void spr1_update( PrismSprite* self, float delta );
static void spr2_update( PrismSprite* self, float delta );
static void transition_complete( PrismTransition* self );
static void sc1_enter( Scene* self );
static void sc2_enter( Scene* self );
static void sc1_update( Scene* self, float delta );
static void sc_draw( Scene* self, float delta );
static void sm_destroy( SceneManager* sceneManager );

//////////////
// Sprite 1 //
//////////////
static void spr1_update( PrismSprite* self, float delta ) {
    prismaticAnimation->play( self->animation, delta ); // Play Sprite 1's animation every time we run sprite->update()
}

//////////////
// Sprite 2 //
//////////////
static void spr2_update( PrismSprite* self, float delta ) {
    sprites->moveBy( self->sprite, 1, 1 ); // Move Sprite 2 by 1px every time we run sprite->update()
}

////////////////
// Transition //
////////////////
static void transition_complete( PrismTransition* self ) {
    prismaticSceneManager->changeSceneByName( sm, "Scene 2" ); // Change the current Scene to Scene 2
}


/////////////
// Scene 1 //
/////////////
static void sc1_enter( Scene* self ) {
    PrismSprite* s = prismaticScene->get( self, "spr1" ); // Get Sprite 1 from the Scene
    sprites->moveTo( s->sprite, 50, 50 ); // Set Sprite 1's position to 50, 50

    transition = prismaticTransition->new( NULL, 0, 0, 0.05f, PrismTransitionType_ShrinkToCenter ); // Initialize a new ShrinkToCenter Transition that animates in intervals of 0.05 seconds
    transition->complete = transition_complete; // Set the Transition's complete method to transition_complete
}

static void sc1_update( Scene* self, float delta ) {

    elapsed += delta; // Keep track of total seconds elapsed with each frame
    if( elapsed >= 5.0f && !bgGot ) {  // When we hit 5 seconds for the first time
        bg = graphics->copyBitmap( graphics->getDisplayBufferBitmap() ); // Take a snapshot of the current frame and copy it into bg

        PrismSprite* sp1 = prismaticScene->get( self, "spr1" ); // Get Sprite 1 from the Scene
        prismaticScene->remove( self, sp1 ); // Remove Sprite 1 from the Scene
        transition->image = bg; // Set the Transition's image to our snapshot

        bgGot = true; // Flag that we've been here already
    }

    if( bgGot ) {
        prismaticTransition->play( transition, delta ); // Play the transition
    }

}

static void sc2_update( Scene* self, float delta ) {
    prismaticLogger->info( "Scene 2" );
}


static void sc_draw( Scene* self, float delta ) {
    // Nothing to do...
}


/////////////
// Scene 2 //
/////////////
static void sc2_enter( Scene* self ) {}

///////////////////
// Scene Manager //
///////////////////
static void sm_destroy( SceneManager* sceneManager ) {
    prismaticTransition->delete( transition ); // Destroy the Transition and free its internals
    graphics->freeBitmap( bg ); // Explicitly free the bg snapshot that we took in Scene 1
    prismaticSprite->freeImages( images ); // Explicitly free the images array that we used to create Sprite 1
}

//////////////////////////////////////////
// Set up the Scenes - Called from game //
//////////////////////////////////////////
SceneManager* initScenes() {

    PrismSprite* s;
    PrismSprite* s2;

    Scene* sc1;
    Scene* sc2;

    /////////////////////////////////
    // Load Bitmaps from path list //
    /////////////////////////////////
    string paths[4] = { 
        "assets/images/entities/test/anim-1",
        "assets/images/entities/test/anim-2",
        "assets/images/entities/test/anim-3",
        "assets/images/entities/test/anim-4",
    };

    images = prismaticSprite->loadImages( paths, 4 );


    ///////////////////////////////////////////
    // Create Sprite 1 from loaded images // //
    ///////////////////////////////////////////
    s = prismaticSprite->newFromImages( images, 0, 0.10f );
    s->update = spr1_update; // Set Sprite 1's update method


    ////////////////////////////////////////
    // Create Sprite 2 directly from path //
    ////////////////////////////////////////
    string paths2[1] = { "assets/images/entities/player/player" };
    s2 = prismaticSprite->newFromPath( paths2, 1 );
    s2->update = spr2_update; // Set Sprite 2's update method


    ////////////////////
    // Create Scene 1 //
    ////////////////////
    sc1 = prismaticScene->new( "Scene 1" );

    ///////////////////////////
    // Set Scene 1's methods //
    ///////////////////////////
    sc1->enter = sc1_enter;
    sc1->update = sc1_update;
    sc1->draw = sc_draw;


    ////////////////////
    // Create Scene 2 //
    ////////////////////
    sc2 = prismaticScene->new( "Scene 2" );

    ///////////////////////////
    // Set Scene 2's methods //
    ///////////////////////////
    sc2->enter = sc2_enter;
    sc2->update = sc2_update;
    sc2->draw = sc_draw;


    /////////////////////////////
    // Add Sprite 1 to Scene 1 //
    /////////////////////////////
    prismaticScene->add( sc1, "spr1", s );


    /////////////////////////////
    // Add Sprite 2 to Scene 2 //
    /////////////////////////////
    prismaticScene->add( sc2, "spr2", s2 );


    ///////////////////////////////////////////////////////
    // Create the Scene Manager, with Scene 1 as default //
    ///////////////////////////////////////////////////////
    sm = prismaticSceneManager->new( sc1 );
    sm->destroy = sm_destroy; // Set the Scene Manager's destroy method


    ///////////////////////////////////////
    // Add Scene 2 to the Scene Manager  //
    ///////////////////////////////////////
    prismaticSceneManager->add( sm, sc2 );

    return sm;

}