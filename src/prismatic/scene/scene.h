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
	bool isActive;
	struct SceneManager* sceneManager;
	void* ref;
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
	void (*destroy)( struct SceneManager* );
} SceneManager; 

typedef struct SceneFn {
	// Creates a new Scene with the given name
	// 
	// ----
	// 
	// string name - A unique identifier for the Scene in the SceneManager
	Scene* ( *new )( string );

	// Deletes the Scene
	// 
	// ----
	// 
	// Scene* scene
	void ( *delete )( Scene* );

	// Add the Sprite to the Scene. 
	// 
	// ----
	// 
	// Scene* scene
	// 
	// string spriteId - A unique identifier for sprite
	// 
	// PrismSprite* sprite
	void ( *add )( struct Scene*, string, PrismSprite* );

	// Remove the Sprite from the Scene
	// 
	// ----
	// 
	// Scene* scene 
	// 
	// PrismSprite* sprite
	void ( *remove )( struct Scene*, PrismSprite* );

	// Get a Sprite from the Scene by its string ID
	// 
	// ----
	// 
	// Scene* scene
	// 
	// string spriteId
	PrismSprite* ( *get )( struct Scene*, string );
} SceneFn;

typedef struct SceneManagerFn {
	// Creates a new SceneManager
	// 
	// ----
	// 
	// Scene defaultScene - The Default Scene for the SceneManager
	SceneManager* (*new)( Scene* );

	// Deletes a SceneManager. 
	// 
	// Calls prismaticScene->delete() for each Scene in the SceneManager. 
	// Then frees sceneManager->scenes. Finally, frees the SceneManager
	// itself.
	// 
	// ----
	// 
	// SceneManager* sceneManager
	void ( *delete )( SceneManager* );

	// Updates the SceneManager. 
	// 
	// First, calls scene->update() for the SceneManager's current Scene. 
	// Then, calls sprite->update() for each Sprite in the Scene.
	// 
	// ----
	// 
	// SceneManager* sceneManager
	// float delta
	void ( *update )( SceneManager*, float );

	// Draws the SceneManager. 
	// 
	// Calls scene->draw() for the SceneManager's current Scene.
	// 
	// ----
	// 
	// SceneManager* sceneManager
	// float delta
	void ( *draw )( SceneManager*, float );

	// Change the current Scene to the named Scene.
	// 
	// ----
	// 
	// SceneManager* sceneManager
	// 
	// string sceneName - The name of the Scene to change to
	Scene* ( *changeSceneByName )( SceneManager*, string );

	// Change the current Scene to the given Scene.
	// 
	// ----
	// 
	// SceneManager* sceneManager
	// 
	// Scene* newScene
	Scene* ( *changeScene )( SceneManager*, Scene* );

	// Change the current Scene to the previous Scene.
	// 
	// SceneManager* sceneManager
	Scene* ( *changeToPrevious )( SceneManager* );

	// Change the current Scene to the default Scene.
	// 
	// ----
	// 
	// SceneManager* sceneManager
	Scene* ( *changeToDefault )( SceneManager* );

	// Add a Scene to the SceneManager
	// 
	// ----
	// 
	// SceneManager* sceneManager
	// 
	// Scene* scene
	void ( *add )( SceneManager*, Scene* );

	// Remove a Scene from the SceneManager
	// 
	// Does not destroy the Scene upon removal. If a Scene is removed from a 
	// SceneManager before the SceneManager is destroyed, the caller is 
	// responsible for destroying the Scene.
	// 
	// ----
	// 
	// SceneManager* sceneManager
	// 
	// Scene* scene
	void ( *remove )( SceneManager*, Scene* );

	// Remove a Scene from the SceneManager
	// 
	// Does not destroy the Scene upon removal. If a Scene is removed from a 
	// SceneManager before the SceneManager is destroyed, the caller is 
	// responsible for destroying the Scene.
	// 
	// ----
	// 
	// SceneManager* sceneManager
	// 
	// string sceneName - The Scene's name
	void ( *removeByName )( SceneManager*, string );

	// Get a Scene by its name from the SceneManager
	// 
	// ----
	// 
	// SceneManager* sceneManager
	// 
	// string sceneName - The Scene's name
	Scene* ( *get )( SceneManager*, string );
} SceneManagerFn;

// The Prismatic Engine Scene Global
extern const SceneFn* prismaticScene;

// The Prismatic Engine SceneManager Global
extern const SceneManagerFn* prismaticSceneManager;

#endif // SCENE_H