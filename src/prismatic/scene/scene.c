#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../prismatic.h"
#include "scene.h"

// Scene Manager

static SceneManager* newSceneManager( Scene* );
static void deleteSceneManager( SceneManager* );
static void updateSceneManager( SceneManager*, float );
static void drawSceneManager( SceneManager*, float );
static Scene* changeSceneByName( SceneManager* sceneManager, string name );
static Scene* changeScene( SceneManager* sceneManager, Scene* scene );
static Scene* changeToPrevious( SceneManager* sceneManager );
static Scene* changeToDefault( SceneManager* sceneManager );
static void addScene( SceneManager* sceneManager, Scene* scene );
static void removeScene( SceneManager* sceneManager, Scene* scene );
static void removeSceneByName( SceneManager* sceneManager, string sceneName );
static Scene* getScene( SceneManager* sceneManager, string sceneName );
static void deleteScene( Scene* scene );
static void addSprite( Scene* scene, string id, PrismSprite* sp );

static SceneManager* newSceneManager( Scene* defaultScene ) {
	
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

static void deleteSceneManager( SceneManager* sceneManager ) {
	
	if( sceneManager == NULL )
		return;

	if( sceneManager->destroy != NULL ) {
		sceneManager->destroy( sceneManager );
	}

	for( size_t i = 0; i < sceneManager->totalScenes; i++ ) {
		Scene* s = sceneManager->scenes[i];
		deleteScene( s );
	}

	sceneManager->scenes = sys->realloc( sceneManager->scenes, 0 );
	sceneManager->totalScenes = 0;
	
	free( sceneManager );
	sceneManager = NULL;

}

static void updateSceneManager( SceneManager* sceneManager, float delta ) {

	if( sceneManager->currentScene == NULL ) {
		return;
	}

	if( sceneManager->currentScene->update == NULL ) {
		prismaticLogger->errorf( "Scene '%s' has NULL update function", sceneManager->currentScene->name );
	} else {
		sceneManager->currentScene->update( sceneManager->currentScene, delta );
	}

	// Run the update method for each of the Scene's sprites
	for( size_t i = 0; i < sceneManager->currentScene->totalSprites; i++ ) {
		
		PrismSprite* sp = sceneManager->currentScene->sprites[i];

		if( sp->update == NULL ) {
			continue;
		}

		sp->update( sp, delta );

	}

}

static void drawSceneManager( SceneManager* sceneManager, float delta ) {

	if( sceneManager->currentScene == NULL ) {
		return;
	}

	if( sceneManager->currentScene->draw == NULL ) {
		prismaticLogger->errorf( "Scene '%s' has NULL draw function", sceneManager->currentScene->name );
	} else {
		sceneManager->currentScene->draw( sceneManager->currentScene, delta );
	}

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

		sceneManager->currentScene->isActive = false;

		// Remove any active sprites from the screen
		sprites->removeAllSprites();

		sceneManager->previousScene = sceneManager->currentScene;

	}

	sceneManager->currentScene = scene;

	// Add Sprites from current Scene
	if( sceneManager->currentScene->sprites != NULL ) {
		size_t i = 0;
		for( i = 0; sceneManager->currentScene->sprites[i] != NULL; i++ ) {
			sprites->addSprite( sceneManager->currentScene->sprites[i]->sprite );
		}
	}

	sceneManager->currentScene->isActive = true;

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
        prismaticLogger->errorf( "Memory allocation failed for adding scene: %s", scene->name );
        return;
    }

    scene->sceneManager = sceneManager;
    
    sceneManager->scenes[sceneManager->totalScenes - 1] = scene;
    sceneManager->scenes[sceneManager->totalScenes] = NULL;

}

static void removeScene( SceneManager* sceneManager, Scene* scene ) {

	if( scene == NULL ) {
		prismaticLogger->infof( "Cannot remove NULL Scene from SceneManager" );
		return;
	}

	if( sceneManager->scenes == NULL ) {
		prismaticLogger->infof( "Scenes NULL when removing Scenes from SceneManager" );
		return;
	}
		
	if( prismaticString->equals( scene->name, sceneManager->currentScene->name ) ) {
		prismaticLogger->infof( "Cannot remove current Scene from SceneManager" );
		return;
	}

	size_t i = 0;
	for( i = 0; sceneManager->scenes[i] != NULL; i++ ) {
		
		if( strcmp( scene->name, sceneManager->scenes[i]->name ) != 0 ) {
			continue;
		}

		break;

	}

    if( sceneManager->scenes[i] == NULL ) {
        prismaticLogger->errorf( "Did not find Scene %d in SceneManager!", scene->name );
        return;
    }

    // Shift Scenes to remove Scene at i 
    for( size_t j = i; sceneManager->scenes[j] != NULL; j++ ) {
        sceneManager->scenes[j] = sceneManager->scenes[j + 1];
    }

    sceneManager->totalScenes--;

}

static void removeSceneByName( SceneManager* sceneManager, string sceneName ) {

	Scene* scene = getScene( sceneManager, sceneName );
	if( scene == NULL ) {
		return;
	}

	removeScene( sceneManager, scene );

}

static Scene* getScene( SceneManager* sceneManager, string sceneName ) {

	if( sceneManager->scenes == NULL ) {
		prismaticLogger->infof( "Scenes NULL when getting Scene '%s'", sceneName );
		return NULL;
	}

	size_t i = 0;
	for( i = 0; sceneManager->scenes[i] != NULL; i++ ) {

		if( strcmp( sceneManager->scenes[i]->name, sceneName ) != 0 ) {
			continue;
		}

		return sceneManager->scenes[i];

	}

	prismaticLogger->infof( "Scene id '%s' not found in SceneManager", sceneName );
	return NULL;

}

const SceneManagerFn* prismaticSceneManager = &(SceneManagerFn){
	.new = newSceneManager,
	.delete = deleteSceneManager,
	.update = updateSceneManager,
	.draw = drawSceneManager,
	.changeSceneByName = changeSceneByName,
	.changeScene = changeScene,
	.changeToPrevious = changeToPrevious,
	.changeToDefault = changeToDefault,
	.add = addScene,
	.remove = removeScene,
	.removeByName = removeSceneByName,
	.get = getScene,
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

	if( scene->sprites != NULL ) {

		for( size_t i = 0; scene->sprites[i] != NULL; i++ ) {
			prismaticSprite->delete( scene->sprites[i] );
		}

		scene->sprites = sys->realloc( scene->sprites, 0 );
		scene->sprites = NULL;
		scene->totalSprites = 0;

	}

	free( scene );
	scene = NULL;

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

	if( scene->sprites != NULL ) {
		size_t i = 0;
		for( i = 0; scene->sprites[i] != NULL; i++ ) {

			if( !prismaticString->equals( spriteId, scene->sprites[i]->id ) ) {
				continue;
			}

			return;

		}
	}

	scene->totalSprites += 1;
	scene->sprites = sys->realloc( scene->sprites, scene->totalSprites * sizeof(PrismSprite*) + 1 );

	if( scene->sprites == NULL ) {
        prismaticLogger->error( "Memory allocation failed for adding sprite." );
        return;
    }

    sp->id = prismaticString->new( spriteId );
    scene->sprites[scene->totalSprites - 1] = sp;
    scene->sprites[scene->totalSprites] = NULL;

    if( scene->isActive ) {
		sprites->addSprite( sp->sprite );
    }

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

	if( scene->sprites != NULL ) {		
		size_t i = 0;
		for( i = 0; scene->sprites[i] != NULL; i++ ) {
			
			if( strcmp( sp->id, scene->sprites[i]->id ) != 0 ) {
				continue;
			}

			sprites->removeSprite( scene->sprites[i]->sprite );
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

}


PrismSprite* getSprite( Scene* scene, string spriteId ) {

	if( scene->sprites == NULL ) {
		prismaticLogger->infof( "Scene sprites NULL when getting sprite '%s'", spriteId );
		return NULL;
	}

	size_t i = 0;
	for( i = 0; scene->sprites[i] != NULL; i++ ) {
		
		if( !prismaticString->equals( spriteId, scene->sprites[i]->id ) ) {
			continue;
		}

		return scene->sprites[i];

	}

	prismaticLogger->infof( "Sprite id '%s' not found in Scene", spriteId );
	return NULL;

}

const SceneFn* prismaticScene = &(SceneFn) {
	.new = newScene,
	.delete = deleteScene,
	.add = addSprite,
	.remove = removeSprite,
	.get = getSprite,
};