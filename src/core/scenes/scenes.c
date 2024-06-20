#include <stdlib.h>
#include <stdio.h>

#include "../../prismatic/prismatic.h"
#include "scenes.h"
#include "splashscene.h"
#include "titlescene.h"
#include "playscene.h"

static SceneManager* sm;
static SplashScene splashScene;
static TitleScene titleScene;
static PlayScene playScene;

const string FONT_PATH_SYSTEM = "/System/Fonts/Asheville-Sans-14-Bold.pft";
const string FONT_PATH_PIXELSPLITTER = "assets/fonts/pixelsplitter";

const LCDFont* FONT_SYSTEM = NULL;
const LCDFont* FONT_PIXELSPLITTER = NULL;

SceneManager* initScenes() {

	splashScene = newSplashScene();
	titleScene = newTitleScene();
	playScene = newPlayScene();

	sm = prismaticSceneManager->new( splashScene );
	if( sm == NULL ) {
		prismaticLogger->error( "Could not initialize Scene Manager!" );
		return NULL;
	} 

	prismaticSceneManager->add( sm, titleScene );
	prismaticSceneManager->add( sm, playScene );

	const char** err = NULL;
	FONT_SYSTEM = graphics->loadFont( FONT_PATH_SYSTEM, err );
	if( FONT_SYSTEM == NULL ) {
		prismaticLogger->errorf( "Could not load system font. err: %s", err );
	}

	err = NULL;
	FONT_PIXELSPLITTER = graphics->loadFont( FONT_PATH_PIXELSPLITTER, err );
	if( FONT_PIXELSPLITTER == NULL ) {
		prismaticLogger->errorf( "Could not load pixelsplitter font. err: %s", err );
	}

	return sm;

}