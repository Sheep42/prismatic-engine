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

// TileMap

static LDtkTileMap* newLDtkTileMap( string path ) {

	return NULL;

}

static void deleteLDtkTileMap( LDtkTileMap* map ) {


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