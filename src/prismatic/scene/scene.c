#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../prismatic.h"

// Scene Machine

static SceneManager* new( Scene* );
static void delete( SceneManager* );
static void update( SceneManager*, float );
static Scene* changeSceneByName( SceneManager* sceneManager, char* name );
static Scene* changeScene( SceneManager* sceneManager, Scene* scene );
static Scene* changeToPrevious( SceneManager* sceneManager );
static Scene* changeToDefault( SceneManager* sceneManager );
static void addScene( SceneManager* sceneManager, Scene* scene );

static SceneManager* new( Scene* defaultScene ) {
	
	SceneManager* sceneManager = calloc( 1, sizeof(SceneManager) );
	if( sceneManager == NULL ) {
		prismaticLogger->error( "Could not allocate memory for new scene manager" );
		return NULL;
	}

	if( defaultScene != NULL ) {
		sceneManager->defaultScene = defaultScene;
		addScene( sceneManager, defaultScene );
		changeScene( sceneManager, defaultScene );
	}

	return sceneManager;

}

static void delete( SceneManager* sceneManager ) {
	
	if( sceneManager == NULL )
		return;

	for( size_t i = 0; i < sceneManager->totalScenes; i++ ) {
		Scene* s = sceneManager->scenes[i];
		s->destroy();
	}

	free( sceneManager->scenes );
	free( sceneManager );

}

static void update( SceneManager* sceneManager, float delta ) {

	if( sceneManager->currentScene == NULL ) {
		return;
	}

	if( sceneManager->currentScene->update == NULL ) {
		prismaticLogger->errorf( "Scene '%s' has NULL update function", sceneManager->currentScene->name );
		return;
	}

	sceneManager->currentScene->update( delta );

}

static void draw( SceneManager* sceneManager, float delta ) {

	if( sceneManager->currentScene == NULL ) {
		return;
	}

	if( sceneManager->currentScene->draw == NULL ) {
		prismaticLogger->errorf( "Scene '%s' has NULL draw function", sceneManager->currentScene->name );
		return;
	}

	sceneManager->currentScene->draw( delta );

}

static Scene* changeSceneByName( SceneManager* sceneManager, char* name ) {

	Scene* scene = NULL;

	for( size_t i = 0; i < sceneManager->totalScenes; i++ ) {
		Scene* s = sceneManager->scenes[i];
		if( strcmp( s->name, name ) == 0 ) {
			scene = changeScene( sceneManager, s );
		}
	}

	return scene;

}

static Scene* changeScene( SceneManager* sceneManager, Scene* scene ) {

	if( sceneManager == NULL ) {
		prismaticLogger->error( "sceneManager cannot be NULL" );
		return NULL;
	}

	if( scene == NULL ) {
		prismaticLogger->error( "scene cannot be NULL" );
		return NULL;
	}

	if( sceneManager->currentScene != NULL ) {
		
		if( sceneManager->currentScene->exit != NULL ) {
			sceneManager->currentScene->exit();
		}

		sceneManager->previousScene = sceneManager->currentScene;

	}

	sceneManager->currentScene = scene;

	if( sceneManager->currentScene->enter != NULL ) {
		sceneManager->currentScene->enter();
	}

	return sceneManager->currentScene;

}

static Scene* changeToPrevious( SceneManager* sceneManager ) {
	
	if( sceneManager->previousScene == NULL ) {
		return prismaticSceneManager->changeToDefault( sceneManager );
	}

	return prismaticSceneManager->changeScene( sceneManager, sceneManager->previousScene );

}

static Scene* changeToDefault( SceneManager* sceneManager ) {
	return prismaticSceneManager->changeScene( sceneManager, sceneManager->defaultScene );
}

static void addScene( SceneManager* sceneManager, Scene* scene ) {

	if( scene == NULL ) {
		prismaticLogger->info( "Not adding NULL scene to scene machine" );
		return;
	}

	sceneManager->totalScenes += 1;
	sceneManager->scenes = sys->realloc( sceneManager->scenes, sceneManager->totalScenes * sizeof(Scene) );

	if (sceneManager->scenes == NULL) {
        prismaticLogger->error( "Memory allocation failed for adding scene.\n" );
        return;
    }
    
    sceneManager->scenes[sceneManager->totalScenes - 1] = scene;

}

const SceneManagerFn* prismaticSceneManager = &(SceneManagerFn){
	.new = new,
	.delete = delete,
	.update = update,
	.draw = draw,
	.changeSceneByName = changeSceneByName,
	.changeScene = changeScene,
	.changeToPrevious = changeToPrevious,
	.changeToDefault = changeToDefault,
	.addScene = addScene,
}; 

// Scene

static Scene* newScene( const char* name ) {

	if( name == NULL ) {
		prismaticLogger->error( "Cannot create new scene with NULL name" );
		return NULL;
	}

	Scene* scene = calloc( 1, sizeof(Scene) );
	if( scene == NULL ) {
		prismaticLogger->error( "Could not allocate memory for new scene" );
		return NULL;
	}

	scene->name = strdup( name );

	return scene;

}

static void deleteScene( Scene* scene ) {

	if( scene->destroy != NULL ) {
		scene->destroy();
	}

	free( scene );

}

const SceneFn* prismaticScene = &(SceneFn) {
	.new = newScene,
	.delete = deleteScene,
};