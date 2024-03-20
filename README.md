# Prismatic Engine - By Unicorner Games

## Disclaimer

I've chosen to open source this project in case others find it useful, and my intention is to use the engine for my own Production games. That said, this engine is still a work in progress, there may be issues and they will be addressed as they arise, but I am a solo dev making games in my spare time. 

Use at your own discretion. 

## Philosophy

Prismatic Engine is a simple game Engine for Panic's Playdate, built with their C API. 

The philosophy is to provide a boilerplate project that simplifies some of the crunchier aspects of C development on the Playdate and allows developers to start prototyping quickly. It aims to provide a layer between the developer and the Playdate API that allows them to focus on building the game and less on memory and state management.

Prismatic Engine has been designed with flexibility in mind. The Engine includes a Scene and Sprite Management system which handles its own memory management for faster development, but does not force you to use any of its internal tools. All of the Prismatic types also expose the underlying raw Playdate API types in order to take advantage of the work already done by the Playdate team.

The engine is built directly into the project, but separated into its own `prismatic` directory. This allows you to examine and/or modify core functionality as needed without getting in your way if you don't care to deal with the specifics.

---

## Architecture

### Game & Engine Files

All core game files are located in the `src` directory. In a new project, you will see a `core` directory, a `prismatic` directory, and a `main.c` file.

#### core

The `src/core` directory is where you will add your game files. By default this contains `game.c` and `game.h`. These files are required to hook your game into Prismatic engine. `game.c` is where you will begin building your game. `game.h` exposes the `newGame` function for the engine to use during bootstrapping.

##### game.c

There are 4 functions defined in `game.c` and they are where you will put your game logic. A simple game could be completely contained in `game.c`, a more complex one might be broken into multiple files, but ultimately the main game loop will always be inside of this file.

- **init**: Use init to hook into the Engine's initialization function. This runs before the first call to update but after the engine has initialized itself

- **update**: update is the main game loop.

	- update has one param `float delta` - This is the amount of time, in seconds, since the last update.

- **draw**: draw should be used to handle screen drawing operations. It is called after update. 

	- draw has one param `float delta` - This is the amount of time, in seconds, since the last draw.

- **destroy**: destroy is called when the game is shut down, before the game itself is freed from memory. If you have any memory to free at shutdown, this is likely the place to do it.

#### main.c

This is the file responsible for hooking your game into the Playdate. Unless you intend to add a new System Event functionality or otherwise manipulate the engine bootstrapping or teardown process, you should not need to modify anything here.

#### prismatic

This directory contains the Prismatic Engine internals. Unless you intend to change or extend the engine, you should not need to modify anything here.

### Assets 

As with other Playdate C projects, you will find a `Source` directory at the root. Inside `Source` is `assets`, this is where you should place sound, images, and any other assets that you intend to load in your project.

### Playdate API Aliases

Aliases for the Playdate API are also provided for convenience, but they are not required for use. 

`pd`: `playdate`

`graphics`: `playdate->graphics`

`sound`: `playdate->sound`

`sprites`: `playdate->sprite`

`sys`: `playdate->system`


### Prismatic Engine Utilities

The engine provides some utilities for managing its own types. A brief overview is provided here, see [Type Documentation](#type-documentation) for more specific details and examples. 

#### prismaticStateMachine

Provides an interface for the creation and management of state machines. 

```C
// Create a new State Machine
// 
// ---
// 
// State* defaultState
StateMachine* ( *new )( State* );

// Delete a State Machine
// 
// ---
// 
// StateMachine* stateMachine
void ( *delete )( StateMachine* );

// Update the State Machine
// 
// Generally you will want to call this from inside of the game's main
// update loop.
// 
// ---
// 
// StateMachine* stateMachine
// 
// float delta
void ( *update )( StateMachine*, float );

// Change the State Machine's current state to the named State
// 
// ---
// 
// StateMachine* stateMachine
// 
// string stateName
State* ( *changeStateByName )( StateMachine*, char* );

// Change the State Machine's current state to the provided State
// 
// ---
// 
// StateMachine* stateMachine
// 
// State* state
State* ( *changeState )( StateMachine*, State* );

// Change the State Machine's current State to its previous State
// 
// ---
// 
// StateMachine* stateMachine
State* ( *changeToPrevious )( StateMachine* );

// Change the State Machine's current State to its default State
// 
// ---
// 
// StateMachine* stateMachine
State* ( *changeToDefault )( StateMachine* );

// Add a State to the State Machine
// 
// ---
// 
// StateMachine* stateMachine
// 
// State* state
void ( *addState )( StateMachine*, State* );
```

##### Usage

```C
// Create a State Machine where State* state is the default, then add State* state2 to the State Machine
StateMachine* stateMachine = prismaticStateMachine->new( state );
prismaticStateMachine->addState( stateMachine, state2 );
```

#### prismaticState

Provides an interface for creating and deleting States

```C
// Create a new State
// 
// ---
// 
// string stateName - A unique identifier for this State
State* (*new)( const char* );

// Delete a State
// 
// ---
// 
// State* state
void (*delete)( State* );
```

##### Usage

```C
// Create a State named "State 1"
State* state1 = prismaticState->new( "State 1" );
```

#### prismaticSprite

Provides an interface for creating, managing, and deleting Sprites

```C
// Create a new Sprite from the given paths
// 
// ----
// 
// string* paths
// 
// size_t pathCount - The length of paths
PrismSprite* ( *newFromPath )( string*, size_t );

// Create a new Sprite from an array of LCDBitmap*s
// 
// The list of images will be converted into frames for the Sprite's 
// underlying PrismAnimation*, accessible via sprite->animation.
// 
// Caller is responsible for freeing frames with prismaticSprite->freeImages
// when Sprite is destroyed.
// 
// ----
// 
// LCDBitmap** frames
// 
// size_t startFrame - The frame to start the Animation from
// 
// float playSpeed - The number of seconds elapsed between frame changes
PrismSprite* ( *newFromImages )( LCDBitmap**, size_t, float );

// Delete a Sprite
// 
// First, calls sprite->destroy(), if it exists.
// 
// Then, removes the image from the underlying
// LCDSprite*. Then, removes the LCDSprite from the screen and calls sprites->freeSprite.
// 
// Then, calls prismaticAnimation->delete() on the Sprite's PrismAnimation, if one exists.
// 
// Then, frees the initialization image, if one exists.
// 
// Finally, calls sys->realloc on the Sprite itself.
// 
// ----
// 
// PrismSprite* sprite
void ( *delete )( PrismSprite* );

// Free an array of LCDBitmap*s used to create a Sprite.
// 
// Generally, this should be called during Sprite or Scene destroy actions.
// If you choose to implement a game without Scenes, this should be called 
// when the game is destroyed.
// 
// ----
// 
// LCDBitmap** images The array of images to free
void ( *freeImages )( LCDBitmap** );

// Load images into an array of LCDBitmap*s
// 
// Caller is responsible for freeing the resulting images
// 
// ----
// 
// string* paths - Paths to images on the file system. Do not include extension.
// 
// size_t pathCount - The legnth of the paths array
LCDBitmap** ( *loadImages )( string*, size_t );

// Set the PrismAnimation* for a PrismSprite* manually
// 
// ----
// 
// PrismSprite* sprite
// 
// PrismAnimation* animation
void ( *setAnimation )( PrismSprite*, PrismAnimation* );
```

##### Usage

```C
string* paths[1] = { "assets/images/my_sprite" };
PrismSprite* sprite = prismaticSprite->newFromPath( paths, 1 );
```

#### prismaticAnimation

Provides an interface for creating, managing, and deleting Animations

```C
// Create a new PrismAnimation*
// 
// ----
// 
// LCDBitmap** frames
// 
// size_t startFrame - The frame to start the Animation from
// 
// float playSpeed - The number of seconds elapsed between frame changes
PrismAnimation* ( *new )( LCDBitmap**, size_t, float );

// Delete a PrismAnimation*
// 
// Frees only the animation, frame images should be freed separately.
// 
// ----
// 
// PrismAnimation* animation
void ( *delete )( PrismAnimation* );

// Play an Animation
// 
// ----
// 
// PrismAnimation* animation
// 
// float delta
void ( *play )( PrismAnimation*, float );

// Play an Animation in a specific order
// 
// ----
// 
// PrismAnimation* animation
// 
// size_t* frameOrder - An array of frame indexes, in the order they should be played. e.g.: { 0, 3, 1 }
// 
// size_t frameCount - The length of the frameOrder array
void ( *playInOrder )( PrismAnimation*, float, size_t*, size_t );
```

##### Usage

```C
// Create an animation from LCDBitmap* array, start on frame 0, and play each frame with an interval of 0.25 seconds
PrismAnimation* animation = prismaticAnimation->new( images, 0, 0.25f );
```

#### prismaticScene

Provides an interface for creating, managing, and deleting Scenes

```C
	// Creates a new Scene with the given name
	// 
	// ----
	// 
	// string name - A unique identifier for the Scene in the SceneManager
	Scene* ( *new )( string );

	// Deletes the Scene
	// 
	// ----
	// 
	// Scene* scene
	void ( *delete )( Scene* );

	// Add the Sprite to the Scene. 
	// 
	// ----
	// 
	// Scene* scene
	// 
	// string spriteId - A unique identifier for sprite
	// 
	// PrismSprite* sprite
	void ( *add )( struct Scene*, string, PrismSprite* );

	// Remove the Sprite from the Scene
	// 
	// ----
	// 
	// Scene* scene 
	// 
	// PrismSprite* sprite
	void ( *remove )( struct Scene*, PrismSprite* );

	// Get a Sprite from the Scene by its string ID
	// 
	// ----
	// 
	// Scene* scene
	// 
	// string spriteId
	PrismSprite* ( *get )( struct Scene*, string );
```

##### Usage

```C
// Create a Scene named Scene 1, then add PrismSprite* sprite to the scene and identify it as "Player"
Scene* scene = prismaticScene->new( 'Scene 1' );
prismaticScene->add( scene, "Player", sprite );
```

#### prismaticSceneManager

Provides an interface for creating, managing, and deleting Scene Managers. 

A Scene Manager is much like a State Machine that is specific to Scenes. 

```C
// Creates a new SceneManager
// 
// ----
// 
// Scene defaultScene - The Default Scene for the SceneManager
SceneManager* (*new)( Scene* );

// Deletes a SceneManager. 
// 
// Calls prismaticScene->delete() for each Scene in the SceneManager. 
// Then frees sceneManager->scenes. Finally, frees the SceneManager
// itself.
// 
// ----
// 
// SceneManager* sceneManager
void ( *delete )( SceneManager* );

// Updates the SceneManager. 
// 
// First, calls scene->update() for the SceneManager's current Scene. 
// Then, calls sprite->update() for each Sprite in the Scene.
// 
// ----
// 
// SceneManager* sceneManager
// float delta
void ( *update )( SceneManager*, float );

// Draws the SceneManager. 
// 
// Calls scene->draw() for the SceneManager's current Scene.
// 
// ----
// 
// SceneManager* sceneManager
// float delta
void ( *draw )( SceneManager*, float );

// Change the current Scene to the named Scene.
// 
// ----
// 
// SceneManager* sceneManager
// 
// string sceneName - The name of the Scene to change to
Scene* ( *changeSceneByName )( SceneManager*, string );

// Change the current Scene to the given Scene.
// 
// ----
// 
// SceneManager* sceneManager
// 
// Scene* newScene
Scene* ( *changeScene )( SceneManager*, Scene* );

// Change the current Scene to the previous Scene.
// 
// SceneManager* sceneManager
Scene* ( *changeToPrevious )( SceneManager* );

// Change the current Scene to the default Scene.
// 
// ----
// 
// SceneManager* sceneManager
Scene* ( *changeToDefault )( SceneManager* );

// Add a Scene to the SceneManager
// 
// ----
// 
// SceneManager* sceneManager
// 
// Scene* scene
void ( *add )( SceneManager*, Scene* );

// Get a Scene by its name from the SceneManager
// 
// ----
// 
// SceneManager* sceneManager
// 
// string sceneName - The Scene's name
Scene* ( *get )( SceneManager*, string );
```

##### Usage

```C
// Create a new Scene Manager where Scene* scene1 is the default, then add Scene* scene2 to the Manager
// Don't forget to call prismaticSceneManager->update() in game.c's update function
SceneManager* sm = prismaticSceneManager->new( scene1 );
prismaticSceneManager->add( sm, scene2 );
```

#### prismaticTransition

Provides an interface for creating, managing, and deleting Transitions.

Transitions are effects that can be performed on any static image.

```C
// Create a new PrismTransition
// 
// LCDBitmap* image - The image to transition. Caller is responsible for freeing this bitmap
// 
// int x - The x position to draw the transition
//
// int y - The y position to draw the transition
//
// float speed - The speed that the transition should play at, in seconds
//
// int type - A PrismTransitionType
PrismTransition* ( *new )( LCDBitmap*, int, int, float, int );

// Delete a PrismTransition
// 
// PrismTransition* transition
void ( *delete )( PrismTransition* );

// Play a PrismTransition
// 
// PrismTransition* transition
// 
// float delta
void ( *play )( PrismTransition*, float );

// Complete a PrismTransition
// 
// You will need to implement your own completion state, and call this when
// you implement a custom transition update function
// 
// PrismTransition* transition
void ( *complete )( PrismTransition* );
```

##### Usage

```C
// Create a new left-to-right out Transition from LCDBitmap* image, positioned at (0,0), with a play speed of 0.25 seconds 
PrismTransition* transition = prismaticTransition->new( image, 0, 0, 0.25f, PrismTransitionType_LTROut );
```

#### prismaticUtils

Provides some general purpose utility functions

```C
// Linear interpolation implementation
// 
// float start 
// 
// float end
// 
// float time
float ( *lerp )( float, float, float );

// Implementation of pow, for use with 8-bit unsigned ints - Helps with math
// for transition implementations
// 
// Caution: This function does not internally protect against overflow 
// or underflow
// 
// uint8_t base
// 
// uint8_t exponent
uint8_t ( *uint8_pow )( uint8_t, uint8_t );
```

#### prismaticString

Provides some utility functions for working with strings

```C
// Create a new string
// 
// ---
// 
// string str
string ( *new )( string );

// Delete a string
// 
// ---
// 
// string str
void ( *delete )( string );

// Check if two strings are equal
// 
// ---
// 
// string str1
// 
// string str2
bool ( *equals )( string, string );
```

#### prismaticLogger

Provides a thin wrapper around Playdate's internal `logToConsole` and `error` functions. 

Includes current datetimestamp and INFO, DEBUG, ERROR prefixes.

```C
// Log an informative message to the Console
// 
// ----
// 
// const char* msg
void ( *info )( const char* );

// Log a debug message to the Console
// 
// ----
// 
// const char* msg
void ( *debug )( const char* );

// Log an error message to the Console
// 
// ----
// 
// const char* msg
void ( *error )( const char* );

// Log an informative message to the Console with printf-style formatting
// 
// ----
// 
// const char* msg
void ( *infof )( const char*, ... );

// Log a debug message to the Console with printf-style formatting
// 
// ----
// 
// const char* msg
void ( *debugf )( const char*, ... );

// Log an error message to the Console with printf-style formatting
// 
// ----
// 
// const char* msg
void ( *errorf )( const char*, ... );
```

---

## Creating a Game

The simplest version of a game created with Prismatic Engine can be fully contained in `src/core/game.c`

**game.c**

```C
static void init() {
	prismaticLogger->info( "init" ); // Print "init" to the console
}

static void update( float delta ) {
	sprites->updateAndDrawSprites();
}

static void draw( float delta ) {
	graphics->drawText( "Hello, World", 12, kUTF8Encoding, 100, 100 ); // Draw "Hello, World at (100, 100)"
}

static void destroy() {
	prismaticLogger->info( "destroy" ); // Print "destroy" to the console
}
```

---

In a more complex example: We can create two Scenes, add them to a Scene Manager, and create a transition from one to the other. This is the demo that the project starts with.

All scenes have been combined into a single file `customscenes.c` for documentation purposes, but you could break each Scene's logic into its own file.

**game.c**
```C
#include <stdlib.h>
#include <stdio.h>

#include "customscenes.h"
#include "../prismatic/prismatic.h"

static void init( void );
static void update( float );
static void draw( float );
static void destroy( void );

static SceneManager* sm;

// Use init to hook into the Engine's initialization function. This runs 
// before the first call to update but after the engine has initialized itself
static void init() {

	prismaticLogger->info( "init" );
	sm = initScenes(); // Create the Scene Manager

}

// update is your game's entry point to the engine
static void update( float delta ) {
	sprites->updateAndDrawSprites();
	sys->drawFPS( 0, 0 );
	prismaticSceneManager->update( sm, delta ); // Make sure our Scene Manager updates
}

// draw should be used to handle screen drawing operations. It is called after 
// update
static void draw( float delta ) {
	// Nothing to do here
}

// destroy is called when the game is shut down, before the game itself is
// freed from memory
static void destroy() {
	prismaticLogger->info( "destroy" );

	// Destroy the Scene Manager during game teardown - The SceneManager will handle freeing memory for its Scenes
	// and, in turn the Scenes will handle freeing memory for their Sprites
	prismaticSceneManager->delete( sm ); 
}
```

**customscenes.h**

```C
#ifndef TEST_H
#define TEST_H

#ifndef PRISMATIC_INCLUDED
	#define PRISMATIC_INCLUDED
	#include "../prismatic/prismatic.h"
#endif

SceneManager* initScenes( void );

#endif
```

**customscenes.c**

```C
#include "customscenes.h"
#include "../prismatic/prismatic.h"

static LCDBitmap** images;
static float elapsed = 0.0f;
static LCDBitmap* bg;
static bool bgGot = false;

static SceneManager* sm;
static PrismTransition* transition;

static void spr1_update( PrismSprite* self, float delta );
static void spr2_update( PrismSprite* self, float delta );
static void transition_complete( PrismTransition* self );
static void sc1_enter( Scene* self );
static void sc2_enter( Scene* self );
static void sc1_update( Scene* self, float delta );
static void sc_draw( Scene* self, float delta );
static void sm_destroy( SceneManager* sceneManager );

//////////////
// Sprite 1 //
//////////////
static void spr1_update( PrismSprite* self, float delta ) {
	prismaticAnimation->play( self->animation, delta ); // Play Sprite 1's animation every time we run sprite->update()
}

//////////////
// Sprite 2 //
//////////////
static void spr2_update( PrismSprite* self, float delta ) {
	sprites->moveBy( self->sprite, 1, 1 ); // Move Sprite 2 by 1px every time we run sprite->update()
}

////////////////
// Transition //
////////////////
static void transition_complete( PrismTransition* self ) {
	prismaticSceneManager->changeSceneByName( sm, "Scene 2" ); // Change the current Scene to Scene 2
}


/////////////
// Scene 1 //
/////////////
static void sc1_enter( Scene* self ) {
	PrismSprite* s = prismaticScene->get( self, "spr1" ); // Get Sprite 1 from the Scene
	sprites->moveTo( s->sprite, 50, 50 ); // Set Sprite 1's position to 50, 50

	transition = prismaticTransition->new( NULL, 0, 0, 0.05f, PrismTransitionType_ShrinkToCenter ); // Initialize a new ShrinkToCenter Transition that animates in intervals of 0.05 seconds
	transition->complete = transition_complete; // Set the Transition's complete method to transition_complete
}

static void sc1_update( Scene* self, float delta ) {

	elapsed += delta; // Keep track of total seconds elapsed with each frame
	if( elapsed >= 5.0f && !bgGot ) {  // When we hit 5 seconds for the first time
		bg = graphics->copyBitmap( graphics->getDisplayBufferBitmap() ); // Take a snapshot of the current frame and copy it into bg

		PrismSprite* sp1 = prismaticScene->get( self, "spr1" ); // Get Sprite 1 from the Scene
		prismaticScene->remove( self, sp1 ); // Remove Sprite 1 from the Scene
		transition->image = bg; // Set the Transition's image to our snapshot

		bgGot = true; // Flag that we've been here already
	}

	if( bgGot ) {
		prismaticTransition->play( transition, delta ); // Play the transition
	}

}


static void sc_draw( Scene* self, float delta ) {
	// Nothing to do...
}


/////////////
// Scene 2 //
/////////////
static void sc2_enter( Scene* self ) {}

static void sc2_update( Scene* self, float delta ) {
	prismaticLogger->info( "Scene 2" );
}

///////////////////
// Scene Manager //
///////////////////
static void sm_destroy( SceneManager* sceneManager ) {
	prismaticTransition->delete( transition ); // Destroy the Transition and free its internals
	graphics->freeBitmap( bg ); // Explicitly free the bg snapshot that we took in Scene 1
	prismaticSprite->freeImages( images ); // Explicitly free the images array that we used to create Sprite 1
}

//////////////////////////////////////////
// Set up the Scenes - Called from game //
//////////////////////////////////////////
SceneManager* initScenes() {

	PrismSprite* s;
	PrismSprite* s2;

	Scene* sc1;
	Scene* sc2;

	/////////////////////////////////
	// Load Bitmaps from path list //
	/////////////////////////////////
	string paths[4] = { 
	    "assets/images/entities/test/anim-1",
	    "assets/images/entities/test/anim-2",
	    "assets/images/entities/test/anim-3",
	    "assets/images/entities/test/anim-4",
	};

	images = prismaticSprite->loadImages( paths, 4 );


	///////////////////////////////////////////
	// Create Sprite 1 from loaded images // //
	///////////////////////////////////////////
	s = prismaticSprite->newFromImages( images, 0, 0.10f );
	s->update = spr1_update; // Set Sprite 1's update method


	////////////////////////////////////////
	// Create Sprite 2 directly from path //
	////////////////////////////////////////
	string paths2[1] = { "assets/images/entities/player/player" };
	s2 = prismaticSprite->newFromPath( paths2, 1 );
	s2->update = spr2_update; // Set Sprite 2's update method


	////////////////////
	// Create Scene 1 //
	////////////////////
	sc1 = prismaticScene->new( "Scene 1" );

	///////////////////////////
	// Set Scene 1's methods //
	///////////////////////////
	sc1->enter = sc1_enter;
	sc1->update = sc1_update;
	sc1->draw = sc_draw;


	////////////////////
	// Create Scene 2 //
	////////////////////
	sc2 = prismaticScene->new( "Scene 2" );

	///////////////////////////
	// Set Scene 2's methods //
	///////////////////////////
	sc2->enter = sc2_enter;
	sc2->update = sc2_update;
	sc2->draw = sc_draw;


	/////////////////////////////
	// Add Sprite 1 to Scene 1 //
	/////////////////////////////
	prismaticScene->add( sc1, "spr1", s );


	/////////////////////////////
	// Add Sprite 2 to Scene 2 //
	/////////////////////////////
	prismaticScene->add( sc2, "spr2", s2 );


	///////////////////////////////////////////////////////
	// Create the Scene Manager, with Scene 1 as default //
	///////////////////////////////////////////////////////
	sm = prismaticSceneManager->new( sc1 );
	sm->destroy = sm_destroy; // Set the Scene Manager's destroy method


	///////////////////////////////////////
	// Add Scene 2 to the Scene Manager  //
	///////////////////////////////////////
	prismaticSceneManager->add( sm, sc2 );

	return sm;

}
```

---

## Building / Running a Game

**Disclaimer**: The build process is configured for Linux using make, cmake, and gcc. Make sure to update accordingly if running elsewhere.

Make sure that you have first set up the Playdate SDK and have set `$PLAYDATE_SDK_PATH`.

Optionally rename the `prismatic-game` directory

Find all instances of `~/Projects/Playdate/prismatic-game/` that appear in the project directory, and replace them with the path to your project.

```bash
cd /path/to/project/
find ./ -type f -exec sed -i 's#~/Projects/Playdate/prismatic-game/#/path/to/project/#g' {} +
`

### Running on the Simulator

Run `./build-run.sh` 

After cmake output, the simulator should launch with the latest compiled version of your game.

---

### Building for the Device

Run `./build.sh`

You'll see a `build` directory should be created. In that directory you will see a `prismatic-game.pdx.zip` file after a successful build.

**Alternative Method**: You can run your game on the simulator and use the "Upload to Device" option there to sideload your game for quick testing.

---

## Type Documentation

TODO