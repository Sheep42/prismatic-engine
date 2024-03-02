// sprite.h
#ifndef SPRITE_H
#define SPRITE_H

#include <stddef.h>
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
	LCDSprite* sprite;
	PrismAnimation* animation;
	void ( *setAnimation )( LCDBitmap** frames, size_t startFrame, int frameRate );
	void ( *update )( float delta );
	void ( *destroy )( void );
} PrismSprite;

typedef struct PrismAnimation {
	LCDBitmap** frames;
	size_t frameCount;
	int currentFrame;
	int frameRate;
} PrismAnimation;

typedef struct SpriteFn {
	PrismSprite* ( *newFromPath )( string path );
	PrismSprite* ( *newFromImages )( LCDBitmap** frames, size_t startFrame, int frameRate );
	void ( *delete )( PrismSprite* sprite );
	void ( *freeImages )( LCDBitmap** images );
	LCDBitmap** ( *loadImages )( string* path, size_t imageCount );
	LCDBitmap* ( *loadImage )( string path );
} SpriteFn;


extern const SpriteFn* prismaticSprite;

#endif  // SPRITE_H