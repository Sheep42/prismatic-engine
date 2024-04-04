// prismatic.h

#ifndef PRISMATIC_H
#define PRISMATIC_H

#ifndef PD_API_INCLUDED
	#define PD_API_INCLUDED
	#include "pd_api.h"
#endif

#ifndef UTILS_INCLUDED
    #define UTILS_INCLUDED
    #include "utils/utils.h"
#endif

#ifndef GAME_INCLUDED
	#define GAME_INCLUDED
	#include "../core/game.h"
#endif

#ifndef TEXT_INCLUDED
	#define TEXT_INCLUDED
	#include "text/text.h"
#endif

#ifndef SPRITE_INCLUDED
	#define SPRITE_INCLUDED
	#include "sprite/sprite.h"
#endif

#ifndef SCENE_INCLUDED
	#define SCENE_INCLUDED
	#include "scene/scene.h"
#endif

#ifndef STATE_INCLUDED
	#define STATE_INCLUDED
	#include "state/state_machine.h"
#endif

#ifndef TRANSITION_INCLUDED
	#define TRANSITION_INCLUDED
	#include "transition/transition.h"
#endif

#ifndef LDTK_INCLUDED
	#define LDTK_INCLUDED
	#include "tilemap/ldtk.h"
#endif

#ifndef LOGGER_INCLUDED
	#define LOGGER_INCLUDED
	#include "logger/logger.h"
#endif

void initEngine( PlaydateAPI* );
void updateEngine( float );
void shutdownEngine( void );

extern const PlaydateAPI *pd;
extern const struct playdate_graphics *graphics;
extern const struct playdate_sound *sound;
extern const struct playdate_sprite *sprites;
extern const struct playdate_sys *sys;

#endif // PRISMATIC_H