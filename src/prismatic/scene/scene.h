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
	void ( *destroy )( void );
} Scene;

typedef struct SceneManager {
	Scene** scenes;
	int totalScenes;
	Scene* defaultScene;
	Scene* currentScene;
	Scene* previousScene;
} SceneManager; 

typedef struct SceneFn {
	Scene* ( *new )( string );
	void ( *delete )( Scene* );
	void ( *add )( struct Scene*, string, PrismSprite* );
	void ( *remove )( struct Scene*, string );
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

extern const SceneFn* prismaticScene;
extern const SceneManagerFn* prismaticSceneManager;

#endif // SCENE_H