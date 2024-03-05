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

	LCDSprite* sp = sprite->newSprite();
    sprite->setImage( sp, image, 0 );

    int width, height;
    graphics->getBitmapData( image, &width, &height, NULL, NULL, NULL );

    PDRect spBounds = PDRectMake( 0, 0, width, height );
    sprite->setBounds( sp, spBounds );
    sprite->setZIndex( sp, 0 );

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
	
	s->update = NULL;
	s->destroy = NULL;

	return s;

}

static void deleteSprite( PrismSprite* s ) {

	if( s->destroy != NULL ) {
		s->destroy( s );
	}

	sprite->setImage( s->sprite, NULL, 0 );
	sprite->removeSprite( s->sprite );
	sprite->freeSprite( s->sprite );

	// TODO: Destroy Animation
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

	return animation;

}

static void deleteAnimation( PrismAnimation* animation ) {

}

static void playAnimation( PrismAnimation* animation ) {

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