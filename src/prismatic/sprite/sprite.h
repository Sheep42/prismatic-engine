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
	PrismAnimation* animation;
	void ( *update )( PrismSprite*, float );
	void ( *destroy )( PrismSprite* );
} PrismSprite;

typedef struct PrismAnimation {
	LCDBitmap** frames;
	size_t frameCount;
	size_t currentFrame;
	uint frameRate;
	PrismSprite* sprite;
	bool looping;
	bool finished;
	void ( *complete )( PrismAnimation* );
} PrismAnimation;

typedef struct SpriteFn {
	PrismSprite* ( *newFromPath )( string );
	PrismSprite* ( *newFromImages )( LCDBitmap**, size_t, uint );
	void ( *delete )( PrismSprite* );
	void ( *freeImages )( LCDBitmap** );
	LCDBitmap** ( *loadImages )( string*, size_t );
	LCDBitmap* ( *loadImage )( string );
	void ( *setAnimation )( PrismSprite*, PrismAnimation* );
} SpriteFn;

typedef struct AnimationFn {
	PrismAnimation* ( *new )( LCDBitmap**, size_t, uint );
	void ( *delete )( PrismAnimation* );
	void ( *play )( PrismAnimation* );
	void ( *playInOrder )( PrismAnimation*, size_t[] );
} AnimationFn;

extern const SpriteFn* prismaticSprite;
extern const AnimationFn* prismaticAnimation;

#endif  // SPRITE_H