#ifndef LDTK_H
#define LDTK_H

#ifndef PD_API_INCLUDED
	#define PD_API_INCLUDED
	#include "pd_api.h"
#endif

#ifndef TEXT_INCLUDED
	#define TEXT_INCLUDED
	#include "../text/text.h"
#endif

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

typedef struct LDtkCollisionLayer {
	int** collision;
	LCDSprite** rects;
} LDtkCollisionLayer;

typedef struct LDtkTileMap {
	string id;
	string iid;
	int worldX;
	int worldY;
	int width;
	int height;
	int tileSize;
	int gridHeight;
	int gridWidth;
	size_t _neighborCount;
	size_t _collisionLayerCount;
	LDtkCollisionLayer** collision;
	LDtkTileMapRef** neighborLevels;
	LDtkLayer** layers;
	LDtkEntity** entities;
	size_t _layerCount;
	string _path;
	void ( *enter )( void );
	void ( *exit )( void );
} LDtkTileMap;

typedef struct LDtkMapManager {
	LDtkTileMap** maps;
	LDtkTileMap* currentMap;
	LDtkTileMap* previousMap;
} LDtkMapManager;

typedef struct LDtkTileMapFn {
	// Create a new LDtkTileMap from the path
	//
	// The path should point to an LDtk simple export directory 
	// containing all of its assets 
	//
	// ----
	// 
	// string path - The path to the directory containing the LDtk Level export 
	//
	// int tileSize - The size, in pixels, of a single tile 
	//
	// string* collistionLayers - A list of csv files containing collision
	// information. Pass NULL for no collision layer. Must be NULL terminated 
	LDtkTileMap* ( *new )( string, int, string* );

	// Delete the LDtkTileMap
	//
	// ----
	//
	// LDtkTileMap* map
	void ( *delete )( LDtkTileMap* );
} LDtkTileMapFn;

typedef struct LDtkMapManagerFn {
	// Add a map to the Map Manager
	//
	// ----
	// 
	// LDtkMapManager* mapManager
	//
	// LDtkTileMap* map
	void ( *add )( LDtkMapManager*, LDtkTileMap* );

	// Remove a map from the Map Manager
	// 
	// Does not free the map, it is up to the caller to free the map from
	// memory
	//
	// ----
	// 
	// LDtkMapManager* mapManager
	//
	// string id - The map's Id or Iid
	void ( *remove )( LDtkMapManager*, string );

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

	// Destroy the map manager and free all of its maps
	void ( *destroy )( void );
} LDtkMapManagerFn;

extern const LDtkTileMapFn* prismaticTileMap;
extern const LDtkMapManagerFn* prismaticMapManager;

#endif