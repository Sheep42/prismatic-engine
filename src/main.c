// main.c is used to hook into the playdate, override the update callback, and 
// bootstrap the engine. If you are looking to add game logic, you are probably 
// looking for src/core/game.c
#include <stdlib.h>
#include <stdio.h>

#include "prismatic/prismatic.h"

static int update(void* userdata);
static int clock;

#ifdef _WINDLL
__declspec(dllexport)
#endif

// eventHandler is where the Playdate hooks into the program.
int eventHandler( PlaydateAPI* p, PDSystemEvent event, uint32_t arg ) {

	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit ) {

		// Initialize clock
		clock = p->system->getCurrentTimeMilliseconds();

		// bootstrap Prismatic Engine
		initEngine( p );

		// Note: If you set an update callback in the kEventInit handler, the system
		// assumes the game is pure C and doesn't run any Lua code in the game
		p->system->setUpdateCallback( update, p );

	} else if( event == kEventTerminate ) {
		shutdownEngine();
	}

	return 0;
}

// Main game loop
static int update( void* userdata ) {

	int update = sys->getCurrentTimeMilliseconds();
	float elapsed = (float)( update - clock );
	float delta = elapsed / 1000.0f;

	updateEngine( delta );
	clock = update;

	return 1;

}

