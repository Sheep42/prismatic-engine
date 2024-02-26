#include <stdio.h>
#include <stdlib.h>

#include "prismatic.h"
#include "pd_api.h"

const struct PlaydateAPI *pd;
const struct playdate_graphics *graphics;
const struct playdate_sound *sound;
const struct playdate_sprite *sprite;
const struct playdate_sys *sys;

void initEngine( PlaydateAPI* p ) {

	pd = p;
	graphics = pd->graphics;
	sound = pd->sound;
	sprite = pd->sprite;
	sys = pd->system;

}