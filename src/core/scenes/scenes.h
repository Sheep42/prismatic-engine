#ifndef GAME_SCENE_H

#ifndef PRISMATIC_INCLUDED
	#include "../../prismatic/prismatic.h"
	#define PRISMATIC_INCLUDED
#endif

#define GAME_SCENE_H

SceneManager* initScenes( void );

typedef Scene* SplashScene;
typedef Scene* TitleScene;
typedef Scene* PlayScene;

extern const string FONT_PATH_SYSTEM;
extern const LCDFont* FONT_SYSTEM;

#endif