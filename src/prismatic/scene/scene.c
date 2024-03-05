#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../prismatic.h"
#include "scene.h"

// Scene Manager

static SceneManager* new( Scene* );
static void delete( SceneManager* );
static void update( SceneManager*, float );
static Scene* changeSceneByName( SceneManager* sceneManager, string name );
static Scene* changeScene( SceneManager* sceneManager, Scene* scene );
static Scene* changeToPrevious( SceneManager* sceneManager );
static Scene* changeToDefault( SceneManager* sceneManager );
static void addScene( SceneManager* sceneManager, Scene* scene );
static void deleteScene( Scene* scene );
static void addSprite( Scene* scene, string id, PrismSprite* sp );

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
		deleteScene( s );
	}

	sceneManager->scenes = sys->realloc( sceneManager->scenes, 0 );
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

	// Run the Scene's update method
	sceneManager->currentScene->update( sceneManager->currentScene, delta );

	// Run the update method for each of the Scene's sprites
	for( size_t i = 0; i < sceneManager->currentScene->totalSprites; i++ ) {
		
		PrismSprite* sp = sceneManager->currentScene->sprites[i];

		if( sp->update == NULL ) {
			continue;
		}

		sp->update( sp, delta );

	}

}

static void draw( SceneManager* sceneManager, float delta ) {

	if( sceneManager->currentScene == NULL ) {
		return;
	}

	if( sceneManager->currentScene->draw == NULL ) {
		prismaticLogger->errorf( "Scene '%s' has NULL draw function", sceneManager->currentScene->name );
		return;
	}

	sceneManager->currentScene->draw( sceneManager->currentScene, delta );

}

static Scene* changeSceneByName( SceneManager* sceneManager, string name ) {

	Scene* scene = NULL;

	for( size_t i = 0; i < sceneManager->totalScenes; i++ ) {
		Scene* s = sceneManager->scenes[i];
		if( strcmp( s->name, name ) == 0 ) {
			scene = changeScene( sceneManager, s );
			break;
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
			sceneManager->currentScene->exit( sceneManager->currentScene );
		}

		sceneManager->previousScene = sceneManager->currentScene;

	}

	sceneManager->currentScene = scene;

	if( sceneManager->currentScene->enter != NULL ) {
		sceneManager->currentScene->enter( sceneManager->currentScene );
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

	sceneManager->totalScenes++;
	sceneManager->scenes = sys->realloc( sceneManager->scenes, sceneManager->totalScenes * sizeof(Scene) + 1 );

	if (sceneManager->scenes == NULL) {
        prismaticLogger->error( "Memory allocation failed for adding scene.\n" );
        return;
    }
    
    sceneManager->scenes[sceneManager->totalScenes - 1] = scene;
    sceneManager->scenes[sceneManager->totalScenes] = NULL;

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

// Creates a new Scene with the given name
static Scene* newScene( string name ) {

	if( name == NULL ) {
		prismaticLogger->error( "Cannot create new scene with NULL name" );
		return NULL;
	}

	Scene* scene = calloc( 1, sizeof(Scene) );
	if( scene == NULL ) {
		prismaticLogger->error( "Could not allocate memory for new scene" );
		return NULL;
	}

	scene->name = name;

	return scene;

}

// Deletes the Scene and performs clean up operations
//  
// First, scene->destroy() is called, if it is set. Next, all sprites are 
// destroyed with prismaticSprite->delete() and the sprites pointer is freed. Finally, the Scene is freed.
static void deleteScene( Scene* scene ) {

	if( scene->destroy != NULL ) {
		scene->destroy( scene );
	}

	for( size_t i = 0; scene->sprites[i] != NULL; i++ ) {
		prismaticSprite->delete( scene->sprites[i] );
		prismaticScene->remove( scene, scene->sprites[i] );
	}

	scene->sprites = sys->realloc( scene->sprites, 0 );
	free( scene );

}

// Adds a Sprite to the Scene. spriteId should be a unique identifier within 
// this Scene.
static void addSprite( Scene* scene, string spriteId, PrismSprite* sp ) {

	if( scene == NULL ) {
		prismaticLogger->info( "Cannot add Sprite to NULL Scene" );
		return;
	}

	if( spriteId == NULL ) {
		prismaticLogger->info( "Cannot add sprite without id" );
		return;
	}

	if( sp == NULL ) {
		prismaticLogger->info( "Cannot add NULL Sprite to Scene" );
		return;
	}

	scene->totalSprites += 1;
	scene->sprites = sys->realloc( scene->sprites, scene->totalSprites * sizeof(PrismSprite*) + 1 );

	if( scene->sprites == NULL ) {
        prismaticLogger->error( "Memory allocation failed for adding sprite." );
        return;
    }
    
    sp->id = spriteId;
    scene->sprites[scene->totalSprites - 1] = sp;
    scene->sprites[scene->totalSprites] = NULL;

    sprite->addSprite( sp->sprite );

}

// Removes the Sprite from the Scene. Does not delete the Sprite. The caller
// is responsible for calling prismaticSprite->delete() when a Sprite is 
// removed in this way.
static void removeSprite( Scene* scene, PrismSprite* sp ) {

	if( scene == NULL ) {
		prismaticLogger->info( "Cannot remove Sprite from NULL Scene" );
		return;
	}

	if( sp == NULL ) {
		prismaticLogger->info( "Cannot remove NULL Sprite from Scene" );
		return;
	}

	size_t i = 0;
	for( i = 0; scene->sprites[i] != NULL; i++ ) {
		
		if( sp->id != scene->sprites[i]->id ) {
			sprite->removeSprite( scene->sprites[i]->sprite );
			continue;
		}

		break;

	}

    if( scene->sprites[i] == NULL ) {
        prismaticLogger->errorf( "Did not find Sprite %d in Scene!", sp->id );
        return;
    }

    // Shift sprites to remove Sprite at i 
    for( size_t j = i; scene->sprites[j] != NULL; j++ ) {
        scene->sprites[j] = scene->sprites[j + 1];
    }

    scene->totalSprites--;

}

const SceneFn* prismaticScene = &(SceneFn) {
	.new = newScene,
	.delete = deleteScene,
	.add = addSprite,
	.remove = removeSprite,
};