typedef struct Scene {
	const char* name;
	void ( *enter )( void );
	void ( *update )( float );
	void ( *draw )( float );
	void ( *exit )( void );
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