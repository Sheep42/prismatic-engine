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
static int readfile( void* readud, uint8_t* buf, int bufsize );

// TileMap

static LDtkTileMap* newLDtkTileMap( string path ) {

	LDtkTileMap* map = calloc( 1, sizeof( LDtkTileMap ) );
	json_reader* mapReader = calloc( 1, sizeof( json_reader ) );
	json_decoder* mapDecoder = calloc( 1, sizeof( json_decoder ) );
	SDFile* file = pd->file->open( path, kFileRead );
	
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

	free( mapReader );
	free( mapDecoder );

	return map;

}

static void deleteLDtkTileMap( LDtkTileMap* map ) {

	free( map );

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
		return 0;
	}

	if( prismaticString->equals( "layers", key ) ) {
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
	return 0; 
}

static void didDecodeArrayValue( json_decoder* decoder, int pos, json_value value ) {

}

static void didDecodeSublist( json_decoder* decoder, const char* name, json_value_type type ) {

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