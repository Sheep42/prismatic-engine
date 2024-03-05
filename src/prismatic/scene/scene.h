#ifndef SCENE_H
	
#define SCENE_H

#ifndef STDDEF_INCLUDED
	#define STDDEF_INCLUDED
	#include <stddef.h>
#endif

#ifndef SPRITE_INCLUDED
	#define SPRITE_INCLUDED
	#include "../sprite/sprite.h"
#endif

#ifndef TEXT_INCLUDED
	#define TEXT_INCLUDED
	#include "../sprite/sprite.h"
#endif

typedef struct Scene {
	string name;
	PrismSprite** sprites;
	size_t totalSprites;
	void ( *enter )( struct Scene* );
	void ( *update )( struct Scene*, float );
	void ( *draw )( struct Scene*, float );
	void ( *exit )( struct Scene* );
	void ( *destroy )( struct Scene* );
} Scene;

typedef struct SceneManager {
	Scene** scenes;
	int totalScenes;
	Scene* defaultScene;
	Scene* currentScene;
	Scene* previousScene;
} SceneManager; 

typedef struct SceneFn {
	// Creates a new Scene with the given name
	// 
	// string name - A unique identifier for the Scene in the SceneManager
	Scene* ( *new )( string );

	// Deletes the Scene
	// 
	// Scene* scene
	void ( *delete )( Scene* );

	// Add the Sprite to the Scene. 
	// 
	// Scene* scene
	// 
	// string spriteId - A unique identifier for sprite
	// 
	// PrismSprite* sprite
	void ( *add )( struct Scene*, string, PrismSprite* );

	// Remove the Sprite from the Scene
	// 
	// Scene* scene 
	// 
	// PrismSprite* sprite
	void ( *remove )( struct Scene*, PrismSprite* );
} SceneFn;

typedef struct SceneManagerFn {
	SceneManager* (*new)( Scene* );
	void ( *delete )( SceneManager* );
	void ( *update )( SceneManager*, float );
	void ( *draw )( SceneManager*, float );
	Scene* ( *changeSceneByName )( SceneManager*, char* );
	Scene* ( *changeScene )( SceneManager*, Scene* );
	Scene* ( *changeToPrevious )( SceneManager* );
	Scene* ( *changeToDefault )( SceneManager* );
	void ( *addScene )( SceneManager*, Scene* );
} SceneManagerFn;

// The Prismatic Engine Scene Global
extern const SceneFn* prismaticScene;

// The Prismatic Engine SceneManager Global
extern const SceneManagerFn* prismaticSceneManager;

#endif // SCENE_H