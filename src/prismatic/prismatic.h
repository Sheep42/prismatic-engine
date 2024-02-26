#include "pd_api.h"
#include "../core/game.h"
#include "state/state.h"

void initEngine( PlaydateAPI* );
void runEngine( float );
void shutdownEngine();

extern const PlaydateAPI *pd;
extern const struct playdate_graphics *graphics;
extern const struct playdate_sound *sound;
extern const struct playdate_sprite *sprite;
extern const struct playdate_sys *sys;
extern const Game* g;