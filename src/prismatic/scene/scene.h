#ifndef SCENE_H
#define SCENE_H

typedef struct Scene {
	const char* name;
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
	Scene* ( *new )( const char* );
	void ( *delete )( Scene* );
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