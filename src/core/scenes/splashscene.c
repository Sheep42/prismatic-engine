#include <stdlib.h>
#include <stdio.h>

#include "titlescene.h"
#include "splashscene.h"
#include "../../prismatic/prismatic.h"

static void enter( Scene* self );
static void exitScene( Scene* self );
static void update( Scene* self, float delta );
static void draw( Scene* self, float delta );
static void destroy( Scene* self );

static void transitionInComplete( PrismTransition* self );
static void transitionOutComplete( PrismTransition* self );

const string SPLASHSCENE_NAME = "SplashScreen";

static Scene* splashScene;
static LCDBitmap* logo;
static PrismTransition* transitionIn;
static PrismTransition* transitionOut;
static PrismTransition* currentTransition;
static FilePlayer* startupPlayer;

Scene* newSplashScene() {

	if( splashScene != NULL ) {
		return splashScene;
	}

	splashScene = prismaticScene->new( SPLASHSCENE_NAME );
	if( splashScene == NULL ) {
		prismaticLogger->errorf( "Could not create new Scene with name %s", SPLASHSCENE_NAME );
	}

	splashScene->enter = enter;
	splashScene->exit = exitScene;
	splashScene->update = update;
	splashScene->draw = draw;
	splashScene->destroy = destroy;

	logo = graphics->loadBitmap( "assets/images/ui/logo", NULL );
	if( logo == NULL ) {
		prismaticLogger->errorf( "%s: Could not load logo", SPLASHSCENE_NAME );
	}

	transitionIn = prismaticTransition->new( logo, 0, 0, 0.025f, PrismTransitionType_FadeIn );
	if( transitionIn == NULL ) {
		prismaticLogger->error( "Could not create fade in transition!" );
	}

	transitionIn->completeDelay = 1.25f;
	transitionIn->complete = transitionInComplete;

	transitionOut = prismaticTransition->new( logo, 0, 0, 0.025f, PrismTransitionType_FadeOut );
	if( transitionOut == NULL ) {
		prismaticLogger->error( "Could not create fade out transition!" );
	}

	transitionOut->completeDelay = 0.25f;
	transitionOut->complete = transitionOutComplete;

	currentTransition = transitionIn;

	startupPlayer = sound->fileplayer->newPlayer();
	sound->fileplayer->loadIntoPlayer( startupPlayer, "assets/sound/startup" );

	return splashScene;

}

static void enter( Scene* self ) {
	sound->fileplayer->play( startupPlayer, 1 );
}

static void exitScene( Scene* self ) {}

static void update( Scene* self, float delta ) {

}

static void draw( Scene* self, float delta ) {
	prismaticTransition->play( currentTransition, delta );
}

static void destroy( Scene* self ) {
	graphics->freeBitmap( logo );
	sound->fileplayer->freePlayer( startupPlayer );
	prismaticTransition->delete( transitionIn );
	prismaticTransition->delete( transitionOut );
}

static void transitionInComplete( PrismTransition* self ) {
	currentTransition = transitionOut;
}

static void transitionOutComplete( PrismTransition* self ) {
	prismaticSceneManager->changeSceneByName( splashScene->sceneManager, TITLESCENE_NAME );
}