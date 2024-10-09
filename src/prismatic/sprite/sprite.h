// sprite.h
#ifndef SPRITE_H
#define SPRITE_H

#ifndef STDBOOL_INCLUDED
	#include <stdbool.h>
	#define STDBOOL_INCLUDED
#endif

#ifndef STDDEF_INCLUDED
	#include <stddef.h>
	#define STDDEF_INCLUDED
#endif

#ifndef PD_API_INCLUDED 
	#include "pd_api.h"  
	#define PD_API_INCLUDED  
#endif

#ifndef TEXT_INCLUDED 
	#include "../text/text.h"
	#define TEXT_INCLUDED  
#endif

typedef struct PrismSprite PrismSprite;
typedef struct PrismAnimation PrismAnimation;
typedef struct SpriteFn SpriteFn;

typedef struct PrismSprite {
	string id;
	LCDSprite* sprite;
	LCDBitmap** imgs;
	PrismAnimation* animation;
	void* ref;
	void ( *update )( PrismSprite*, float );
	void ( *destroy )( PrismSprite* );
} PrismSprite;

typedef struct PrismAnimation {
	LCDBitmap** frames;
	size_t frameCount;
	size_t currentFrame;
	size_t customOrderPtr;
	float playSpeed;
	PrismSprite* sprite;
	bool looping;
	bool finished;
	bool paused;
	float timer;
	void ( *complete )( PrismAnimation* );
} PrismAnimation;

typedef struct SpriteFn {
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
} SpriteFn;

typedef struct AnimationFn {
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
} AnimationFn;

extern const SpriteFn* prismaticSprite;
extern const AnimationFn* prismaticAnimation;

#endif  // SPRITE_H