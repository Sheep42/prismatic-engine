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

## Building / Running a Game

**Note:** You won't be able to build the game on a different platform than you are trying to run on. You will run into errors in the simulator if, for example, you build on Linux and run on Windows. This means WSL won't be an effective workaround. This does not apply to building for device.

### Linux / Mac - Recommended

Make sure that you have first set up the Playdate SDK and have set `$PLAYDATE_SDK_PATH`. You'll also need to make sure that you've installed `make`, `cmake`, and the `arm-none-eabi-newlib` packages via your package manager. It is also not a bad idea to install `build-essential`, `base-devel`, or your OS' equivalent essential C/C++ build tools before attempting compilation.

Optionally rename the `prismatic-game` directory.

Find all instances of `~/Projects/Playdate/prismatic-game/` that appear in the project directory, and replace them with the path to your project.

```bash
cd /path/to/project/
find ./ -type f -exec sed -i 's#~/Projects/Playdate/prismatic-game/#/path/to/project/#g' {} +
```

---

#### Running on the Simulator

**Note**: Before the build can run, the Playdate Simulator needs to be linked as `/usr/bin/pdsim` or `/usr/local/bin/pdsim`, or an alias `pdsim` needs to be created in your environment.

Run `./build-run.sh` 

After cmake output, the simulator should launch with the latest compiled version of your game.

---

#### Building for the Device

Run `./build.sh`

You'll see a `build` directory should be created. In that directory you will see a `prismatic-game.pdx.zip` file after a successful build.

**Alternative Method**: You can run your game on the simulator and use the "Upload to Device" option there to sideload your game for quick testing.

---

### Windows

**Note:** To build for Windows you will need to install Visual Studio and the Visual C++ dependencies.

- Install Visual Studio
- Install All C++ Development Workloads
- Install MSVC, C++ Redist, C++ CMake Tools, C++ Clang Compiler, and Windows 10 SDK components
- Install Cmake for Windows & make sure the cmake executable is added to your PATH
- Replace all occurrences of `~/Projects/Playdate/prismatic-game/` with `C:/path/to/project/`
- Replace all Occurrences of `C:\PlaydateSDK` with the path to your PlaydateSDK installation, and `C:\prismatic-game` and `C:/prismatic-game` with the path to your project directory (keeping forward or backslashes intact) in ALL_BUILD.vcxproj, ALL_BUILD.vcxproj.filters, ZERO_CHECK.vcxproj, and ZERO_CHECK.vcxproj.filters.
- Click **Build->Clean ALL_BUILDS**
- Click **Build->Rebuild ALL_BUILDS**

#### Running on the Simulator

- Open the project folder in VS
- Press Ctrl-B to build the package
- Run the Playdate Simulator
- Click **File->Open**
- Select the `prismatic-game.pdx` folder

#### Building for the Device

- Open the project folder in VS
- Press Ctrl-B to build the package
- Navigate to the `prismatic-game.pdx` folder in the Project root
- Right click, and select "Compress to Zip"
- Sideload the zip file onto the device

**Alternative Method**: You can run your game on the simulator and use the "Upload to Device" option there to sideload your game for quick testing.

---

## Engine Architecture

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
// 
// float playSpeed - The number of seconds elapsed between frame changes
PrismSprite* ( *newFromPath )( string*, size_t, float );

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
PrismSprite* sprite = prismaticSprite->newFromPath( paths, 1, 0 );
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

	// Get a Sprite from the Scene by its LCDSprite
	// 
	// ----
	// 
	// Scene* scene
	// 
	// LCDSprite* sprite
	PrismSprite* ( *getByLCDSprite )( struct Scene*, LCDSprite* sprite );
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

// Generate a random float between min and max
// 
// Caution: This function will only work properly if rand has been
// seeded using srand
// 
// float min
// 
// float max
float (*randBetween)( float min, float max );
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

// Check if a string contains another string
//
// ----
//
// string needle - The substring to search for
//
// string haystack - The string to search through
bool ( *contains )( string, string );

// Trim the trailing character of a string, if it exists
//
// Note that target is duplicated and the trimmed string is returned
//
// ----
//
// string target - The string to trim the trailing character from
//
// char ch - The character to trim
string ( *trimLast )( string, char );

// Concatenate a string with another string
//
// Both target and src should be pointers, so the caller is responsible for
// freeing them.
//
// ---- 
//
// string* target - The string that src should be added to 
//
// string src - The string that should be concatenated to target
void ( *concat )( string*, string );
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

#### prismaticTileMap

Provides an interface for importing and working with LDtk Tile Map exports. 

```C
// Create a new LDtkTileMap from the path
//
// The path should point to an LDtk simple export directory 
// containing all of its assets 
//
// ----
// 
// string path - The path to the directory containing the LDtk Level export 
//
// int tileSize - The size, in pixels, of a single tile 
//
// string* collistionLayers - A list of csv files containing collision
// information. Pass NULL for no collision layer. Must be NULL terminated 
LDtkTileMap* ( *new )( string, int, string* );

// Delete the LDtkTileMap
//
// ----
//
// LDtkTileMap* map
void ( *delete )( LDtkTileMap* );

// Draw the map's layers to screen as raw LCDBitmap*s 
//
// This is a convenience function, you may choose to draw the map yourself
// if doing so is preferable.
//
// ---
//
// LDtkTileMap* map
void ( *draw )( LDtkTileMap* );

// Draw the map's layers to screen as Sprites
//
// This is a convenience function, you may choose to draw the map yourself
// if doing so is preferable.
//
// ---
//
// LDtkTileMap* map
void ( *add )( LDtkTileMap* );

// Remove the map's layers from the screen.
//
// This is a convenience function, you may choose to draw the map yourself
// if doing so is preferable.
//
// ---
//
// LDtkTileMap* map
void ( *remove )( LDtkTileMap* );

// Add the map's collision layers to the screen.
//
// This is a convenience function, you may choose to manage the collision 
// yourself if doing so is preferable.
//
// ---
//
// LDtkTileMap* map
void ( *addCollision )( LDtkTileMap* );

// Remove the map's collision layers from the screen.
//
// This is a convenience function, you may choose to manage the collision 
// yourself if doing so is preferable.
//
// ---
//
// LDtkTileMap* map
void ( *removeCollision )( LDtkTileMap* );

// Tag a the Sprites in a map's collision layer
//
// ---
//
// LDtkTileMap* map
//
// string layerName
//
// uint8_t tag 
void ( *tagCollision )( LDtkTileMap*, string, uint8_t );
```

#### prismaticMapManager

```C
// Create a new MapManager
LDtkMapManager* ( *new )( void );

// Delete a MapManager
//
// Does not free the MapManager's maps, it is up to the caller to call 
// prismaticTileMap->delete on their maps.
// ---
//
// LDtkMapManager* mapManager
void ( *delete )( LDtkMapManager* );

// Add a map to the Map Manager, if it is not already added.
//
// ----
// 
// LDtkMapManager* mapManager
//
// LDtkTileMap* map
void ( *add )( LDtkMapManager*, LDtkTileMap* );

// Remove a map from the Map Manager
// 
// Does not free the map, it is up to the caller to free the map from
// memory
//
// ----
// 
// LDtkMapManager* mapManager
//
// string id - The map's Id or Iid
void ( *remove )( LDtkMapManager*, string );

// Get a map from the Map Manager by its Iid
//
// ----
//
// LDtkMapManager* mapManager
//
// string id - The map's Id or Iid
LDtkTileMap* ( *getMap )( LDtkMapManager*, string );

// Change the current map to the map with the specified Iid
//
// ----
//
// LDtkMapManager* mapManager
//
// string iid - The map's Iid
void ( *changeMapByIid )( LDtkMapManager*, string );

// Change the current map to the map with the specified Identifier
//
// ----
//
// LDtkMapManager* mapManager
//
// string id - The map's identifier
void ( *changeMapByName )( LDtkMapManager*, string );

// Change the current map to the specified map. Adds the map to the MapManager
// if it is not already added. Can be used as a shorthand to add & set current Map.
//
// ----
//
// LDtkMapManager* mapManager
//
// LDtkTileMap* map
void ( *changeMap )( LDtkMapManager*, LDtkTileMap* );
```

---

#### dialogueController

```C
// Sets text for a Dialogue box
// 
// --- 
// 
// Dialogue* dialogue 
// 
// string text
void (*setText)( Dialogue* dialogue, string text );

// Sets a Dialogue box's position
// 
// ---
// 
// Dialogue* dialogue
// 
// float x - The dialogue box's x position
// 
// float y - The dialogue box's y position
void (*setPosition)( Dialogue* dialogue, float x, float y );

// Sets autohide value for a Dialogue box
// 
// ---
// 
// Dialogue* dialogue
// 
// bool autoHide - True: On, False: Off
// 
// float duration - How long to show the Dialogue before hiding
void (*setAutoHide)( Dialogue* dialogue, bool autoHide, float duration );

// Sets a Dialogue box width, height, and background color (LCDPatterns are supported)
// 
// ---
// 
// Dialogue* dialogue
// 
// float width
// 
// float height
// 
// LCDColor boxColor
void (*setBox)( Dialogue* dialogue, float width, float height, LCDColor boxColor );

// Sets a Dialogue box border width, height, and color (LCDPatterns are supported)
// 
// ---
// 
// Dialogue* dialogue
// 
// float width
// 
// float height
// 
// LCDCOlor borderColor
void (*setBorder)( Dialogue* dialogue, float width, float height, LCDColor borderColor );

// Sets a Dialogue box's font and text draw mode
// 
// ---
// 
// Dialogue* dialogue
// 
// LCDFont* font
// 
// LCDBitmapDrawMode drawMode
void (*setFont)( Dialogue* dialogue, LCDFont* font, LCDBitmapDrawMode drawMode );

// Sets a Dialogue box's draw offset
// 
// All Dialogue boxes are drawn to their own context which covers the entire screen.
// Setting the draw offset will draw the box and text at the box's position, relative
// to the offset x and y values, without affecting anything else on the screen.
// 
// ---
// 
// Dialogue* dialogue
// 
// float x - The x offset
// 
// float y - The y offset
void (*setDrawOffset)( Dialogue* dialogue, float x, float y );

// Sets padding around text in a Dialogue box
// 
// ---
// 
// Dialogue* dialogue
// 
// float x - The x padding (left and right)
// 
// float y - The y padding (top and bottom)
void (*setTextPadding)( Dialogue* dialogue, float x, float y );

// Set a Dialogue box's text speed
// 
// ---
// 
// Dialogue* dialogue
// 
// float speed - Speed is the amount of seconds between character advancement. 
// Default: 0.0f 
void (*setSpeed)( Dialogue* dialogue, float speed );

// Sets a Dialogue box's active script
// 
// ---
// 
// Dialogue* dialogue
// 
// DialogueScript* script
void (*setScript)( Dialogue* dialogue, DialogueScript* script );

// Sets a Dialogue box's audio pitch
// 
// ---
// 
// Dialogue* dialogue
// 
// float pitch
void (*setPitch)( Dialogue* dialogue, float pitch );

// Sets a Dialogue box's text alignment
// 
// ---
// 
// Dialogue* dialogue
// 
// PDTextAlignment alignment - Default: kAlignTextLeft
void (*setAlignment)( Dialogue* dialogue, PDTextAlignment alignment );

// Sets a Dialogue box's wrap mode
// 
// ---
// 
// Dialogue* dialogue
// 
// PDTextWrappingMode wrapMode - Default: kWrapWord
void (*setWrapMode)( Dialogue* dialogue, PDTextWrappingMode wrapMode );

// Enables audio for a Dialogue box - Disabled by default
// 
// ---
// 
// Dialogue* dialogue
void (*enableSound)( Dialogue* dialogue );

// Disables audio for a Dialogue box
// 
// ---
// 
// Dialogue* dialogue
void (*disableSound)( Dialogue* dialogue );

// Sets a dialogue box's dialogue type
// 
// ---
// 
// Dialogue* dialogue
// 
// DialogueType type
void (*setType)( Dialogue* dialogue, DialogueType type );

// Advances a Dialogue box's script to the next line
// 
// ---
// 
// Dialogue* dialogue
void (*advance)( Dialogue* dialogue );

// Shows a Dialogue box
// 
// ---
// 
// Dialogue* dialogue
void (*show)( Dialogue* dialogue );

// Hides a Dialogue box
// 
// ---
// 
// Dialogue* dialogue
void (*hide)( Dialogue* dialogue );

// Flags a Dialogue box as finished
// 
// ---
//
// Dialogue* dialogue
void (*finish)( Dialogue* dialogue );

// Draws a Dialogue box
//
// Generally, should be called from inside of a Scene's or a Game's draw method
//  
// ---
// 
// Dialogue* self
// 
// float delta
void (*draw)( struct Dialogue* self, float delta );

// Updates a Dialogue box
//
// Generally, should be called from inside of a Scene's or a Game's update method
//  
// ---
// 
// Dialogue* self
// 
// float delta
void (*update)( struct Dialogue* self, float delta );

// Creates a new Dialogue box
// 
// ---
// 
// float x - The Dialogue box's starting X position
// 
// float y - The Dialogue box's starting Y position
Dialogue* (*new)( float x, float y );

// Destroys a Dialogue box
// 
// DOES NOT free any DialogueScript* or DialogueLine* that are currently attached 
// to the Dialogue box. It is up to the caller to delete a script and/or lines
// separately.
// 
// ---
// 
// Dialogue* self
void (*destroy)( struct Dialogue* self );
```

#### dialogueScriptManager

```C
// Creates a new Dialogue Script
// 
// ---
// 
// string id - A machine-friendly unique identifier for the script
DialogueScript* (*new)( string id );

// Adds a dialogue line to a script
// 
// --- 
// 
// DialogueScript* script
// 
// DialogueLine* line
void (*add)( DialogueScript* script, DialogueLine* line );

// Removes a dialogue line from a script
// 
// Does not free the dialogue line being removed - That is the responsibility of the caller
// 
// --- 
// 
// DialogueScript* script
// 
// DialogueLine* line
void (*remove)( DialogueScript* script, DialogueLine* line );

// Removes the dialogue line at a specific index in a script
// 
// Does not free the dialogue line being removed - That is the responsibility of the caller
// 
// --- 
// 
// DialogueScript* script
// 
// size_t index
void (*removeIndex)( DialogueScript* script, size_t index );

// Resets a dialogue script's internal pointer to 0
// 
// ---
// 
// DialogueScript* script
void (*reset)( DialogueScript* script );

// Delete's a script
// 
// Deleting a script DOES free any lines that are added to the script at the 
// time of its deletion. The caller should refrain from referencing any script
// lines after the script is deleted. If a line needs to remain active, remove 
// it from the script prior to deletion.
// 
// ---
// 
// DialogueScript* script
void (*delete)( DialogueScript* script );
```

#### dialogueLineManager

```C
// Create a new dialogue line
// 
// DialogueLineType type - The dialogue line type 
DialogueLine* (*new)( DialogueLineType type );

// Delete a dialogue line
//  
// DialogueLine* line - The dialogue line to delete
void (*delete)( DialogueLine* line );
```

#### cameraController

```C
typedef struct CameraController {
    Camera* (*newFixedCam)( float x, float y );
    Camera* (*newLerpCam)( float startX, float startY, float time );
    Camera* (*newFollowCam)( PrismSprite* target );
    Camera* (*newFollowLerpCam)( PrismSprite* target, float time );
    
    // Set origin x & y position for a Camera
    void (*setOrigin)( Camera* self, float x, float y );

    // Set current x & y position for a Camera
    void (*setPos)( Camera* self, float x, float y );
    
    // Set min bouds or a restricted Camera
    void (*setMinBounds)( Camera* self, float minX, float minY );
    
    // Set max bounds for a restricted Camera
    void (*setMaxBounds)( Camera* self, float maxX, float maxY );
    
    // Set restricted value
    void (*setRestricted)( Camera* self, bool restricted );

    // Move a Camera to a new position. Will have different effect depending on camera type:
    // 
    // ---
    // 
    // Fixed Cam - Same as setPos( x, y )
    // 
    // Lerp Cam - Will move from current (x, y) to new (x, y)
    // 
    // Follow & Follow Lerp Cams - no effect
    // 
    // ---
    // 
    // Camera* self - The Camera
    // 
    // float newX
    // 
    // float newY
    void (*moveTo)( Camera* self, float newX, float newY );

    // Push a Dialogue box into this camera, so that the Dialogue box is
    // rendered within the camera's active space
    // 
    // Camera* self - The Camera
    // 
    // Dialogue* dialogue - The Dialogue box
    void (*pushDialogue)( Camera* self, Dialogue* dialogue );

    // Remove a Dialogue box that has been pushed to this camera
    // 
    // NOTE: This DOES NOT free the Dialogue, that must be done separately
    // 
    // Camera* self - The Camera
    // 
    // Dialogue* dialogue - The Dialogue box
    void (*removeDialogue)( Camera* self, Dialogue* dialogue );
    
    // The Camera's update function
    // 
    // ---
    // 
    // Camera* self - The Camera
    // 
    // float delta
    void (*update)( Camera* self, float delta );

    // Destroy a Camera
    // 
    // // NOTE: This DOES NOT free Dialogues attached to the Camera, that must be done separately
    void (*destroy)( Camera* self );
} CameraController;
```

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

In a more complex example: We can create two Scenes, add them to a Scene Manager, and create a transition from one to the other.

All example scenes have been combined into a single file `customscenes.c` for documentation purposes, but you could break each Scene's logic into its own file.

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
	s2 = prismaticSprite->newFromPath( paths2, 1, 0 );
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

The demo project that ships with the engine contains a more realistic example, consisting of a SplashScene, TitleScene, and PlayScene, which have all of their logic broken out into individual source files.

This is intended to act as a boilerplate starter project, but none of the included scenes are required.

The .c source files included in `core/scenes` are where you will find the logic for each scene. 

---

## Type Documentation

### States & StateMachines

`State`s are members of a `StateMachine`, in charge of handling specific game logic. 

#### State

**Type Name**: `State`

- `const char* name`: A unique identifier for the State

- `void (*enter)( void )`: The function that is fired when entering the State

- `void (*exit)( void )`: The function that is fired when exiting the State

- `void (*tick)( float )`: The function that is fired continuously while the State is active

	- **Param**: `float delta` - The time, in seconds, since the last tick

#### StateMachine

**Type Name**: `StateMachine`

- `State** states`: The array of `State`s added to the `StateMachine`

- `size_t totalStates`: The length of `stateMachine->states`

- `State* defaultState`: The default `State` for the `StateMachine`

- `State* previousState`: The previously active `State`

- `State* currentState`: The currently active `State`


**Example**:

```C
// game.c
// Define our custom functions
static void enter( void );
static void exit( void );
static void tick( float );

// Define static vars
static StateMachine* sm;
static State* s;

static void init() {
	
	// Create a State named "State 1"
	s = prismaticState->new( "State 1" );
	if( s == NULL ) {
		prismaticLogger->error( "Could not create State" );
		return;
	}

	// Assign the State's methods to our custom logic
	s->enter = enter;
	s->exit = exit;
	s->update = tick;

	// Create a StateMachine and assign the State as default
	sm = prismaticStateMachine->new( s );
	if( sm == NULL ) {
		prismaticLogger->error( "Could not create StateMachine" );
		return;
	}

}

static void update( float delta ) {

	if( sm == NULL ) {
		return;
	}

	// Update the StateMachine
	// This runs s->tick and passes along delta
	prismaticStateMachine->update( sm, delta );

}

static void destroy() {

	if( sm == NULL ) {
		return;
	}

	// Free the StateMachine
	// This will also free its States and their interal allocated memory
	prismaticStateMachine->delete( sm );

}

static void enter() {
	prismaticLogger->info( "Enter" );
}

static void exit() {
	prismaticLogger->info( "Exit" );
}

static void tick( float delta ) {
	prismaticLogger->infof( "%f", delta );
}

```

### Scenes & SceneManagers

#### Scene

**Type Name**: `Scene`

- `string name`: A unique identifier for the `Scene`

- `PrismSprite** sprites`: The array of `PrismSprite`s currently added to the `Scene`

- `size_t totalSprites`: The length of `scene->sprites`

- `bool isActive`: A flag for whether the `Scene` is active or not

- `struct SceneManager* sceneManager`: The `SceneManager` that the `Scene` belongs to

- `void* ref`: Optional - Can be used as a pointer to a custom struct for extending a Scene's properties. Caller is responsible for freeing `ref`, and any of its contents.

- `void ( *enter )( struct Scene* )`:  The function that runs when the `Scene` is entered

	- **Param**: `Scene* self` - A reference to the `Scene` for use inside the enter function

- `void ( *update )( struct Scene*, float )`: The `Scene`'s update function

	- **Param**: `Scene* self` - A reference to the `Scene` for use inside the update function
	- **Param**: `float delta` - The time, in seconds, since the last update

- `void ( *draw )( struct Scene*, float )`: The `Scene`'s draw function

	- **Param**: `Scene* self` - A reference to the `Scene` for use inside the draw function
	- **Param**: `float delta` - The time, in seconds, since the last draw

- `void ( *exit )( struct Scene* )`: The function that runs when the `Scene` is exited

	- **Param**: `Scene* self` - A reference to the `Scene` for use inside the exit function

- `void ( *destroy )( struct Scene* )`: The function that runs just before the `Scene` is destroyed

	- **Param**: `Scene* self` - A reference to the `Scene` for use inside the destroy function

#### SceneManager

**Type Name**: `SceneManager`

- `Scene** scenes`: The array of `Scene`'s currently added to the `SceneManager`

- `int totalScenes`: The length of `sceneManager->scenes`

- `Scene* defaultScene`: The default `Scene` for the `SceneManager`

- `Scene* currentScene`: The currently active `Scene`

- `Scene* previousScene`: The previously active `Scene`

- `void (*destroy)( struct SceneManager* )`: The function that runs just before the `SceneManager` is destroyed

	- **Param**: `SceneManager* self` - A reference to the `SceneManager` for use inside the destroy function

**Example**: See [demo](#creating-a-game)

### Sprites & Animations

**Type Name**: `PrismSprite`

- `string id`: A unique identifier for the `Sprite`

- `LCDSprite* sprite`: The underlying Playdate `LCDSprite*`

	- Used for movement, collision, etc

- `LCDBitmap** imgs`: For `Sprite`s with a `PrismAnimation`, the array of `LCDBitmap*`s that make up the animation

- `PrismAnimation* animation`: The `Sprite`'s animation - Will be NULL if the sprite has 1 or less images

- `Scene* scene`: Tracks the Sprite's active Scene, if it has been added to one. Will be NULL if the Sprite has not been added to, or has been removed from a Scene.

- `void* ref`: Optional - Can be used as a pointer to a custom struct for extending a Scene's properties. Caller is responsible for freeing `ref`, and any of its contents.

- `bool active`: Used to tell a Scene if a Sprite is active. If active is false, the Sprite's update function will be skipped.

- `void ( *update )( PrismSprite*, float )`: The `Sprite`'s update function

	- **Param**: `PrismSprite* self` - A reference to the `PrismSprite` for use inside the update function
	- **Param**: `float delta` - The time, in seconds, since the last update

- `void ( *destroy )( PrismSprite* )`: The function that runs just before the `Sprite` is destroyed

	- **Param**: `PrismSprite* self` - A reference to the `PrismSprite` for use inside the destroy function


**Type Name**: `PrismAnimation`

- `LCDBitmap** frames`: The array of `LCDBitmap*`s that make up the `PrismAnimation`'s frames

- `size_t frameCount`: The length of `animation->frames`

- `size_t currentFrame`: The `PrismAnimation`'s current frame index

- `size_t customOrderPtr`: Internal index used to keep track of frames when a custom play order is specified

- `float playSpeed`: The number of seconds between each frame

- `PrismSprite* sprite`: The `PrismSprite` associated with the `PrismAnimation`

- `bool looping`: Is this a looping animation? - Default: True

- `bool finished`: Is the animation finished? - Always false for looping animations

- `bool paused`: Flag used to pause the `PrismAnimation` - Default: False

- `float timer`: Internal timer used for handling frame changes

- `void ( *complete )( PrismAnimation* )`: Fires every time the animation reaches it's final frame

	- **Param**: `PrismAnimation* self` - A reference to the `PrismAnimation` for use inside the complete function


**Example**:

```C
// game.c
// Define our custom functions
static void sprite_update( float );

// Define static vars
static PrismSprite* s;

// Define custom properties
typedef struct {
	float timer;
} SpriteProps;

static void init() {
	
	// Create a new Sprite with an Animation that has a .1 second delay between each frame
	string paths[2] = { "assets/images/my_sprite_frame1", "assets/images/my_sprite_frame2" };
	s = prismaticSprite->newFromPath( paths, 2, 0.10f );

	SpriteProps* props = calloc( 1, sizeof( SpriteProps ) );
	props->timer = 0.0f;

	s->ref = props;
	
	if( s != NULL ) {
		// Set the Sprite's update function to our custom function
		s->update = sprite_update;
	}

}

static void update( float delta ) {

	// Run s->update() if Sprite and its update function are not NULL
	if( s != NULL && s->update != NULL ) {
		s->update( s, delta );
	}

}

static void destroy() {
	
	// Free the props
	SpriteProps* props = (SpriteProps*)self->ref;
	free( props );
	props = NULL;
	self->ref = NULL;

	// Free the Sprite when the game is shut down
	prismaticSprite->delete( s );

}

static void sprite_update( PrismSprite* self, float delta ) {
	// Move the Sprite by (1,1) each pass through the update loop
	sprites->moveBy( self->sprite, 1, 1 );

	SpriteProps* props = (SpriteProps*)self->ref;
	props->timer += delta;
	
	// Play Sprite's animation every time we run sprite->update()
	prismaticAnimation->play( self->animation, delta );
}

```

### Transitions

Transitions are effects that can be applied to any `LCDBitmap*`. The primary intention for transitions is to be used as transitions between scenes, but they could also be used as effects applied to Sprite images.

A transition can only be applied to a single `LCDBitmap*` image, not directly to a `PrismSprite` or a `PrismAnimation`.

The fundamental concept behind how transitions work is this: 

- Obtain a static image (i.e.: a screen snapshot)

- Position the static image at the transition's (x,y) coordinates

- Start with an initial LCDPattern state (usually fully visible or fully hidden)

- On each pass through the transition's update function, operate on the LCDPattern to change its appearance

- On each pass through the transition's draw function:

	- Turn the LCDPattern into an image and store it in `transition->mask`
	- Apply `transition->mask` to `transition->image` as a bitmap mask
	- Store the rendered result in `transition->_rendered`
	- Draw `transition->_rendered` to the screen


**Type Name**: `PrismTransition`

- `int type`: The `PrismTransitionType` for the transition

- `float speed`: The speed at which the transition should play, in seconds

- `float moveSpeed`: The number of pixels the transition should move in a frame - Used by Slide transitions. Default: 15.0

- `float elapsed`: Internal timer used to keep track of playing the transition

- `float completeElapsed`: Internal timer used to keep track of the completion delay

- `float runTime`: A running total of the time a transition has been playing

- `float completeDelay`: The amount of time, in seconds, to delay before the complete callback is called. Default 0.0

- `int x`: The x position of the transition image

- `int y`: The y position of the transition image 

- `LCDBitmapFlip flipped`: The flip value of the transition image

- `LCDPattern pattern`: The pattern used to generate the transition

- `LCDBitmap* mask`: Internal image used when converting the pattern into a bitmap mask

- `LCDBitmap* image`: The base image to apply the transition on

- `LCDBitmap* _rendered`: Internal image used to store the final render for each step of the transition

- `bool finished`: Is the transition finished?

- `uint8_t _exp1`: Internal value for storing exponents used in 8-bit math

- `uint8_t _exp2`: Internal value for storing exponents used in 8-bit math

- `bool _pass`: Generic flag that can be used to keep track of states in a transition - Used by Fade In/Fade Out Effects to set even bytes, then odd bytes

- `void ( *enter )( struct PrismTransition* )`: Function that runs when the transition is entered - If you use a pre-defined transition type, this will be set for you

	- **Param**: `PrismTransition* self` - A reference to the `PrismTransition` for use inside the enter function

- `void ( *update )( struct PrismTransition*, float )`: The transition's update function - If you use a pre-defined transition type, this will be set for you
	
	- **Param**: `PrismTransition* self` - A reference to the `PrismTransition` for use inside the update function
	- **Param**: `float delta` - The time, in seconds, since the last update

- `void ( *draw )( struct PrismTransition*, float )`: The transition's draw function - If you use a pre-defined transition type, this will be set for you

	- **Param**: `PrismTransition* self` - A reference to the `PrismTransition` for use inside the draw`function
	- **Param**: `float delta` - The time, in seconds, since the last draw

- `void ( *complete )( struct PrismTransition* )`: Fires when a transition has completed - If you use a pre-defined transition type, this will be set for you

	- **Param**: `PrismTransition* self` - A reference to the `PrismTransition` for use inside the complete function

#### Transition Type Values

- `PrismTransitionType_LTRIn`: Left-to-right draw in effect

- `PrismTransitionType_LTROut`: Left-to-right draw out effect

- `PrismTransitionType_RTLIn`: Right-to-left draw in effect

- `PrismTransitionType_RTLOut`: Right-to-left draw out effect

- `PrismTransitionType_GrowFromCenter`: Grow out from center

- `PrismTransitionType_ShrinkToCenter`: Shrink in to center

- `PrismTransitionType_SlideRight`: Slide entire image to the Right

- `PrismTransitionType_SlideLeft`: Slide entire image to the Left

- `PrismTransitionType_FadeIn`: "Fade" in, checkerboard pattern

- `PrismTransitionType_FadeOut`: "Fade" out, checkerboard pattern

- `PrismTransitionType_Custom`: User-defined effect - You are repsonsible for implementation

#### Custom Transitions

As outlined above, transitions are essentially controllers to create an animated `LCDPattern`. This is done via constant updates in `transition->update` and `transition->draw`.

In order to create a custom transition animation, you will need to have a good understanding of how `LCDPattern`s work, and some understanding of binary math.

There is not much documentation on `LCDPattern` from Playdate, so I will ouline my understanding of it here.

`LCDPattern` is an alias for `uint8_t[16]`. A `uint8_t` is an 8-bit unsigned integer or, in other words, a value from 0-255. Since it's an 8-bit int, we can easily represent its values with hex (0x00-0xFF) or binary (b00000000-b11111111).

If you are going to create a pattern by hand, I find the binary representation to be the easiest to work with since it (almost) visually represents what you will see in the pattern.

If you are creating a math-based transition effect, then you can store the numbers however makes sense to you.

The way that the pattern works is that the first 8 elements in the array represent the pattern bitmap, and the last 8 elements in the array represent "opacity". Since the playdate is 1-bit, it's not true opacity, but you can think of it in a similar way. It is basically an additional bitmap mask to be applied over the pattern.

So, to define a pattern where the image being masked is fully visible, we'd set all elements to 255, 0xFF, or b11111111.

```C
LCDPattern pattern = {
	// bitmap
	0b11111111,0b11111111,0b11111111,0b11111111,
	0b11111111,0b11111111,0b11111111,0b11111111,
	// mask
	0b11111111,0b11111111,0b11111111,0b11111111,
	0b11111111,0b11111111,0b11111111,0b11111111,
};
```

And conversely, to define a fully invisible image we'd leave the mask bits high (255) and set the bitmap bits low (0).

```C
LCDPattern pattern = {
	// bitmap
	0b00000000,0b00000000,0b00000000,0b00000000,
	0b00000000,0b00000000,0b00000000,0b00000000,
	// mask
	0b11111111,0b11111111,0b11111111,0b11111111,
	0b11111111,0b11111111,0b11111111,0b11111111,
};
```

To explain how building a pattern works, I will defer to one of the built-in patterns: `GrowFromCenter`

```C
transition->_exp1 = 3;
transition->_exp2 = 4;

...

static void GrowFromCenter_update( PrismTransition* self, float delta ) {

    bool done = true;

    for( uint8_t i = 0; i < 8; i++ ) {
        
        if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
            self->pattern[i] += ( prismaticUtils->uint8_pow( 2, self->_exp1 ) + prismaticUtils->uint8_pow( 2, self->_exp2 ) );

            if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
                done = false;
            }
        } else {
            self->pattern[i] = PRISM_TRANSITION_MAX;
        }

    }

    if( self->_exp1 > 0 ) self->_exp1--;
    if( self->_exp2 < 7 ) self->_exp2++;

    if( done ) {
        prismaticTransition->complete( self );
    }

}
```

What's going on in the above is as follows:

- Set `_exp1` to 3 and `_exp2` to 4

- Each pass through update:

	- Loop through the first 8 elements of the pattern
		- Check if the current element is less than 255
		- If so, then update its value to `current_value + 2^_exp1 + 2^_exp2`
		- Check if each value has reached 255, if not, keep updating until they all reach 255
	- Decrement `_exp1`, unless it has already reached 0
	- Increment `_exp2`, unless it has already reached 7
	- If all 8 elements have reached 255 complete the transition (stop running update)

The math works like this, I'm only showing a single number for simplicity:

- We have a binary number starting at 0: `x = 00000000`

- In order to fill out the number from the center we start at bits 3 and 4

- On our first pass, we say `x = 0 + (2^3) + (2^4)`, and we end up with `x = 0 + 8 + 16` or `x = 24`

	- 24 in binary is `00011000`, we've filled out the 2 center bits!

- Next, we decrement our first exponent to move left one bit, and increment our second one to move right one bit.

- On the next pass we say `x = 24 + (2^2) + (2^5)`, and we end up with `x = 24 + 4 + 32` or `x = 60`

	- 60 in binary is `00111100`, we've got the next two 

- Continuing down this path will eventually lead to 255 or `11111111` at which point the transition will complete

### Tile Maps

Prismatic Engine includes tools for importing and working with LDtk Tile Map exports. 

Specifically, the tile map system uses the LDtk super-simple json export format. 
It is worth noting that super-simple export creates bitmaps for each layer which
combine all tiles from that layer into a single rendered image. This sacrifices
individual tiles for simplicity.

Prismatic Engine loads each layer as its own bitmap, which you may choose to render in
its raw bitmap form, or as a sprite. Layers can be accessed via `map->layers`.

Collision is imported via csv files generated by LDtk. In LDtk, you should add any number 
of Integer Grid layers to represent your collision layers. These layers will be saved as 
csv files in your Level export directory. You need to specify an array of file names for 
collision layers when initializing your map.

**Type Name**: `LDtkTileMap`

- `string id`: The Map's identifier

- `string iid`: The Map's unique identifier

- `int worldX`: The X position of the Map in the "World" (i.e.: Screen Space)

- `int worldY`: The Y position of the Map in the "World" (i.e.: Screen Space)

- `int width`: The width of the Map

- `int height`: The height of the Map

- `int tileSize`: The tile size of the Map

- `int gridHeight`: The Map's grid height ( height / tileSize ), used for collision & sprite positioning

- `int gridWidth`: The Map's grid width ( width / tileSize ), used for collision & sprite positioning

- `size_t _collisionLayerCount`: The total number of collision layers for the Map

- `LDtkCollisionLayer** collision`: The Map's collision layers

- `size_t _neighborCount`: The total number of neighbor references 

- `LDtkTileMapRef** neighborLevels`: The Map's Neighbor Levels

- `size_t _layerCount`: The total number of Map layers

- `LDtkLayer** layers`: The Map's layers

- `LDtkEntity** entities`: The Map's Entities (not implemented, coming soon)

- `size_t _layerSpriteCount`: The total number of layer Sprites

- `LCDSprite** _layerSprites`: The Map's layer Sprites

- `string _path`: The path to the Map's files

- `void ( *enter )( struct LDtkTileMap* )`: Optional callback for when the map is set as current in the MapManager

	- **Param**: `LDtkTileMap* self`

- `void ( *exit )( struct LDtkTileMap* )`: Optional callback for when the map transitions away from current in the MapManager

	- **Param**: `LDtkTileMap* self`


**Type Name**: `LDtkLayer`

- `string filename`: The file name for the Layer.

- `int zIndex`: The Layer's z-index. Set automatically, based on import. Applies to Layer Sprites when rendering Layers as Sprites.

- `LCDBitmap* image`: The raw `LCDBitmap*` containing the Layer image.


**Type Name**: `LDtkTileMapRef`

- `string levelIid`: The unique id for the referenced Map

- `string dir`: A string representing the direction of the neighbor. "n, s, e, w"


**Type Name**: `LDtkCollisionLayer`

- `string name`: The name of the Collision Layer (matches file name).

- `int** collision`: The int array representing the Collision Layer definition.

- `LCDSprite** rects`: The Collision Layer sprites.


**Example**:

```C
const int TILE_SIZE = 16;
static LDtkTileMap* map;

typedef enum {
	kFloor = 0,
	kWall = 1,
} CollisionType;

// Use init to hook into the Engine's initialization function. This runs 
// before the first call to update but after the engine has initialized itself
static void init() {
    
    prismaticLogger->info( "init" );

	// assets/maps/Level_0/Floor.csv & assets/maps/Level_0/Walls.csv
    string collision[] = { "Floor", "Walls", NULL }; 

	// Initialize the map
    map = prismaticTileMap->new( "assets/maps/Level_0/", TILE_SIZE, collision );

	// Add the map layers and collision to the screen
    prismaticTileMap->add( map );
    prismaticTileMap->addCollision( map );

	prismaticTileMap->tagCollision( map, collision[0], kFloor );
	prismaticTileMap->tagCollision( map, collision[1], kWall );

}

// update is your game's entry point to the engine
static void update( float delta ) {

}

// draw should be used to handle screen drawing operations. It is called after 
// update
static void draw( float delta ) {
    sprites->updateAndDrawSprites();
    sys->drawFPS( 0, 0 );

	// If not adding the map layers as Sprites, you can choose to draw the raw
	// bitmaps on each frame like so.
    // prismaticTileMap->draw( map );
}

// destroy is called when the game is shut down, before the game itself is
// freed from memory
static void destroy() {
    prismaticTileMap->delete( map );
}
```

**Type Name**: `LDtkMapManager`

`LDtkMapManager` is a convenience type for keeping track of active maps.

Maps can be added to the map manager, and it will track the current map and the previous map. You can use multiple `MapManager`s at once, if desirable.

When a Map in a MapManager becomes the current Map, it will run its `enter` function, if one has been set.

When a Map in a MapManager leaves the current state, it will run its `exit` function, if one has been set.

- `size_t _mapCount`: The number of Maps stored in the MapManager

- `LDtkTileMap** maps`: The MapManager's Maps

- `LDtkTileMap* currentMap`: The MapManager's current Map

- `LDtkTileMap* previousMap`: The MapManager's previous Map

**Example**:

```C
const int TILE_SIZE = 16;
static LDtkTileMap* map;
static LDtkMapManager* mm;

typedef enum {
	kFloor = 0,
	kWall = 1,
} CollisionType;

static void mapEnter( LDtkTileMap* self ) {

	// Add the map layers and collision to the screen
    prismaticTileMap->add( self );
    prismaticTileMap->addCollision( self );

}

static void mapExit( LDtkTileMap* self ) {

	// Remove the map layers and collision from the screen
    prismaticTileMap->remove( self );
    prismaticTileMap->removeCollision( self );

}

// Use init to hook into the Engine's initialization function. This runs 
// before the first call to update but after the engine has initialized itself
static void init() {
    
    prismaticLogger->info( "init" );

	// assets/maps/Level_0/Floor.csv & assets/maps/Level_0/Walls.csv
    string collision[] = { "Floor", "Walls", NULL }; 

	// Initialize the map
    map = prismaticTileMap->new( "assets/maps/Level_0/", TILE_SIZE, collision );

	prismaticTileMap->tagCollision( map, collision[0], kFloor );
	prismaticTileMap->tagCollision( map, collision[1], kWall );

	map->enter = mapEnter;
	map->exit = mapExit;

	// Add map to the Map Manager, and set it as current
    prismaticMapManager->changeMap( mm, map );

}

// update is your game's entry point to the engine
static void update( float delta ) {

}

// draw should be used to handle screen drawing operations. It is called after 
// update
static void draw( float delta ) {
    sprites->updateAndDrawSprites();
    sys->drawFPS( 0, 0 );
}

// destroy is called when the game is shut down, before the game itself is
// freed from memory
static void destroy() {
	prismaticMapManager->delete( mm );
    prismaticTileMap->delete( map );
}
```

### Strings

**Type Name**: `string`

A `string` is simply an alias for `char*`, for convenience purposes, and can be used interchangably with `char*` and `const char*`.

### Dialogue Boxes & Dialogue Management

Prismatic Engine provides a robust system for creating and managing dialogue boxes within your game.

The dialogue, script, and line managers outlined in the utilities section are designed to handle the creation of dialogue "scripts", which consist of "lines". 

A dialogue line is simply a string or a pointer to a function. Lines that are strings will be displayed as text in a dialogue box when a script is added, and the line is active. Lines that are function pointers will execute that function when the line is active. The purpose of functions as dialogue lines is to allow you to execute game logic as dialogue (audio cues, logic flags, etc).

A dialogue script is really just an array of lines with an internal pointer that is keeping track of which line is currently active. Scripts are named with unique string ids, created by you, so that a script can be easily selected and added to a dialogue box as needed.

See the sample script created in `src/core/dialogue/` and how dialogue is used in `src/core/scenes/playscene.c` for more details. You don't necessarily need to implement dialogue in the same way that I have done in the example, it's just there as a starter for those who want to get running quickly.

**Type Name**: `Dialogue`

- `string text`: The dialogue box's current text - Controls what text is being displayed

- `float x`: The dialogue box's current X position

- `float y`: The dialogue box's current Y position

- `float _innerX`: Used internally by Dialogue methods to track the actual X render position based on X position, X padding, border width, and box width

- `float _innerY`: Used internally by Dialogue methods to track the actual Y render position based on Y position, Y padding, border height, and box height

- `float camX`: The X position of the context in which the Dialogue box is rendered (The context is the size of the screen)

- `float camY`: The Y position of the context in which the Dialogue box is rendered (The context is the size of the screen)

- `bool autoHide`: Determines whether the dialogue box should auto-hide or not. Default: False

- `float showDuration`: When autoHide is enabled, determines the amount of time the dialogue box should appear before hiding

- `float boxWidth`: The width of the dialogue box before a border is added

- `float boxHeight`: The height of the dialogue box before a border is added

- `float borderWidth`: The width of the dialogue box's border

- `float borderHeight`: The height of the dialogue box's border

- `float textPaddingX`: The horizontal padding for the dialogue box's text

- `float textPaddingY`: The vertical padding for the dialogue box's text

- `float speed`: The text speed for the dialogue box - Speed is number of seconds before the pointer advances to the next character in the active text

- `float pitch`: The pitch of the synth used for the dialogue box

- `PDSynth* synth`: The synth, used internally by the dialogue box to play audio when a character is rendered

- `DialogueState state`: The current state of the dialogue box (see `DialogueState`)

- `DialogueType type`: The type of dialogue box (see `DialogueType`)

- `LCDBitmap* _canvas`: Used internally by Dialogue - This is the context onto which the dialogue box is drawn

- `LCDBitmap* _overlay`: Currently unused - Intended for indicators eventually

- `LCDFont* font`: The dialogue box's font

- `LCDColor borderColor`: The color of the dialogue box's border 

- `LCDColor boxColor`: The background color of the dialogue box

- `LCDBitmapDrawMode fontDrawMode`: The draw mode used to render the dialogue box's font

- `PDTextWrappingMode wrapMode`: The wrap mode applied to text within the dialogue box

- `PDTextAlignment alignment`: The text alignment applied to text within the dialouge box

- `int _drawDialoguePointer`: Internal pointer used by typewriter mode to determine the active characters to display

- `float _drawDialogueTimer`: Internal timer used by typewriter mode to advance to the next character

- `bool _lineFinished`: Internal flag used by typewriter mode to determine when a line has finished

- `PDButtons _input_current`: Used by handleInput to keep track of actively pressed buttons

- `PDButtons _input_pressed`: Used by handleInput to keep track of buttons that were just pressed

- `PDButtons _input_released`: Used by handleInput to keep track of buttons that were just released

- `bool _finished`: Keeps track of whether the dialogue box's script is finished or not

- `float _hiddenTimer`: Used internally by Dialogue to manage a small buffer after hiding a dialogue box

- `bool _justHidden`: Keeps track of when a dialogue box was just hidden

- `DialogueScript* _script`: The dialogue box's active script

- `void (*handleInput)( struct Dialogue* dialogue, float delta )`: An optional user-created function to handle any input when the dialogue box is active 

- `void (*onFinishedCallback)( struct Dialogue* dialogue )`: An optoinal user-created callback function to perform actions when the dialogue box's script is finished

**Type Name**: `DialogueScript`

- `string id`: The script's unique identifier - Used for lookup when the script is needed

- `size_t _linePointer`: The internal pointer that keeps track of which line in the script is currently active

- `DialogueLine** _lines`: The array of lines that belong to the script

- `size_t _lineCount`: The total number of lines in the script

**Type Name**: `DialogueLine`

- `DialogueLineType type`: The type of line - See `DialogueLineType`

- `string text`: The line's text 

- `void (*fn)( void )`: The line's function pointer

#### DialogueState Values
- `D_Hide`: The state applied when a dialogue box is hidden - Enum Value = 0
- `D_Show`: The state applied when a dialogue box is shown - Enum Value = 1

#### DialogueType Values
- `DT_Instant`: Render all text instantly - Enum Value = 0
- `DT_Typewriter`: Render text one character at a time, like a typewriter - Enum Value = 1

#### DialogueLineType Values
- `DialogueLineType_String`: Represents a DialogueLine with a string value - Enum Value = 0
- `DialogueLineType_Fn`: Represents a DialogueLine that should execute a function - Enum Value = 1


### Camera System

The engine provides an optional camera system for your game. The "camera" works by setting a draw offset on the screen based on the type of Camera you define, and the target of said camera.

There are 4 types of Cameras available:

- **Fixed Cam**: A fixed Camera has (x, y) coordinates that you specify when you initialize the Camera. The coordinates are not automatically updated via any internal Camera mechanism. The coordinates can be updated via the `cameraController->setPos()` or `cameraController->moveTo()` functions, if desired, and when updated the Camera will instantly change to the new coordinates. This type of Camera can be used to create a static camera, or a Camera that needs to "warp" to specific coordinates. 

- **Lerp Cam**: A lerp Camera has (x, y) coordinates that you specify when you initialize the Camera. The coordinates are not automatically updated via any internal Camera mechanism. The coordinates can be updated via the `cameraController->moveTo()` function, and when updated the Camera will smoothly pan to the new coordinates using linear interpolation. Transition time is controlled via the time parameter passed into `newLerpCam`.

- **Follow Cam**: A follow Camera sets its own (x, y) coordinates based on the position of any `PrismSprite*` that is passed into it during initialization. The Camera's position will continue to be updated on each pass through the update loop with the current position of the `PrismSprite` that it is following. The Camera will follow the `PrismSprite`'s position exactly and instantly, and will not trail behind.

- **Follow Lerp Cam**: A follow-lerp Camera is a combination of a lerp Camera and a follow Camera. It sets its own (x, y) coordinates based on the position of any `PrismSprite*` that is passed into it during initialization. The Camera's position will continue to be updated on each pass through the update loop with the current position of the `PrismSprite` that it is following. The Camera will smoothly pan to the `PrismSprite`'s current location, meaning that it will lag behind the target. Transition time is controlled via the time parameter passed into `newFollowLerpCam`.

#### Restricting Camera bounds

All Camera types can be restricted using the `setMinBounds`, `setMaxBounds`, and `setRestricted` functions. The min/max bounds of a restricted Camera are set by the respective functions, and `cameraController->setRestricted( true )` enables restricted mode. 

When a Camera is restricted it will move freely until it hits its minimum or maximum bounds, where it will then be fixed if it would exceed them.

#### Dialogue & Cameras

Dialogue boxes are rendered separately from Cameras by default. The Camera system provides a way to attach Dialogue boxes to a Camera, so they render at the same offset as the Camera.

To do this, you would use `cameraController->pushDialogue()`, and pass in the Dialogue that you would like to attach to your Camera. Cameras are capable of handling multiple Dialogue boxes, but you will need to call `pushDialogue` for each Dialogue box that you want to attach to the Camera. 

**Type Name**: `Camera`

```C
typedef struct Camera {

    // The Camera Type - See CameraStyle in camera.h
    CameraStyle _cameraType;

    // The camera's target - Used by Follow Cams
    PrismSprite* __target;

    // The total dialogues currently added to the Camera
    size_t __dialogueCount; 

    // The Dialogue boxes contained by the Camera
    Dialogue** dialogues;

    // The camera x origin - Default: center of the screen
    float __originX;

    // The camera y origin - Default: center of the screen
    float __originY;

    // The camera's X position - Use setPos to set this value
    float __x;

    // The camera's Y position - Use setPos to set this value
    float __y;

    // The target X position - Used by Lerp Cams
    float __targetX;

    // The target Y position - Used by Lerp Cams
    float __targetY;

    // The lerp time for this Camera - Used by Lerp Cams
    float __lerpTime;

    // Used with restricted prop - The minimum X bounds
    float __minX;

    // Used with restricted prop - The maximum X bounds
    float __maxX;

    // Used with restricted prop - The minimum Y bounds
    float __minY;

    // Used with restricted prop - The maximum Y bounds
    float __maxY;

    // Is the camera movement restricted to min/max bounds? True/False
    // 
    // Default: False
    bool __restricted;

    // Will be true after the first update loop
    bool __init;
} Camera;
```