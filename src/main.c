// main.c is used to hook into the playdate, override the update callback, and 
// bootstrap the engine. If you are looking to add game logic, you are probably 
// looking for src/core/game.c

#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"
#include "prismatic/prismatic.h"

static int update(void* userdata);

#ifdef _WINDLL
__declspec(dllexport)
#endif

// eventHandler is where the Playdate hooks into the program.
int eventHandler( PlaydateAPI* p, PDSystemEvent event, uint32_t arg ) {

	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit ) {

		// bootstrap Prismatic Engine
		initEngine( p );

		// Note: If you set an update callback in the kEventInit handler, the system
		// assumes the game is pure C and doesn't run any Lua code in the game
		p->system->setUpdateCallback( update, p );

	}
	
	return 0;
}

// Main game loop
static int update( void* userdata ) {

	return 1;

}

