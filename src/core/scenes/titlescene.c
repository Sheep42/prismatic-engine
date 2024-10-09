#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>

#include "../../prismatic/prismatic.h"
#include "scenes.h"
#include "splashscene.h"
#include "titlescene.h"
#include "playscene.h"

static void enter( Scene* self );
static void exitScene( Scene* self );
static void update( Scene* self, float delta );
static void draw( Scene* self, float delta );
static void destroy( Scene* self );

static void transitionComplete( PrismTransition* self );

static void handleInput( float delta );

const string TITLESCENE_NAME = "TitleScene";
static const string START_TEXT = "Press A or B to Start";

Scene* newTitleScene( void );

static Scene* titleScene;
static int textWidth;
static uint8_t fontHeight;
static PDButtons input_current;
static PDButtons input_pressed;
static PDButtons input_released;
static bool startGame = false;
static LCDBitmap* screencap;

static PrismTransition* transitionOut;

Scene* newTitleScene() {

	if( titleScene != NULL ) {
		return titleScene;
	}

	titleScene = prismaticScene->new( TITLESCENE_NAME );
	if( titleScene == NULL ) {
		prismaticLogger->errorf( "Could not create Scene with name %s", TITLESCENE_NAME );
	}

	titleScene->enter = enter;
	titleScene->exit = exitScene;
	titleScene->update = update;
	titleScene->draw = draw;
	titleScene->destroy = destroy;

	transitionOut = prismaticTransition->new( NULL, 0, 0, 0.05f, PrismTransitionType_FadeOut );
	transitionOut->complete = transitionComplete;

	return titleScene;

}

static void enter( Scene* self ) {

	textWidth = graphics->getTextWidth( FONT_SYSTEM, START_TEXT, strlen( START_TEXT ), kUTF8Encoding, graphics->getTextTracking() );
	fontHeight = graphics->getFontHeight( FONT_SYSTEM );

}

static void exitScene( Scene* self ) {

}

static void update( Scene* self, float delta ) {

	if( startGame ) {

		if( transitionOut->image == NULL ) {
			screencap = graphics->copyBitmap( graphics->getDisplayBufferBitmap() );
			transitionOut->image = screencap;
		}

		return;
	}

	handleInput( delta );

}

static void handleInput( float delta ) {
	sys->getButtonState( &input_current, &input_pressed, &input_released );
	startGame = ( input_pressed & kButtonA || input_pressed & kButtonB );
}

static void draw( Scene* self, float delta ) {
	
	if( startGame ) {
		if( transitionOut->image != NULL ) {
			prismaticTransition->play( transitionOut, delta );
		}

		return;
	}

	// Draw background overlay
	graphics->fillRect( 0, 0, pd->display->getWidth(), pd->display->getHeight(), kColorBlack );

	// Draw Start Text
	graphics->setDrawMode( kDrawModeInverted );
	graphics->setFont( FONT_SYSTEM );

	graphics->drawText( 
		START_TEXT, strlen( START_TEXT ), 
		kUTF8Encoding, 
		( pd->display->getWidth() / 2 ) - ( textWidth / 2 ), 
		( pd->display->getHeight() / 2 ) - ( fontHeight / 2 )
	);

}

static void destroy( Scene* self ) {
	graphics->freeBitmap( screencap );
}

static void transitionComplete( PrismTransition* self ) {
	prismaticSceneManager->changeSceneByName( titleScene->sceneManager, PLAYSCENE_NAME );
}
