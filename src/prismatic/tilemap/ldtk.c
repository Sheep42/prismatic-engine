#include <stdlib.h>
#include <stdio.h>

#include "../prismatic.h"
#include "ldtk.h"

static LDtkTileMap* newLDtkTileMap( string path );
static void deleteLDtkTileMap( LDtkTileMap* map );

static LDtkTileMap* getMapByIid( string iid );
static void changeMapByIid( string iid );
static void changeMapByName( string id );
static void changeMap( LDtkTileMap* map );

static void decodeError( json_decoder* decoder, const char* error, int linenum );
static void willDecodeSublist( json_decoder* decoder, const char* name, json_value_type type );
static int shouldDecodeTableValueForKey( json_decoder* decoder, const char* key ) ;
static void didDecodeTableValue( json_decoder* decoder, const char* key, json_value value );
static int shouldDecodeArrayValueAtIndex( json_decoder* decoder, int pos ) ;
static void didDecodeArrayValue( json_decoder* decoder, int pos, json_value value );
static void didDecodeSublist( json_decoder* decoder, const char* name, json_value_type type );
static void decodeLayers( json_decoder* decoder, int pos, json_value value );

static int readfile( void* readud, uint8_t* buf, int bufsize );

// TileMap

static LDtkTileMap* newLDtkTileMap( string path ) {

	const string dataFile = "/data.json";
	string trimmedPath = prismaticString->trimLast( path, '/' );

	string dataPath = prismaticString->new( trimmedPath );
	dataPath = sys->realloc( dataPath, strlen( trimmedPath ) + strlen( dataFile ) + 1 );
	strcat( dataPath, dataFile );

	LDtkTileMap* map = calloc( 1, sizeof( LDtkTileMap ) );
	json_reader* mapReader = calloc( 1, sizeof( json_reader ) );
	json_decoder* mapDecoder = calloc( 1, sizeof( json_decoder ) );
	SDFile* file = pd->file->open( dataPath, kFileRead );

	map->_path = trimmedPath;
	map->_layerCount = 0;
	
	mapReader->read = readfile;
	mapReader->userdata = file;

	mapDecoder->decodeError = decodeError;
	mapDecoder->shouldDecodeTableValueForKey = shouldDecodeTableValueForKey;
	mapDecoder->didDecodeTableValue = didDecodeTableValue;
	mapDecoder->willDecodeSublist = willDecodeSublist;
	mapDecoder->shouldDecodeArrayValueAtIndex = shouldDecodeArrayValueAtIndex;
	mapDecoder->didDecodeArrayValue = didDecodeArrayValue;
	mapDecoder->didDecodeSublist = didDecodeSublist;
	mapDecoder->userdata = map;

	pd->json->decode( mapDecoder, *mapReader, NULL );

	// TODO: remove
	// for( size_t i = 0; map->layers[i] != NULL; i++ ) {
	// 	LDtkLayer* layer = map->layers[i];
	// 	printf( "%s", layer->filename );
	// 	graphics->drawBitmap( layer->image, 0, 0, kBitmapUnflipped );
	// }

	// Free allocated memory
	prismaticString->delete( trimmedPath );
	prismaticString->delete( dataPath );
	sys->realloc( mapReader, 0 );
	sys->realloc( mapDecoder, 0 );

	return map;

}

static void deleteLDtkTileMap( LDtkTileMap* map ) {

	// Since our strings are duplicated, we need to explicitly free them
	sys->realloc( map->id, 0 );
	sys->realloc( map->iid, 0 );

	sys->realloc( map, 0 );

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


// JSON Parsing

static void decodeError( json_decoder* decoder, const char* error, int linenum ) {
	prismaticLogger->errorf( "Error decoding TileMap: '%s' at line %d", error, linenum );
}

static void willDecodeSublist( json_decoder* decoder, const char* name, json_value_type type ) {
	
	if( prismaticString->equals( "layers", name ) ) {
		decoder->didDecodeArrayValue = decodeLayers;
		return;
	}

	decoder->didDecodeArrayValue = didDecodeArrayValue;

}

static int shouldDecodeTableValueForKey( json_decoder* decoder, const char* key ) { 
	
	LDtkTileMap* map = decoder->userdata;

	if( prismaticString->equals( "bgColor", key ) ) {
		return 0;
	}

	if( prismaticString->equals( "neighbourLevels", key ) ) {
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

	string fileName = prismaticString->new( json_stringValue( value ) );

	string layerPath = prismaticString->new( map->_path );
	prismaticString->concat( &layerPath, "/" );
	prismaticString->concat( &layerPath, fileName );

	LDtkLayer* layer = calloc( 1, sizeof( LDtkLayer ) );
	layer->filename = fileName;

	// TODO: Error handling
	layer->image = graphics->loadBitmap( layerPath, NULL );
	if( layer->image == NULL ) {
		prismaticLogger->errorf( "Layer image at %s could not be loaded!", layerPath );
	}

	layer->zIndex = pos;

	map->_layerCount += 1;
	map->layers = sys->realloc( map->layers, sizeof( LDtkLayer* ) * map->_layerCount + 1 );

	if( map->layers == NULL ) {
        prismaticLogger->errorf( "Memory allocation failed for adding layer: %s", fileName );
        return;
    }
    
    map->layers[map->_layerCount - 1] = layer;
    map->layers[map->_layerCount] = NULL;

	sys->realloc( fileName, 0 );
	sys->realloc( layerPath, 0 );

}

static int readfile( void* readud, uint8_t* buf, int bufsize ) {
	return pd->file->read( (SDFile*)readud, buf, bufsize );
}

const LDtkTileMapFn* prismaticTileMap = &( LDtkTileMapFn ){
	.new = newLDtkTileMap,
	.delete = deleteLDtkTileMap,
};

const LDtkMapManagerFn* prismaticMapManager = &( LDtkMapManagerFn ){
	.getMapByIid = getMapByIid,
	.changeMapByIid = changeMapByIid,
	.changeMapByName = changeMapByName,
	.changeMap = changeMap,
};