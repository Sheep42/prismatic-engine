#ifndef TRANSITION_H
#define TRANSITION_H

#ifndef STDINT_INCLUDED
	#define STDINT_INCLUDED
	#include <stdint.h>
#endif

#ifndef PD_API_INCLUDED
	#define PD_API_INCLUDED
	#include "pd_api.h"
#endif

#ifndef TEXT_INCLUDED
	#define TEXT_INCLUDED
	#include "../text/text.h"
#endif

typedef enum {
	// Left-to-right draw in effect
	PrismTransitionType_LTRIn,
	// Left-to-right draw out effect
	PrismTransitionType_LTROut,
	// Right-to-left draw in effect
	PrismTransitionType_RTLIn,
	// Right-to-left draw out effect
	PrismTransitionType_RTLOut,
	// Grow out from center
	PrismTransitionType_GrowFromCenter,
	// Shrink in to center
	PrismTransitionType_ShrinkToCenter,
	// Slide whole image to the Right
	PrismTransitionType_SlideRight,
	// Slide whole image to the Left
	PrismTransitionType_SlideLeft,
	// Fade in, checkerboard pattern
	PrismTransitionType_FadeIn,
	// Fade out, checkerboard pattern
	PrismTransitionType_FadeOut,
	// User-defined effect
	PrismTransitionType_Custom,
} PrismTransitionType;

typedef struct PrismTransition {
	int type;
	float speed;
	float moveSpeed;
	float elapsed;
	float completeElapsed;
	float runTime;
	float completeDelay;
	int x;
	int y;
	LCDBitmapFlip flipped;
	LCDPattern pattern;
	LCDBitmap* mask;
	LCDBitmap* image;
	LCDBitmap* _rendered;
	bool finished;
	uint8_t _exp1;
	uint8_t _exp2;
	bool _pass;
	// Fires when entering the PrismTransition
	// 
	// PrismTransition* self
	void ( *enter )( struct PrismTransition* );
	// Fires while the PrismTransition is playing and is not finished
	// 
	// PrismTransition* self
	// 
	// float delta
	void ( *update )( struct PrismTransition*, float );
	// Draws the PrismTransition - Fires before update, on every frame
	// 
	// Default draw function for all transitions is drawTransition in 
	// transition.c
	// 
	// PrismTransition* self
	// 
	// float delta
	void ( *draw )( struct PrismTransition*, float );
	// Fires when a PrismTranistion first completes
	// 
	// PrismTransition* self
	void ( *complete )( struct PrismTransition* );
} PrismTransition;

typedef struct TransitionFn {
	// Create a new PrismTransition
	// 
	// LCDBitmap* image - The image to transition. Caller is responsible for freeing this bitmap
	// 
	// int x - The x position to draw the transition
	//
	// int y - The y position to draw the transition
	//
	// float speed - The speed that the transition should play at, in seconds
	//
	// int type - A PrismTransitionType
	PrismTransition* ( *new )( LCDBitmap*, int, int, float, int );
	// Delete a PrismTransition
	// 
	// PrismTransition* transition
	void ( *delete )( PrismTransition* );
	// Play a PrismTransition
	// 
	// PrismTransition* transition
	// 
	// float delta
	void ( *play )( PrismTransition*, float );
	// Complete a PrismTransition
	// 
	// You will need to implement your own completion state, and call this when
	// you implement a custom transition update function
	// 
	// PrismTransition* transition
	void ( *complete )( PrismTransition* );
} TransitionFn;

extern const int PRISM_TRANSITION_MIN;
extern const int PRISM_TRANSITION_MAX;

extern const TransitionFn* prismaticTransition;

#endif