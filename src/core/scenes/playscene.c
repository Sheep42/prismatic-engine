#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../prismatic/prismatic.h"
#include "scenes.h"
#include "splashscene.h"
#include "titlescene.h"

static void enter( Scene* self );
static void exitScene( Scene* self );
static void update( Scene* self, float delta );
static void draw( Scene* self, float delta );
static void destroy( Scene* self );

static void playerUpdate( PrismSprite* self, float delta );

static void handleInput( float delta );

const string PLAYSCENE_NAME = "PlayScene";

typedef enum {
    kDefault = 0,
    kFloor = 1,
    kWall = 2,
} CollisionType;

Scene* newPlayScene( void );

static Scene* playScene;
static PrismSprite* player;
static PDButtons input_current;
static PDButtons input_pressed;
static PDButtons input_released;
static LDtkTileMap* map;

Scene* newPlayScene() {

    if( playScene != NULL ) {
        return playScene;
    }

    playScene = prismaticScene->new( PLAYSCENE_NAME );
    if( playScene == NULL ) {
        prismaticLogger->errorf( "Could not create Scene with name %s", PLAYSCENE_NAME );
    }

    ///////////////////////////////
    // Set the Scene's functions //
    ///////////////////////////////
    playScene->enter = enter;
    playScene->exit = exitScene;
    playScene->update = update;
    playScene->draw = draw;
    playScene->destroy = destroy;

    string* collision = sys->realloc( NULL, sizeof( string ) * 2 + 1 );
    collision[0] = "Collision";
    collision[1] = "Floor";
    collision[2] = NULL;

    /////////////////////////////////
    // Create map from LDTK export //
    /////////////////////////////////
    map = prismaticTileMap->new( "assets/maps/Level_0", 16, collision, NULL );
    if( map == NULL ) {
    	return NULL;
    }

    /////////////////////////////////////
    // Tag map collision layer Sprites //
    /////////////////////////////////////
    prismaticTileMap->tagCollision( map, "Collision", kWall );
    prismaticTileMap->tagCollision( map, "Floor", kFloor );

    /////////////////////////
    // Create a new Sprite //
    /////////////////////////
    string paths[1] = { "assets/images/entities/player/player" };
    player = prismaticSprite->newFromPath( paths, 1, 0 );
    if( player == NULL ) {
    	return NULL;
    }

    ///////////////////////////////////////////////////////////////////
    // Register player update function                               //
    // This runs automatically when the player is added to the Scene //
    ///////////////////////////////////////////////////////////////////
    player->update = playerUpdate;

    ////////////////////////////////////////////////
    // Initialize the Sprite position and z-index //
    ////////////////////////////////////////////////
    sprites->moveTo( player->sprite, 32, 32 );
    sprites->setZIndex( player->sprite, 2 );

    /////////////////////////////////////
    // Set the Sprite's collision rect //
    /////////////////////////////////////
    PDRect bounds = sprites->getBounds( player->sprite );
    bounds.x = 4;
    bounds.y = 4;
    bounds.width -= 8;
    bounds.height -= 8;

    sprites->setCollideRect( player->sprite, bounds );

    /////////////////////////////////
    // Add the Sprite to the Scene //
    /////////////////////////////////
    prismaticScene->add( playScene, "player", player );

    sys->realloc( collision, 0 );
    collision = NULL;

    return playScene;

}

static void enter( Scene* self ) {
    graphics->clear( kColorWhite );
    graphics->setDrawMode( kDrawModeFillBlack );

    ///////////////////////////////////////////
    // Add the map & collision to the screen //
    ///////////////////////////////////////////
    prismaticTileMap->add( map );
    prismaticTileMap->addCollision( map );

    ///////////////////////////////////////////////////
    // Remove Scenes we will never switch back to 	 //
    ///////////////////////////////////////////////////
    SplashScene splashScene = prismaticSceneManager->get( self->sceneManager, SPLASHSCENE_NAME );
    prismaticSceneManager->remove( self->sceneManager, splashScene );
    prismaticScene->delete( splashScene );

}

static void exitScene( Scene* self ) {
	///////////////////////////////////////
	// Remove the map and its collisions //
	///////////////////////////////////////
	prismaticTileMap->removeCollision( map );
	prismaticTileMap->remove( map );
}

static void update( Scene* self, float delta ) {
	handleInput( delta );
}

static void handleInput( float delta ) {
	sys->getButtonState( &input_current, &input_pressed, &input_released );
}

static void draw( Scene* self, float delta ) {

}

static void destroy( Scene* self ) {
	////////////////////
	// Delete the map //
	////////////////////
	prismaticTileMap->delete( map );
}


static void playerUpdate( PrismSprite* self, float delta ) {
	
	////////////////////////////////////
	// Simple movement with collision //
	////////////////////////////////////
	
	float playerX = 0, playerY = 0;

	sprites->getPosition( player->sprite, &playerX, &playerY );
	float newX = playerX, newY = playerY;
	
	if( input_current & kButtonUp ) {
		newY = playerY - 1;
	} else if( input_current & kButtonDown ) {
		newY = playerY + 1;
	}

	if( input_current & kButtonLeft ) {
		newX = playerX - 1;
	} else if( input_current & kButtonRight ) {
		newX = playerX + 1;
	}
	
	sprites->moveWithCollisions( self->sprite, newX, newY, NULL, NULL, NULL );

}
