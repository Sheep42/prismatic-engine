#ifndef LDTK_H
#define LDTK_H

#include <stddef.h>
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

typedef struct LDtkEntityGroup {
	string type;
	size_t _entityCount;
	LDtkEntity** entities;
} LDtkEntityGroup;

typedef struct LDtkTileMapRef {
	string levelIid;
	string dir;
} LDtkTileMapRef;

typedef struct LDtkCollisionLayer {
	string name;
	int** collision;
	LCDSprite** rects;
} LDtkCollisionLayer;

typedef struct LDtkFieldHandler {
	// Used for handling custom fields during map decoding
	int ( *decodeFields )( json_decoder* decoder, const char* key );
} LDtkFieldHandler;

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
	size_t _collisionLayerCount;
	LDtkCollisionLayer** collision;
	size_t _neighborCount;
	LDtkTileMapRef** neighborLevels;
	size_t _layerCount;
	LDtkLayer** layers;
	size_t _entityGroupCount;
	LDtkEntityGroup** entities;
	size_t _layerSpriteCount;
	LCDSprite** _layerSprites;
	string _path;
	// Used for handling custom fields during map decoding, caller is responsible
	// for freeing the pointer.
	LDtkFieldHandler* _customFieldHandler;

	// Optional callback for when the map is set as current in the MapManager
	//
	// ---
	//
	// LDtkTileMap* self
	void ( *enter )( struct LDtkTileMap* );

	// Optional callback for when the map transitions away from current in the 
	// MapManager
	//
	// ---
	//
	// LDtkTileMap* self
	void ( *exit )( struct LDtkTileMap* );
} LDtkTileMap;

typedef struct LDtkMapManager {
	size_t _mapCount;
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
	// string* collisionLayers - A list of csv files containing collision
	// information. Pass NULL for no collision layer. Must be NULL terminated 
	// 
	// LDtkFieldHandler* customFieldHandler 
	LDtkTileMap* ( *new )( string, int, string*, LDtkFieldHandler* );

	// Delete the LDtkTileMap
	//
	// ----
	//
	// LDtkTileMap* map
	void ( *delete )( LDtkTileMap* );

	// Draw the map's layers to screen as raw LCDBitmap*s 
	//
	// This is a convenience function, you may choose to draw the map yourself
	// if doing so is preferable.
	//
	// ---
	//
	// LDtkTileMap* map
	void ( *draw )( LDtkTileMap* );

	// Draw the map's layers to screen as Sprites
	//
	// This is a convenience function, you may choose to draw the map yourself
	// if doing so is preferable.
	//
	// ---
	//
	// LDtkTileMap* map
	void ( *add )( LDtkTileMap* );

	// Remove the map's layers from the screen.
	//
	// This is a convenience function, you may choose to draw the map yourself
	// if doing so is preferable.
	//
	// ---
	//
	// LDtkTileMap* map
	void ( *remove )( LDtkTileMap* );
	
	// Add the map's collision layers to the screen.
	//
	// This is a convenience function, you may choose to manage the collision 
	// yourself if doing so is preferable.
	//
	// ---
	//
	// LDtkTileMap* map
	void ( *addCollision )( LDtkTileMap* );

	// Remove the map's collision layers from the screen.
	//
	// This is a convenience function, you may choose to manage the collision 
	// yourself if doing so is preferable.
	//
	// ---
	//
	// LDtkTileMap* map
	void ( *removeCollision )( LDtkTileMap* );

	// Tag a the Sprites in a map's collision layer
	//
	// ---
	//
	// LDtkTileMap* map
	//
	// string layerName
	//
	// uint8_t tag 
	void ( *tagCollision )( LDtkTileMap*, string, uint8_t );
} LDtkTileMapFn;

typedef struct LDtkMapManagerFn {
	// Create a new MapManager
	LDtkMapManager* ( *new )( void );

	// Delete a MapManager
	//
	// Does not free the MapManager's maps, it is up to the caller to call 
	// prismaticTileMap->delete on their maps.
	// ---
	//
	// LDtkMapManager* mapManager
	void ( *delete )( LDtkMapManager* );

	// Add a map to the Map Manager, if it is not already added.
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
	// memory.
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
	// LDtkMapManager* mapManager
	//
	// string id - The map's Id or Iid
	LDtkTileMap* ( *getMap )( LDtkMapManager*, string );

	// Change the current map to the map with the specified Iid
	//
	// ----
	//
	// LDtkMapManager* mapManager
	//
	// string iid - The map's Iid
	void ( *changeMapByIid )( LDtkMapManager*, string );

	// Change the current map to the map with the specified Identifier
	//
	// ----
	//
	// LDtkMapManager* mapManager
	//
	// string id - The map's identifier
	void ( *changeMapByName )( LDtkMapManager*, string );

	// Change the current map to the specified map. Adds the map to the MapManager
	// if it is not already added. Can be used as a shorthand to add & set current Map.
	//
	// ----
	//
	// LDtkMapManager* mapManager
	//
	// LDtkTileMap* map
	void ( *changeMap )( LDtkMapManager*, LDtkTileMap* );
} LDtkMapManagerFn;

extern const LDtkTileMapFn* prismaticTileMap;
extern const LDtkMapManagerFn* prismaticMapManager;

#endif
