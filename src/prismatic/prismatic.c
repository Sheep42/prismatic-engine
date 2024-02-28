#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "prismatic.h"
#include "pd_api.h"

const struct PlaydateAPI *pd;
const struct playdate_graphics *graphics;
const struct playdate_sound *sound;
const struct playdate_sprite *sprite;
const struct playdate_sys *sys;
const Game* g;

void initEngine( PlaydateAPI* p ) {

	pd = p;
	graphics = pd->graphics;
	sound = pd->sound;
	sprite = pd->sprite;
	sys = pd->system;

	g = initGame();

}

void updateEngine( float delta ) {

	if( g != NULL ) {
		g->update( delta );
	}

}

void shutdownEngine() {
	free( (void*)g ); // Might not be able to do this.
}

string strNew( string str ) {
	return strdup( str );
}

void strDelete( string str ) {
	free( str );
}