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

	g = newGame();
	g->init();

}

void updateEngine( float delta ) {

	if( g != NULL ) {
		g->update( delta );
		g->draw( delta );
	}

}

void shutdownEngine() {
	g->destroy();
	free( (void*)g ); // Might not be able to do this.
}

// String Helpers

static string newString( string str ) {
	return strdup( str );
}

static void deleteString( string str ) {
	free( str );
}

const StringUtils* prismaticString = &(StringUtils) {
	.new = newString,
	.delete = deleteString,
};