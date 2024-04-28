#include <stdlib.h>
#include <stdio.h>

#include "../prismatic.h"
#include "ldtk.h"

static LDtkTileMap* newLDtkTileMap( string path, int tileSize, string* collisionLayers );
static void deleteLDtkTileMap( LDtkTileMap* map );
static void drawLDtkTileMap( LDtkTileMap* map );
static void addLDtkTileMap( LDtkTileMap* map );
static void removeLDtkTileMap( LDtkTileMap* map );
static void addCollisionLDtkTileMap( LDtkTileMap* map );
static void removeCollisionLDtkTileMap( LDtkTileMap* map );

static void freeMapCollisions( LDtkTileMap* map );
static void freeMapRefs( LDtkTileMap* map );
static void freeMapLayers( LDtkTileMap* map );

static void freeLayer( LDtkLayer* layer );

static LDtkTileMap* getMapByIid( string iid );
static void changeMapByIid( string iid );
static void changeMapByName( string id );
static void changeMap( LDtkTileMap* map );

static void parseCollision( SDFile* file, LDtkTileMap* map );
static void stripNewlines( string str );
static void csvToCollision( string str, LDtkTileMap* map );

static void decodeError( json_decoder* decoder, const char* error, int linenum );
static void willDecodeSublist( json_decoder* decoder, const char* name, json_value_type type );
static int shouldDecodeTableValueForKey( json_decoder* decoder, const char* key ) ;
static void didDecodeTableValue( json_decoder* decoder, const char* key, json_value value );
static int shouldDecodeArrayValueAtIndex( json_decoder* decoder, int pos ) ;
static void didDecodeArrayValue( json_decoder* decoder, int pos, json_value value );
static void didDecodeSublist( json_decoder* decoder, const char* name, json_value_type type );
static void decodeLayers( json_decoder* decoder, int pos, json_value value );
static int newNeighbor( json_decoder* decoder, int pos );
static void decodeNeighbor( json_decoder* decoder, const char* key, json_value value );

static int readfile( void* readud, uint8_t* buf, int bufsize );

// TileMap

static LDtkTileMap* newLDtkTileMap( string path, int tileSize, string* collisionLayers ) {

	const string dataFile = "/data.json";
	string trimmedPath = prismaticString->trimLast( path, '/' );

	string dataPath = prismaticString->new( trimmedPath );
	prismaticString->concat( &dataPath, dataFile );

	LDtkTileMap* map = calloc( 1, sizeof( LDtkTileMap ) );
	json_reader* mapReader = calloc( 1, sizeof( json_reader ) );
	json_decoder* mapDecoder = calloc( 1, sizeof( json_decoder ) );
	SDFile* jsonFile = pd->file->open( dataPath, kFileRead );

	if( jsonFile == NULL ) {
		prismaticLogger->errorf( "Failed to open JSON at path \"%s\"", dataPath );

		// Free allocated memory
		prismaticString->delete( trimmedPath );
		prismaticString->delete( dataPath );
		free( mapReader );
		free( mapDecoder );
		
		return NULL;
	}

	map->_path = trimmedPath;
	map->_layerCount = 0;
	map->_neighborCount = 0;
	map->tileSize = tileSize;
	
	mapReader->read = readfile;
	mapReader->userdata = jsonFile;

	mapDecoder->decodeError = decodeError;
	mapDecoder->shouldDecodeTableValueForKey = shouldDecodeTableValueForKey;
	mapDecoder->didDecodeTableValue = didDecodeTableValue;
	mapDecoder->willDecodeSublist = willDecodeSublist;
	mapDecoder->shouldDecodeArrayValueAtIndex = shouldDecodeArrayValueAtIndex;
	mapDecoder->didDecodeArrayValue = didDecodeArrayValue;
	mapDecoder->didDecodeSublist = didDecodeSublist;
	mapDecoder->userdata = map;

	pd->json->decode( mapDecoder, *mapReader, NULL );

	map->gridWidth = map->width / map->tileSize;
	map->gridHeight = map->height / map->tileSize;

	if( collisionLayers != NULL ) {

		for( size_t i = 0; collisionLayers[i] != NULL; i++ ) {

			if( collisionLayers[i] == "" ) {
				continue;
			}

			SDFile* collisionFile = NULL;
			string collisionPath = prismaticString->new( trimmedPath );
			prismaticString->concat( &collisionPath, "/" );
			prismaticString->concat( &collisionPath, collisionLayers[i] );
			prismaticString->concat( &collisionPath, ".csv" );

			collisionFile = pd->file->open( collisionPath, kFileRead );
			if( collisionFile == NULL ) {
				prismaticLogger->errorf( "Failed to open collision csv at path \"%s\"", collisionPath );

				// Free allocated memory
				prismaticString->delete( trimmedPath );
				prismaticString->delete( dataPath );
				prismaticString->delete( collisionPath );

				free( mapReader );
				free( mapDecoder );

				// Close Files
				pd->file->close( jsonFile );
				return NULL;
			}

			parseCollision( collisionFile, map );

			// free collisionPath
			prismaticString->delete( collisionPath );

			// Close collisionFile
			pd->file->close( collisionFile );

		}

	}

	// Free allocated memory
	prismaticString->delete( trimmedPath );
	prismaticString->delete( dataPath );
	free( mapReader );
	free( mapDecoder );

	// Close Files
	pd->file->close( jsonFile );
	
	return map;

}

static void deleteLDtkTileMap( LDtkTileMap* map ) {

	// Since our strings are duplicated, we need to explicitly free them
	prismaticString->delete( map->id );
	prismaticString->delete( map->iid );

	freeMapCollisions( map );
	freeMapRefs( map );
	freeMapLayers( map );

	free( map );

}

static void drawLDtkTileMap( LDtkTileMap* map ) {

    for( size_t i = 0; map->layers[i] != NULL; i++ ) {
		LDtkLayer* layer = map->layers[i];
		graphics->drawBitmap( layer->image, 0, 0, kBitmapUnflipped );
	}

}

static void addLDtkTileMap( LDtkTileMap* map ) {
 
	if( map->layers == NULL ) {
		return;
	}

    for( size_t i = 0; map->layers[i] != NULL; i++ ) {

		map->_layerSpriteCount++;
		map->_layerSprites = sys->realloc( map->_layerSprites, sizeof( LCDSprite* ) * map->_layerSpriteCount + 1 );
		if( map->_layerSprites == NULL ) {
			prismaticLogger->error( "Could not allocate memory for layer sprites" );
			return;
		}

		LDtkLayer* layer = map->layers[i];
		LCDSprite* sprite = sprites->newSprite();
		sprites->setCenter( sprite, 0.0, 0.0 );
		sprites->setImage( sprite, layer->image, kBitmapUnflipped );
		sprites->setSize( sprite, map->width, map->height );
		sprites->moveTo( sprite, map->worldX, map->worldY );
		sprites->setZIndex( sprite, layer->zIndex );
		sprites->addSprite( sprite );

		map->_layerSprites[map->_layerSpriteCount - 1] = sprite;
		map->_layerSprites[map->_layerSpriteCount] = NULL;

	}

}

static void removeLDtkTileMap( LDtkTileMap* map ) {

}

static void addCollisionLDtkTileMap( LDtkTileMap* map ) {

}

static void removeCollisionLDtkTileMap( LDtkTileMap* map ) {

}


static void freeMapCollisions( LDtkTileMap* map ) {

	if( map->collision == NULL ) {
		return;
	}

	for( int i = 0; map->collision[i] != NULL; i++ ) {

		for( int j = 0; j < map->gridWidth; j++ ) {
			sys->realloc( map->collision[i]->collision[j], 0 );
			map->collision[i]->collision[j] = NULL;
		}
		
		for( int k = 0; map->collision[i]->rects[k] != NULL; k++ ) {
			sprites->freeSprite( map->collision[i]->rects[k] );
		}
		
		sys->realloc( map->collision[i]->rects, 0 );
		map->collision[i]->rects = NULL;

		sys->realloc( map->collision[i]->collision, 0 );
		map->collision[i]->collision = NULL;

		sys->realloc( map->collision[i], 0 );
		map->collision[i] = NULL;

	}
	
	sys->realloc( map->collision, 0 );
	map->collision = NULL;
	map->_collisionLayerCount = 0;
	
}

static void freeMapRefs( LDtkTileMap* map ) {

	if( map->_neighborCount <= 0 ) {
		return;
	}

	for( int i = 0; map->neighborLevels[i] != NULL; i++ ) {
		prismaticString->delete( map->neighborLevels[i]->levelIid );
		prismaticString->delete( map->neighborLevels[i]->dir );

		free( map->neighborLevels[i] );
		map->neighborLevels[i] = NULL;
	}

	sys->realloc( map->neighborLevels, 0 );
	map->neighborLevels = NULL;
	map->_neighborCount = 0;

}

static void freeMapLayers( LDtkTileMap* map ) {

	if( map->_layerCount <= 0 ) {
		return;
	}

	for( int i = 0; map->layers[i] != NULL; i++ ) {		
		freeLayer( map->layers[i] );
	}

	// if( map->_layerSprites != NULL ) {
	// 	for( int i = 0; map->_layerSprites[i] != NULL; i++ ) {
	// 	}
	// }

	sys->realloc( map->layers, 0 );
	map->layers = NULL;
	map->_layerCount = 0;

}

// MapManager

static LDtkTileMap* getMapByIid( string iid ) {
	return NULL;
}

static void changeMapByIid( string iid ) {

}

static void changeMapByName( string id ) {

}

static void changeMap( LDtkTileMap* map ) {

}


// Collisions

static void parseCollision( SDFile* file, LDtkTileMap* map ) {

	if( file == NULL || map == NULL ) {
		return;
	}

	int seekEndErr = pd->file->seek( file, 0, SEEK_END );
	if( seekEndErr == -1 ) {
		prismaticLogger->error( "Could not seek EOF for collision file" );
		return;
	}

	int fsize = pd->file->tell( file );
	if( fsize == -1 ) {
		prismaticLogger->error( "Could not determine file size for collision file" );
		return;
	}

	int seekSetErr = pd->file->seek( file, 0, SEEK_SET );
	if( seekSetErr == -1 ) {
		prismaticLogger->error( "Could not reset seek pointer in collision file" );
		return;
	}

	string rawCollisionData = sys->realloc( NULL, fsize + 1 );
	if( rawCollisionData == NULL ) {
		prismaticLogger->error( "Could not allocate memory for collision data" );
		return;
	}

	int readErr = pd->file->read( file, rawCollisionData, fsize );	
	if( readErr == -1 ) {
		prismaticLogger->error( "Could not read collision file" );
		sys->realloc( rawCollisionData, 0 );
		return;
	}

	stripNewlines( rawCollisionData );
	csvToCollision( rawCollisionData, map );

	sys->realloc( rawCollisionData, 0 );

}

static void stripNewlines( string str ) {

    string src = str;
    string dst = str;
    
    while( *src ) {
        if( *src != '\n' && *src != '\r' ) {
            *dst++ = *src;
        }

        src++;
    }
    
    *dst = '\0';

}

static void csvToCollision( string str, LDtkTileMap* map ) {
	
	// Memory allocation
	LDtkCollisionLayer* collisionLayer = calloc( 1, sizeof( LDtkCollisionLayer ) );

	map->_collisionLayerCount += 1;
	map->collision = sys->realloc( map->collision, sizeof( LDtkCollisionLayer* ) * map->_collisionLayerCount + 1 );

	collisionLayer->collision = sys->realloc( NULL, sizeof( int* ) * map->gridWidth );
	if( collisionLayer->collision == NULL ) {
		prismaticLogger->error( "Could not allocate memory for collision" );
		return;
	}

	for( int x = 0; x < map->gridWidth; x++ ) {
		collisionLayer->collision[x] = sys->realloc( NULL, sizeof( int ) * map->gridHeight );
		if( collisionLayer->collision[x] == NULL ) {
			// xxx: It is the caller's responsibility to call the map deletion function and free map->collision
			prismaticLogger->errorf( "Could not allocate memory for collisionLayer->collision[%d]", x );
			return;
		}
	}

	// // Build the array
	const char* ptr = str;
	int x = 0, y = 0;
	size_t collisionRects = 0;

    while( y < map->gridHeight ) {

		while( x < map->gridWidth ) {

			// Skip commas
			if( *ptr == ',' ) {
				*ptr++;
				continue;
			}

            collisionLayer->collision[x][y] = *ptr - '0';

			// Create the collision sprites
			if( collisionLayer->collision[x][y] == 1 ) {

				collisionRects++;
				collisionLayer->rects = sys->realloc( collisionLayer->rects, sizeof( LCDSprite* ) * collisionRects + 1 );
				if( collisionLayer->rects == NULL ) {
					prismaticLogger->error( "Could not allocate memory for collisionLayer->rects" );
					return;
				}

				LCDSprite* col = sprites->newSprite();
				float xf = (float)(x * map->tileSize);
				float yf = (float)(y * map->tileSize);

				PDRect r = (PDRect){
					.x = 0.0,
					.y = 0.0,
					.width = (float)(map->tileSize),
					.height = (float)(map->tileSize),
				};

				sprites->setCenter( col, 0.0, 0.0 );
				sprites->setCollisionsEnabled( col, 1 );
				sprites->setBounds( col, r );
				sprites->setCollideRect( col, r );
				sprites->setVisible( col, 0 );
				sprites->moveTo( col, xf, yf );

				collisionLayer->rects[collisionRects - 1] = col;
				collisionLayer->rects[collisionRects] = NULL;

			}

			*ptr++;
			x++;

        }		

		x = 0;
		y++;

    }

	map->collision[map->_collisionLayerCount - 1] = collisionLayer;
	map->collision[map->_collisionLayerCount] = NULL;

}

// JSON Parsing

static void decodeError( json_decoder* decoder, const char* error, int linenum ) {
	prismaticLogger->errorf( "Error decoding TileMap: '%s' at line %d", error, linenum );
}

static void willDecodeSublist( json_decoder* decoder, const char* name, json_value_type type ) {
	
	decoder->didDecodeArrayValue = didDecodeArrayValue;
	decoder->didDecodeTableValue = didDecodeTableValue;
	decoder->shouldDecodeArrayValueAtIndex = shouldDecodeArrayValueAtIndex;
	
	if( prismaticString->equals( "layers", name ) ) {
		decoder->didDecodeArrayValue = decodeLayers;
		return;
	}

	if( prismaticString->contains( "neighbourLevels", name ) ) {
		decoder->shouldDecodeArrayValueAtIndex = newNeighbor;
		decoder->didDecodeTableValue = decodeNeighbor;
		return;
	}

}

static int shouldDecodeTableValueForKey( json_decoder* decoder, const char* key ) { 
	
	LDtkTileMap* map = decoder->userdata;

	if( prismaticString->equals( "bgColor", key ) ) {
		return 0;
	}

	if( prismaticString->equals( "customFields", key ) ) {
		// TODO: Custom Field Parsing
		return 0;
	}

	if( prismaticString->equals( "entities", key ) ) {
		return 0;
	}

	return 1;

}

static void didDecodeTableValue( json_decoder* decoder, const char* key, json_value value ) {

	LDtkTileMap* map = decoder->userdata;

	if( prismaticString->equals( "identifier", key ) ) {
		map->id = prismaticString->new( json_stringValue( value ) );
		return;
	}

	// fixme: intentional typo - LDtk Exports this way, fix when LDtk fixes their json
	if( prismaticString->equals( "uniqueIdentifer", key ) ) {
		map->iid = prismaticString->new( json_stringValue( value ) );
		return;
	}

	if( prismaticString->equals( "x", key ) ) {
		map->worldX = json_intValue( value );
		return;
	}

	if( prismaticString->equals( "y", key ) ) {
		map->worldY = json_intValue( value );
		return;
	}

	if( prismaticString->equals( "width", key ) ) {
		map->width = json_intValue( value );
		return;
	}

	if( prismaticString->equals( "height", key ) ) {
		map->height = json_intValue( value );
		return;
	}

}

static int shouldDecodeArrayValueAtIndex( json_decoder* decoder, int pos ) { 
	return 1; 
}

static void didDecodeArrayValue( json_decoder* decoder, int pos, json_value value ) {

}

static void didDecodeSublist( json_decoder* decoder, const char* name, json_value_type type ) {

}

static void decodeLayers( json_decoder* decoder, int pos, json_value value ) {

	LDtkTileMap* map = decoder->userdata;
	LDtkLayer* layer = calloc( 1, sizeof( LDtkLayer ) );
	if( layer == NULL ) {
		prismaticLogger->error( "Could not allocate memory for new map layer" );
		return;
	}

	layer->filename = prismaticString->new( json_stringValue( value ) );

	string layerPath = prismaticString->new( map->_path );
	prismaticString->concat( &layerPath, "/" );
	prismaticString->concat( &layerPath, layer->filename );

	string err = NULL;
	layer->image = graphics->loadBitmap( layerPath, &err );

	if( err != NULL ) {
		prismaticLogger->errorf( "%s", err );
	}

	if( layer->image == NULL ) {
		prismaticLogger->errorf( "Layer image at %s could not be loaded!", layerPath );
	}

	layer->zIndex = pos;

	map->_layerCount += 1;
	map->layers = sys->realloc( map->layers, sizeof( LDtkLayer* ) * map->_layerCount + 1 );

	if( map->layers == NULL ) {
        prismaticLogger->errorf( "Memory allocation failed for adding layer: %s", layer->filename );
		prismaticString->delete( layerPath );
		freeLayer( layer );
        return;
    }
    
    map->layers[map->_layerCount - 1] = layer;
    map->layers[map->_layerCount] = NULL;

	prismaticString->delete( layerPath );

}

static int newNeighbor( json_decoder* decoder, int pos ) {
	
	LDtkTileMap* map = decoder->userdata;

	map->_neighborCount++;
	map->neighborLevels = sys->realloc( NULL, map->_neighborCount * sizeof( LDtkTileMapRef* ) + 1 );
	if( map->neighborLevels == NULL ) {
		prismaticLogger->error( "Could not allocate memory for neighborLevels!" );
		return 0;
	}

	map->neighborLevels[map->_neighborCount - 1] = calloc( 1, sizeof( LDtkTileMapRef ) );
	map->neighborLevels[map->_neighborCount] = NULL;
	
	return 1;

}

static void decodeNeighbor( json_decoder* decoder, const char* key, json_value value ) {

	LDtkTileMap* map = decoder->userdata;
	LDtkTileMapRef* neighbor = map->neighborLevels[map->_neighborCount - 1];

	if( prismaticString->equals( "levelIid", key ) ) {
		neighbor->levelIid = prismaticString->new( json_stringValue( value ) );
		return;
	}

	if( prismaticString->equals( "dir", key ) ) {
		neighbor->dir = prismaticString->new( json_stringValue( value ) );
		return;
	}

}

static int readfile( void* readud, uint8_t* buf, int bufsize ) {
	return pd->file->read( (SDFile*)readud, buf, bufsize );
}

static void freeLayer( LDtkLayer* layer ) {

	prismaticString->delete( layer->filename );

	if( layer->image != NULL ) {
		graphics->freeBitmap( layer->image );
	}

	free( layer );
	layer = NULL;

}

const LDtkTileMapFn* prismaticTileMap = &( LDtkTileMapFn ){
	.new = newLDtkTileMap,
	.delete = deleteLDtkTileMap,
	.draw = drawLDtkTileMap,
	.add = addLDtkTileMap,
};

const LDtkMapManagerFn* prismaticMapManager = &( LDtkMapManagerFn ){
	.getMapByIid = getMapByIid,
	.changeMapByIid = changeMapByIid,
	.changeMapByName = changeMapByName,
	.changeMap = changeMap,
};