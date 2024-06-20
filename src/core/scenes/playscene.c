#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../prismatic/prismatic.h"
#include "scenes.h"

static void enter( Scene* self );
static void exitScene( Scene* self );
static void update( Scene* self, float delta );
static void draw( Scene* self, float delta );
static void destroy( Scene* self );

static void handleInput( float delta );

const string PLAYSCENE_NAME = "PlayScene";

typedef enum {
    kDefault = 0,
    kFloor = 1,
    kWall = 2,
} CollisionType;

Scene* newPlayScene( void );

static Scene* playScene;
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
    map = prismaticTileMap->new( "assets/maps/Level_0", 16, collision );

    /////////////////////////////////////
    // Tag map collision layer Sprites //
    /////////////////////////////////////
    prismaticTileMap->tagCollision( map, "Collision", kWall );
    prismaticTileMap->tagCollision( map, "Floor", kFloor );

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
}

static void exitScene( Scene* self ) {

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
	prismaticTileMap->removeCollision( map );
	prismaticTileMap->remove( map );
	prismaticTileMap->delete( map );
}