#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "../prismatic.h"
#include "sprite.h"

static LCDSprite* newLCDSprite( LCDBitmap* image );
static PrismSprite* newFromPath( string* paths, size_t pathCount, float playSpeed );
static PrismSprite* newFromImages( LCDBitmap** frames, size_t startFrame, float playSpeed );
static void deleteSprite( PrismSprite* sp );
static void freeImages( LCDBitmap** images );
static LCDBitmap** loadImages( string* paths, size_t pathCount );
static void setSpriteAnimation( PrismSprite* sp, PrismAnimation* animation );

static PrismAnimation* newAnimation( LCDBitmap** frames, size_t startFrame, float playSpeed );
static void deleteAnimation( PrismAnimation* animation );
static void playAnimation( PrismAnimation* animation, float delta );
static void playAnimationInOrder( PrismAnimation* animation, float delta, size_t* frameOrder, size_t frameCount );

// Sprites

static LCDSprite* newLCDSprite( LCDBitmap* image ) {

	LCDSprite* sp = sprites->newSprite();
    sprites->setImage( sp, image, 0 );

    int width, height;
    graphics->getBitmapData( image, &width, &height, NULL, NULL, NULL );

    PDRect spBounds = PDRectMake( 0, 0, width, height );
    sprites->setBounds( sp, spBounds );
    sprites->setZIndex( sp, 0 );

    return sp;

}

static PrismSprite* newFromPath( string* path, size_t pathCount, float playSpeed ) {

	size_t start = 0;

	LCDBitmap** imgs = loadImages( path, pathCount );
	if( imgs == NULL ) {
		prismaticLogger->errorf( "Could not create Sprite from path '%s'", path );
		return NULL;
	}

	PrismSprite* s = newFromImages( imgs, start, playSpeed );
	
	if( s == NULL ) {
		prismaticLogger->errorf( "Could not create Sprite from path '%s'", path );
		return NULL;
	}

	s->imgs = imgs;

	return s;

}

static PrismSprite* newFromImages( LCDBitmap** frames, size_t startFrame, float playSpeed ) {
	
	PrismSprite* s = sys->realloc( NULL, sizeof(PrismSprite));

	size_t frameCount = 0;
	while( frames[frameCount] != NULL ) {
		frameCount++;
	}

	if( frameCount < 1 ) {
		prismaticLogger->error( "Cannot create Sprite with no frames" );
		return NULL;
	}

	if( (frameCount - 1) < startFrame ) {
		prismaticLogger->errorf( "startFrame %d is out of bounds. frames size: %d", startFrame, frameCount );
		return NULL;
	}

	LCDBitmap* image = frames[startFrame];
	s->sprite = newLCDSprite( image );

	s->id = NULL;
	s->animation = NULL;

	if( frameCount > 1 ) {
		s->animation = newAnimation( frames, startFrame, playSpeed );
		s->animation->sprite = s;
	}
	
	s->update = NULL;
	s->destroy = NULL;
	s->imgs = NULL;

	return s;

}

static void deleteSprite( PrismSprite* s ) {

	if( s->destroy != NULL ) {
		s->destroy( s );
	}

	sprites->setImage( s->sprite, NULL, 0 );
	sprites->removeSprite( s->sprite );
	sprites->freeSprite( s->sprite );

	if( s->id != NULL ) {
		prismaticString->delete( s->id );
		s->id = NULL;
	}

	if( s->animation != NULL ) {
		deleteAnimation( s->animation );
		s->animation = NULL;
	}

	if( s->imgs != NULL ) {
		freeImages( s->imgs );
	}

	s = sys->realloc( s, 0 );
	s = NULL;

}

static void freeImages( LCDBitmap** images ) {

	if( images == NULL ) {
		return;
	}

	for( size_t i = 0; images[i] != NULL; i++ ) {
		graphics->freeBitmap( images[i] );
	}

	sys->realloc( images, 0 );
	images = NULL;

}

static LCDBitmap** loadImages( string* paths, size_t pathCount ) {

	const char *outErr = NULL;
	size_t imgCount = 0;
	LCDBitmap** images = sys->realloc( NULL, sizeof(LCDBitmap*) );

	for( size_t i = 0; i < pathCount; i++ ) {

		string path = paths[i];

		LCDBitmap *img = pd->graphics->loadBitmap( path, &outErr );
		if ( outErr != NULL ) {
			prismaticLogger->errorf( "Error loading image at path '%s': %s", path, outErr );
			continue;
		}

		imgCount += 1;
		images = sys->realloc( images, imgCount * sizeof(LCDBitmap*) + 1 );
		images[imgCount - 1] = img;

	}

	images[imgCount] = NULL;

	return images;

}

static void setSpriteAnimation( PrismSprite* sp, PrismAnimation* animation ) {

	if( sp == NULL ) {
		prismaticLogger->error( "Cannot set Animation for NULL Sprite!" );
		return;
	}

	if( animation == NULL ) {
		prismaticLogger->error( "Cannot set NULL Animation for Sprite!" );
		return;
	}

	sp->animation = animation;
	animation->sprite = sp;

}

const SpriteFn* prismaticSprite = &(SpriteFn) {
	.freeImages = freeImages,
	.loadImages = loadImages,
	.newFromPath = newFromPath,
	.newFromImages = newFromImages,
	.delete = deleteSprite,
	.setAnimation = setSpriteAnimation,
};

// Animations

static PrismAnimation* newAnimation( LCDBitmap** frames, size_t startFrame, float playSpeed ) {

	PrismAnimation* animation = calloc( 1, sizeof( PrismAnimation ) );

	size_t frameCount = 0;
	while( frames[frameCount] != NULL ) {
		frameCount++;
	}

	if( frameCount < 1 ) {
		prismaticLogger->error( "Cannot create Animation with no frames" );
		return NULL;
	}

	if( (frameCount - 1) < startFrame ) {
		prismaticLogger->errorf( "startFrame %d is out of bounds. frames size: %d", startFrame, frameCount );
		return NULL;
	}

	animation->frames = frames;
	animation->frameCount = frameCount;
	animation->currentFrame = startFrame;
	animation->playSpeed = playSpeed;
	animation->looping = true; 
	animation->paused = false;

	return animation;

}

static void deleteAnimation( PrismAnimation* animation ) {

	animation->frameCount = 0;
	animation->frames = NULL;
	animation->currentFrame = 0;
	animation->playSpeed = 0;

	free( animation );
	animation = NULL;

}

static void playAnimation( PrismAnimation* animation, float delta ) {

	if( animation->finished || animation->paused ) {
		return;
	}

	animation->timer += delta;

	if( animation->timer < animation->playSpeed ) {
		return;
	}

	if( animation->currentFrame >= animation->frameCount ) {

		if( animation->complete != NULL ) {
			animation->complete( animation );
		}

		if( animation->looping ) {
			animation->currentFrame = 0;
			return;
		}
		
		animation->finished = true;

	} else {

		LCDBitmap* nextFrame = animation->frames[animation->currentFrame];

		if ( nextFrame == NULL ) {
			prismaticLogger->errorf( "NULL frame in Animation at [%d]", animation->currentFrame );
			return;
		}

		if( animation->sprite == NULL ) {
			prismaticLogger->errorf( "NULL Sprite in Animation" );
			return;
		}

		sprites->setImage( animation->sprite->sprite, animation->frames[animation->currentFrame], kBitmapUnflipped );
		
		animation->timer = 0;
		animation->currentFrame++;

	}

}

static void playAnimationInOrder( PrismAnimation* animation, float delta, size_t* frameOrder, size_t frameCount ) {

	if( animation->finished ) {
		return;
	}

	animation->timer += delta;

	if( animation->timer < animation->playSpeed ) {
		return;
	}

	if( animation->currentFrame >= animation->frameCount || animation->customOrderPtr >= frameCount ) {

		if( animation->complete != NULL ) {
			animation->complete( animation );
		}

		if( animation->looping ) {
			animation->currentFrame = frameOrder[0];
			animation->customOrderPtr = 0;
			return;
		}
		
		animation->finished = true;

	} else {

		LCDBitmap* nextFrame = animation->frames[animation->currentFrame];

		if ( nextFrame == NULL ) {
			prismaticLogger->errorf( "NULL frame in Animation at [%d]", animation->currentFrame );
			return;
		}

		if( animation->sprite == NULL ) {
			prismaticLogger->errorf( "NULL Sprite in Animation" );
			return;
		}

		sprites->setImage( animation->sprite->sprite, animation->frames[animation->currentFrame], kBitmapUnflipped );
		
		animation->timer = 0;
		animation->customOrderPtr++;
		animation->currentFrame = frameOrder[animation->customOrderPtr];

	}

}

const AnimationFn* prismaticAnimation = &(AnimationFn) {
	.new = newAnimation,
	.delete = deleteAnimation,
	.play = playAnimation,
	.playInOrder = playAnimationInOrder,
};

// Animations