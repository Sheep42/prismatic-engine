#include <stddef.h>
#ifndef CAMERA_H

#ifndef SPRITE_INCLUDED
    #define SPRITE_INCLUDED
    #include "../sprite/sprite.h"
#endif

#ifndef DIALOGUE_INCLUDED
    #include "../dialogue/dialogue.h"
    #define DIALOGUE_INCLUDED
#endif

#define CAMERA_H

typedef enum {
    CS_Fixed = 0,
    CS_Manual_Lerp = 1,
    CS_Follow = 2,
    CS_Follow_Lerp = 3,
} CameraStyle;

typedef struct Camera {

    // The Camera Type - See CameraStyle in camera.h
    CameraStyle _cameraType;

    // The camera's target - Used by Follow Cams
    PrismSprite* __target;

    // The total dialogues currently added to the Camera
    size_t __dialogueCount; 

    // The Dialogue boxes contained by the Camera
    Dialogue** dialogues;

    // The camera x origin - Default: center of the screen
    float __originX;

    // The camera y origin - Default: center of the screen
    float __originY;

    // The camera's X position - Use setPos to set this value
    float __x;

    // The camera's Y position - Use setPos to set this value
    float __y;

    // The target X position - Used by Lerp Cams
    float __targetX;

    // The target Y position - Used by Lerp Cams
    float __targetY;

    // The lerp time for this Camera - Used by Lerp Cams
    float __lerpTime;

    // Used with restricted prop - The minimum X bounds
    float __minX;

    // Used with restricted prop - The maximum X bounds
    float __maxX;

    // Used with restricted prop - The minimum Y bounds
    float __minY;

    // Used with restricted prop - The maximum Y bounds
    float __maxY;

    // Is the camera movement restricted to min/max bounds? True/False
    // 
    // Default: False
    bool __restricted;

    // Will be true after the first update loop
    bool __init;
} Camera;

typedef struct CameraController {
    Camera* (*newFixedCam)( float x, float y );
    Camera* (*newLerpCam)( float startX, float startY, float time );
    Camera* (*newFollowCam)( PrismSprite* target );
    Camera* (*newFollowLerpCam)( PrismSprite* target, float time );
    
    // Set origin x & y position for a Camera
    void (*setOrigin)( Camera* self, float x, float y );

    // Set current x & y position for a Camera
    void (*setPos)( Camera* self, float x, float y );
    
    // Set min bouds or a restricted Camera
    void (*setMinBounds)( Camera* self, float minX, float minY );
    
    // Set max bounds for a restricted Camera
    void (*setMaxBounds)( Camera* self, float maxX, float maxY );
    
    // Set restricted value
    void (*setRestricted)( Camera* self, bool restricted );

    // Move a Camera to a new position. Will have different effect depending on camera type:
    // 
    // ---
    // 
    // Fixed Cam - Same as setPos( x, y )
    // 
    // Lerp Cam - Will move from current (x, y) to new (x, y)
    // 
    // Follow & Follow Lerp Cams - no effect
    // 
    // ---
    // 
    // Camera* self - The Camera
    // 
    // float newX
    // 
    // float newY
    void (*moveTo)( Camera* self, float newX, float newY );

    // Push a Dialogue box into this camera, so that the Dialogue box is
    // rendered within the camera's active space
    // 
    // Camera* self - The Camera
    // 
    // Dialogue* dialogue - The Dialogue box
    void (*pushDialogue)( Camera* self, Dialogue* dialogue );

    // Remove a Dialogue box that has been pushed to this camera
    // 
    // NOTE: This DOES NOT free the Dialogue, that must be done separately
    // 
    // Camera* self - The Camera
    // 
    // Dialogue* dialogue - The Dialogue box
    void (*removeDialogue)( Camera* self, Dialogue* dialogue );
    
    // The Camera's update function
    // 
    // ---
    // 
    // Camera* self - The Camera
    // 
    // float delta
    void (*update)( Camera* self, float delta );

    // Destroy a Camera
    // 
    // // NOTE: This DOES NOT free Dialogues attached to the Camera, that must be done separately
    void (*destroy)( Camera* self );
} CameraController;

extern const CameraController* cameraController;

#endif