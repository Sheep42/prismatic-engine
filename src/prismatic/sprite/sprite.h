// sprite.h
#ifndef SPRITE_H
#define SPRITE_H

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
	PrismAnimation* animation;
	void ( *setAnimation )( PrismSprite*, LCDBitmap**, size_t, int );
	void ( *update )( PrismSprite*, float );
	void ( *destroy )( PrismSprite* );
} PrismSprite;

typedef struct PrismAnimation {
	LCDBitmap** frames;
	size_t frameCount;
	int currentFrame;
	int frameRate;
} PrismAnimation;

typedef struct SpriteFn {
	PrismSprite* ( *newFromPath )( string );
	PrismSprite* ( *newFromImages )( LCDBitmap**, size_t, int );
	void ( *delete )( PrismSprite* );
	void ( *freeImages )( LCDBitmap** );
	LCDBitmap** ( *loadImages )( string*, size_t );
	LCDBitmap* ( *loadImage )( string );
} SpriteFn;


extern const SpriteFn* prismaticSprite;

#endif  // SPRITE_H