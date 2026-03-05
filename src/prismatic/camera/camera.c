#include <stdio.h>

#include "camera.h"
#include "../../prismatic/prismatic.h"

static Camera* __newCam( float x, float y, CameraStyle type );

static Camera* newFixedCam( float x, float y );
static Camera* newLerpCam( float startX, float startY, float time );
static Camera* newFollowCam( PrismSprite* target );
static Camera* newFollowLerpCam( PrismSprite* target, float time );
static void setOrigin( Camera* self, float x, float y );
static void setPos( Camera* self, float x, float y );
static void setMinBounds( Camera* self, float minX, float minY );
static void setMaxBounds( Camera* self, float maxX, float maxY );
static void setRestricted( Camera* self, bool restricted );
static void moveTo( Camera* self, float newX, float newY );
static void pushDialogue( Camera* self, Dialogue* dialogue );
static void removeDialogue( Camera* self, Dialogue* dialogue );
static void update( Camera* self, float delta );
static void destroy( Camera* self );

static const float LERP_THRESHOLD = 0.1f;

static Camera* __newCam( float x, float y, CameraStyle type ) {

    Camera* cam = sys->realloc( NULL, sizeof( Camera ) );

    cam->__originX = (pd->display->getWidth() / 2.0f);
    cam->__originY = (pd->display->getHeight() / 2.0f);

    cam->__x = x;
    cam->__y = y;
    cam->__targetX = x;
    cam->__targetY = y;
    cam->__lerpTime = 0;
    
    cam->__minX = 0;
    cam->__maxX = 0;
    cam->__minY = 0;
    cam->__maxY = 0;

    cam->dialogues = NULL;
    cam->__dialogueCount = 0;

    cam->_cameraType = type;
    cam->__restricted = false;
    cam->__init = false;

    return cam;

}

static Camera* newFixedCam( float x, float y ) {

    Camera* cam = __newCam( x, -y, CS_Fixed );

    cam->__x = cam->__targetX = cam->__originX - x;
    cam->__y = cam->__targetY = cam->__originY - y;

    return cam;

}

static Camera* newLerpCam( float startX, float startY, float time ) {


    Camera* cam = __newCam( startX, startY, CS_Manual_Lerp );
    
    startX = cam->__originX - startX;
    startY = cam->__originY - startY;
    
    cam->__lerpTime = time;
    cam->__x = cam->__targetX = startX;
    cam->__y = cam->__targetY = startY;

    return cam;

}

static Camera* newFollowCam( PrismSprite* target ) {

    float startX = 0, startY = 0; 
    sprites->getPosition( target->sprite, &startX, &startY );

    Camera* cam = __newCam( startX, startY, CS_Follow );

    startX = cam->__originX - startX;
    startY = cam->__originY - startY;

    cam->__target = target;
    cam->__x = cam->__targetX = startX;
    cam->__y = cam->__targetY = startY;

    return cam;

}


static Camera* newFollowLerpCam( PrismSprite* target, float time ) {

    Camera* cam = newFollowCam( target );
    cam->_cameraType = CS_Follow_Lerp;
    cam->__lerpTime = time;

    return cam;

}

static void setOrigin( Camera* self, float x, float y ) {

    self->__originX = x;
    self->__originY = y;

}

static void setPos( Camera* self, float x, float y ) {

    self->__x = self->__targetX = self->__originX - x;
    self->__y = self->__targetY = self->__originY - y;

}

static void setMinBounds( Camera* self, float minX, float minY ) {

    self->__minX = minX;
    self->__minY = minY;

}

static void setMaxBounds( Camera* self, float maxX, float maxY ) {

    self->__maxX = maxX;
    self->__maxY = maxY;

}

static void setRestricted( Camera* self, bool restricted ) {

    self->__restricted = restricted;

}

static void moveTo( Camera* self, float newX, float newY ) {

    switch( self->_cameraType ) {
    case CS_Fixed:
        cameraController->setPos( self, newX, newY );
        break;
    case CS_Manual_Lerp:
        self->__targetX = self->__originX - newX;
        self->__targetY = self->__originY - newY;
        break;
    case CS_Follow:
    case CS_Follow_Lerp:
    default:
        // Do Nothing
        break;
    }

}

static void pushDialogue( Camera* self, Dialogue* dialogue ) {

    self->__dialogueCount++;
    self->dialogues = sys->realloc( self->dialogues, (self->__dialogueCount + 1) * sizeof( Dialogue* ) );

    self->dialogues[self->__dialogueCount - 1] = dialogue;
    self->dialogues[self->__dialogueCount] = NULL;

}

static void removeDialogue( Camera* self, Dialogue* dialogue ) {

    if( self == NULL ) {
        prismaticLogger->error( "cameraController->removeDialogue: Cannot remove Dialogue from NULL Camera" );
        return;
    }

    if( dialogue == NULL ) {
        prismaticLogger->error( "cameraController->removeDialogue: Cannot remove NULL Dialogue from Camera" );
        return;
    }

    size_t i = 0;
    for( i = 0; self->dialogues[i] != NULL; i++ ) {

        if( self->dialogues[i] != dialogue ) {
            continue;
        }

        break;

    }

    if( self->dialogues[i] == NULL ) {
        prismaticLogger->info( "cameraController->removeDialogue: Dialogue not found in Camera" );
        return;
    }
 
    for( size_t j = i; self->dialogues[j] != NULL; j++ ) {
        self->dialogues[j] = self->dialogues[j + 1];
    }

    self->__dialogueCount--;

}

static void update( Camera* self, float delta ) {

    float targetX = 0, targetY = 0;

    switch( self->_cameraType ) {
    case CS_Fixed:
    case CS_Manual_Lerp:
        break;

    case CS_Follow:
    case CS_Follow_Lerp:
        sprites->getPosition( self->__target->sprite, &targetX, &targetY );
        self->__targetX = self->__originX - targetX;
        self->__targetY = self->__originY - targetY;

        break;

    default:
        break;
    }

    if( self->__restricted ) {
    
        // xxx: sign reversals
        // target X and Y values are offsets, so they are opposite. 
        // hence the sign reversal in the check and assignment.
        if( -self->__targetX <= self->__minX ) {
            self->__targetX = self->__minX;
        } else if( -self->__targetX >= self->__maxX ) {
            self->__targetX = -self->__maxX;
        }

        if( -self->__targetY <= self->__minY ) {
            self->__targetY = self->__minY;
        } else if( -self->__targetY >= self->__maxY ) {
            self->__targetY = -self->__maxY;
        }
    }

    // Set initial offset ignoring lerp
    if( !self->__init ) {
        self->__x = self->__targetX;
        self->__y = self->__targetY;

        graphics->setDrawOffset( self->__targetX, self->__targetY );
    }

    if( self->__lerpTime > 0 ) {
        
        float newX = self->__targetX, newY = self->__targetY;
        
        newX = prismaticUtils->lerp( self->__x, self->__targetX, self->__lerpTime * delta );
        newY = prismaticUtils->lerp( self->__y, self->__targetY, self->__lerpTime * delta );

        if( fabsf( newX - self->__x ) > LERP_THRESHOLD ) {
            self->__x = newX;
        } else {
            self->__x = self->__targetX;
        }

        if( fabsf( newY - self->__y ) > LERP_THRESHOLD ) {
            self->__y = newY;
        } else {
            self->__y = self->__targetY;
        }

    } else {

        self->__x = self->__targetX;
        self->__y = self->__targetY;

    }

    graphics->setDrawOffset( self->__x, self->__y );

    if( self->__dialogueCount > 0 ) {

        for( size_t i = 0; self->dialogues[i] != NULL; i++ ) {
            // xxx: Dialogue internal cam is a position, not a screen offset, 
            // so we need to reverse the sign of the Camera's offsets to pass 
            // into Dialogue draw offset.
            dialogueController->setDrawOffset( self->dialogues[i], -self->__x, -self->__y );
        }

    }

    if( !self->__init ) {
        self->__init = true;
    }

}

static void destroy( Camera* self ) {

    if( self->dialogues != NULL ) {
        self->dialogues = sys->realloc( self->dialogues, 0 );
        self->dialogues = NULL;
        self->__dialogueCount = 0;
    }

    self = sys->realloc( self, 0 );
    self = NULL;

}

const CameraController* cameraController = &(CameraController) {
    .newFixedCam = newFixedCam,
    .newLerpCam = newLerpCam,
    .newFollowCam = newFollowCam,
    .newFollowLerpCam = newFollowLerpCam,
    .setOrigin = setOrigin,
    .setPos = setPos,
    .setMinBounds = setMinBounds,
    .setMaxBounds = setMaxBounds,
    .setRestricted = setRestricted,
    .moveTo = moveTo,
    .pushDialogue = pushDialogue,
    .removeDialogue = removeDialogue,
    .update = update,
    .destroy = destroy,
};