#ifndef LDTK_H
#define LDTK_H

#ifndef PRISMATIC_INCLUDED
	#define PRISMATIC_INCLUDED
	#include "../../prismatic/prismatic.h"
#endif

typedef struct LDtkTileMap {
	string id;
	string iid;
	int worldX;
	int worldY;
	int width;
	int height;	
	LDtkTileMapRef* neighborLevels;
	LDtkLayer* layers;
	LCDBitmap* composite;
	void ( *enter )( void );
	void ( *exit )( void );
} LDtkTileMap;

typedef struct LDtkLayer {
	string filename;
	int zIndex;
	LCDBitmap* image;
} LDtkLayer;

typedef struct LDtkEntity {
	string id;
	string iid;
	string layer;
	int x;
	int y;
	int zIndex;
	int width;
	int height;
} LDtkEntity;

typedef struct LDtkTileMapRef {
	string levelIid;
	string dir;
} LDtkTileMapRef;

typedef struct LDtkMapManager {
	LDtkTileMap** maps;
	LDtkTileMap* currentMap;
	LDtkTileMap* previousMap;
} LDtkMapManager;

typedef struct LDtkTileMapFn {
	// Create a new LDtkTileMap from the path
	//
	// The path should point to an LDtk simple export JSON file in 
	// a directory with all of its assets 
	//
	// ----
	// 
	// string path
	LDtkTileMap* ( *newLDtkTileMap )( string );

	// Delete the LDtkTileMap
	//
	// ----
	//
	// LDtkTileMap* map
	void ( *deleteLDtkTileMap )( LDtkTileMap* );
} LDtkTileMapFn;

typedef struct LDtkMapManagerFn {
	// Get a map from the Map Manager by its Iid
	//
	// ----
	//
	// string Iid - The map's Iid
	LDtkTileMap* ( *getMapByIid )( string );

	// Change the current map to the map with the specified Iid
	//
	// ----
	//
	// string iid - The map's Iid
	void ( *changeMapByIid )( string );

	// Change the current map to the map with the specified Identifier
	//
	// ----
	//
	// string id - The map's identifier
	void ( *changeMapByName )( string );

	// Change the current map to the specified map
	//
	// ----
	//
	// LDtkTileMap* map
	void ( *changeMap )( LDtkTileMap* );
} LDtkMapManagerFn;

extern const LDtkTileMapFn* prismaticTileMap;
extern const LDtkMapManagerFn* prismaticMapManager;

#endif