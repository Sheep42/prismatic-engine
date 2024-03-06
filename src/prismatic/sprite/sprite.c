#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "../prismatic.h"
#include "sprite.h"

static LCDSprite* newLCDSprite( LCDBitmap* image );
static PrismSprite* newFromPath( string path );
static PrismSprite* newFromImages( LCDBitmap** frames, size_t startFrame, uint frameRate );
static void deleteSprite( PrismSprite* sp );
static void freeImages( LCDBitmap** images );
static LCDBitmap** loadImages( string* paths, size_t pathCount );
static LCDBitmap* loadImage( string path );
static void setSpriteAnimation( PrismSprite* sp, PrismAnimation* animation );

static PrismAnimation* newAnimation( LCDBitmap** frames, size_t startFrame, uint frameRate );
static void deleteAnimation( PrismAnimation* animation );
static void playAnimation( PrismAnimation* animation );
static void playAnimationInOrder( PrismAnimation* animation, size_t frameOrder[] );

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

static PrismSprite* newFromPath( string path ) {

	PrismSprite* s = sys->realloc( NULL, sizeof(PrismSprite));

	LCDBitmap* image = loadImage( path );
	s->sprite = newLCDSprite( image );

	graphics->freeBitmap( image );

	return s;

}

static PrismSprite* newFromImages( LCDBitmap** frames, size_t startFrame, uint frameRate ) {
	
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

	if( frameCount > 1 ) {
		s->animation = newAnimation( frames, startFrame, frameRate );
		s->animation->sprite = s;
	}
	
	s->update = NULL;
	s->destroy = NULL;

	return s;

}

static void deleteSprite( PrismSprite* s ) {

	if( s->destroy != NULL ) {
		s->destroy( s );
	}

	sprites->setImage( s->sprite, NULL, 0 );
	sprites->removeSprite( s->sprite );
	sprites->freeSprite( s->sprite );

	if( s->animation != NULL ) {
		deleteAnimation( s->animation );
	}

	sys->realloc( s, 0 );

}

static void freeImages( LCDBitmap** images ) {

	if( images == NULL ) {
		return;
	}

	for( size_t i = 0; images[i] != NULL; i++ ) {
		graphics->freeBitmap( images[i] );
	}

	images = sys->realloc( images, 0 );

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

static LCDBitmap* loadImage( string path ) {

	string paths[1] = { path };
    LCDBitmap** images = prismaticSprite->loadImages( paths, 1 );

    size_t imageCount = 0;
    while( images[imageCount] != NULL ) {
    	imageCount++;
    }

    if( imageCount != 1 ) {
    	prismaticLogger->errorf( "Could not load image at path: %s", path );
    	return NULL;
    }

    LCDBitmap* img = images[0];
    freeImages( images );

    return img;

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
	.loadImage = loadImage,
	.newFromPath = newFromPath,
	.newFromImages = newFromImages,
	.delete = deleteSprite,
	.setAnimation = setSpriteAnimation,
};

// Animations

static PrismAnimation* newAnimation( LCDBitmap** frames, size_t startFrame, uint frameRate ) {

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
	animation->frameRate = frameRate;
	animation->looping = true; 

	return animation;

}

static void deleteAnimation( PrismAnimation* animation ) {

	free( animation );

}

static void playAnimation( PrismAnimation* animation ) {

	if( animation->finished ) {
		return;
	}

	animation->currentFrame++;

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

	}

}

static void playAnimationInOrder( PrismAnimation* animation, size_t frameOrder[] ) {

}

const AnimationFn* prismaticAnimation = &(AnimationFn) {
	.new = newAnimation,
	.delete = deleteAnimation,
	.play = playAnimation,
	.playInOrder = playAnimationInOrder,
};

// Animations