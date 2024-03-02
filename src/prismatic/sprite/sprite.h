// sprite.h
#ifndef SPRITE_H
#define SPRITE_H

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
	void ( *setAnimation )( LCDBitmap**, int, int );
	void ( *update )( float );
	void ( *destroy )( void );
} PrismSprite;

typedef struct PrismAnimation {
	LCDBitmap** frames;
	size_t frameCount;
	int currentFrame;
	int frameRate;
} PrismAnimation;

typedef struct SpriteFn {
	PrismSprite* ( *newFromPath )( string );
	PrismSprite* ( *newFromImages )( LCDBitmap**, int, int );
	void ( *delete )( PrismSprite* );
	LCDBitmap** ( *loadImages )( string*, size_t );
} SpriteFn;


extern const SpriteFn* prismaticSprite;

#endif  // SPRITE_H