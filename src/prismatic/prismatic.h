#include "pd_api.h"

extern const PlaydateAPI *pd;
extern const struct playdate_graphics *graphics;
extern const struct playdate_sound *sound;
extern const struct playdate_sprite *sprite;
extern const struct playdate_sys *sys;

struct Game {};

void initEngine( PlaydateAPI* pd );