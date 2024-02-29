#include "pd_api.h"
#include "../core/game.h"
#include "state/state.h"
#include "logger/logger.h"

typedef char* string;

string newString( string );
void deleteString( string );

void initEngine( PlaydateAPI* );
void updateEngine( float );
void shutdownEngine( void );

extern const PlaydateAPI *pd;
extern const struct playdate_graphics *graphics;
extern const struct playdate_sound *sound;
extern const struct playdate_sprite *sprite;
extern const struct playdate_sys *sys;
extern const Game* g;