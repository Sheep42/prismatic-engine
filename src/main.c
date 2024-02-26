#include <stdio.h>
#include <stdlib.h>

#include "pd_api.h"

static PlaydateAPI* pd;
static int update(void* userdata);
const float JUMP_STRENGTH = -20.0;
const float GRAVITY = 1.0;
const float WORD_SPEED = 10.0;
const char* fontpath = "/System/Fonts/Asheville-Sans-14-Bold.pft";
const char *playerImgPath = "assets/images/entities/player/player";
const char* words[2] = { "Hello", "World" };
LCDFont* font = NULL;
LCDSprite* player = NULL;
LCDBitmap *playerImg = NULL;
float playerVelY = 0;
int wordPtr = 0;
float wordX = 0, wordY = 0;
int shot = 0;

#ifdef _WINDLL
__declspec(dllexport)
#endif

static void handleMovement( LCDSprite* sprite, PDButtons current, PDButtons pressed, PDButtons released ) {

	float posX, posY;
	pd->sprite->getPosition( sprite, &posX, &posY );

	int w, h;
	pd->graphics->getBitmapData( playerImg, &w, &h, NULL, NULL, NULL );

	int jumping = 0;

	float dx = 0;
	// float dy = ( posY < LCD_ROWS - h - 5 ) ? GRAVITY : 0;
	float dy = 0;

	if ( current & kButtonLeft ) {
		dx = -4;
	} else if ( current & kButtonRight ) {
		dx = 4;
	}

	if( posY < LCD_ROWS - h - 5 ) {
		jumping = 1;
	}

	if ( ( current & kButtonA ) && jumping == 0 ) {
		playerVelY = JUMP_STRENGTH;
	}

	playerVelY += GRAVITY;

	if( jumping == 1 ) {

		// Apply gradual deceleration when reaching the peak of the jump
		if( playerVelY < 0.0f ) {
			playerVelY *= 0.9f;  // Adjust the deceleration factor
		}

	}

	if( posY >= LCD_ROWS - h - 5 && playerVelY > 0.0f ) {
		jumping = 0;
		playerVelY = 0.0f;  // Reset velocity when on the ground
	}

	dy += playerVelY;

	// Apply movement
	pd->sprite->moveBy( sprite, dx, dy );

}

static void handleShoot( LCDSprite* sprite, PDButtons current, PDButtons pressed, PDButtons released ) {

	if( ( current & kButtonB ) && shot == 0 ) {
		pd->sprite->getPosition( sprite, &wordX, &wordY );
		wordY -= 5;
		shot = 1;
	}

}

static void playerUpdate( LCDSprite* sprite ) {

	// Check inputs
	PDButtons current, pressed, released;
	pd->system->getButtonState( &current, &pressed, &released );

	handleMovement( sprite, current, pressed, released );
	handleShoot( sprite, current, pressed, released );

}

static void createPlayer() {

	const char *outErr = NULL;

	playerImg = pd->graphics->loadBitmap( playerImgPath, &outErr );
	if ( outErr != NULL ) {
		pd->system->logToConsole( "Error loading image at path '%s': %s", playerImgPath, outErr );
	}

	int w, h;
	pd->graphics->getBitmapData( playerImg, &w, &h, NULL, NULL, NULL );

	player = pd->sprite->newSprite();
	pd->sprite->setImage( player, playerImg, kBitmapUnflipped );

	pd->sprite->moveTo( player, ( LCD_COLUMNS / 2 ), ( LCD_ROWS / 2 ) );
	pd->sprite->setZIndex( player, 1000 );
	pd->sprite->addSprite( player );
	pd->sprite->setUpdateFunction( player, playerUpdate );

}

// eventHandler is where the Playdate hooks into the program. Think of it like 'main' in
// a traditional C program
int eventHandler( PlaydateAPI* p, PDSystemEvent event, uint32_t arg ) {

	(void)arg; // arg is currently only used for event = kEventKeyPressed

	if ( event == kEventInit ) {

		pd = p;
		const char* err;
		font = pd->graphics->loadFont( fontpath, &err );
		
		if ( font == NULL )
			pd->system->error( "%s:%i Couldn't load font %s: %s", __FILE__, __LINE__, fontpath, err );

		createPlayer();

		// Note: If you set an update callback in the kEventInit handler, the system
		// assumes the game is pure C and doesn't run any Lua code in the game
		p->system->setUpdateCallback( update, p );

	}
	
	return 0;
}

#define TEXT_WIDTH 86
#define TEXT_HEIGHT 16

int x = ( LCD_COLUMNS - TEXT_WIDTH ) / 2;
int y = ( LCD_ROWS - TEXT_HEIGHT ) / 2;

// Main game loop
static int update( void* userdata ) {

	PlaydateAPI* pd = userdata;
	pd->graphics->clear( kColorWhite );

	pd->sprite->updateAndDrawSprites();

	pd->graphics->setFont( font );

	if( shot == 1 ) {
		wordX += WORD_SPEED;
		pd->graphics->drawText( words[wordPtr], strlen( words[wordPtr] ), kASCIIEncoding, wordX, wordY );

		if( wordX > LCD_COLUMNS ) {
			shot = 0;
			wordPtr = wordPtr == 0 ? 1 : 0;
			wordX = 0;
			wordY = 0;
		}
	}

	pd->system->drawFPS(0,0);

	return 1;

}

