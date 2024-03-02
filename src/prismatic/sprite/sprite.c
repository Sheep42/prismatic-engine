#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>

#include "../prismatic.h"

// Sprites

static PrismSprite* newFromPath( string path );
static PrismSprite* newFromImages( LCDBitmap** frames, int startFrame, int frameRate );
static void deleteSprite( PrismSprite* sprite );
static LCDBitmap** loadImages( string* paths, size_t pathCount );

static PrismSprite* newFromPath( string path ) {

}

static PrismSprite* newFromImages( LCDBitmap** frames, int startFrame, int frameRate ) {

}

static void deleteSprite( PrismSprite* sprite ) {

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
		images = sys->realloc( images, imgCount * sizeof(LCDBitmap*) );
		images[imgCount- 1] = img;

	}

	return images;

}

const SpriteFn* prismaticSprite = &(SpriteFn) {
	.loadImages = loadImages,
};

// Animations