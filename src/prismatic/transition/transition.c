#include "../prismatic.h"
#include "transition.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static PrismTransition* newTransition( LCDBitmap* image, int x, int y, float speed, int type );
static void deleteTransition( PrismTransition* transition );
static void playTransition( PrismTransition* transition, float delta );
static void completeTransition( PrismTransition* transition, float delta );
static void drawTransition( PrismTransition* self, float delta );
static void cleanTransitionImages( PrismTransition* transition );

static void initializeShow( PrismTransition* transition );
static void initializeHide( PrismTransition* transition );

static void LTRIn_update( PrismTransition* self, float delta );
static void LTROut_update( PrismTransition* self, float delta );
static void RTLIn_update( PrismTransition* self, float delta );
static void RTLOut_update( PrismTransition* self, float delta );
static void GrowFromCenter_update( PrismTransition* self, float delta );
static void ShrinkToCenter_update( PrismTransition* self, float delta );
static void SlideRight_update( PrismTransition* self, float delta );
static void SlideLeft_update( PrismTransition* self, float delta );
static void FadeIn_update( PrismTransition* self, float delta );
static void FadeOut_update( PrismTransition* self, float delta );

const int PRISM_TRANSITION_MIN = 0;
const int PRISM_TRANSITION_MAX = 255;

static PrismTransition* newTransition( LCDBitmap* image, int x, int y, float speed, int type ) {

    PrismTransition* transition = calloc( 1, sizeof( PrismTransition ) );

    transition->type = type;
    transition->speed = speed;
    transition->moveSpeed = 15;
    transition->finished = false;
    transition->elapsed = 0.0f;
    transition->completeElapsed = 0.0f;
    transition->runTime = 0.0f;
    transition->flipped = kBitmapUnflipped;
    transition->image = image;
    transition->_pass = false;
    transition->completeDelay = 0;

    // initialize properties for built-ins
    switch( type ) {
        case PrismTransitionType_LTROut:
            initializeShow( transition );
            transition->update = LTROut_update;
            transition->_exp1 = 7;
            break;
        case PrismTransitionType_LTRIn:
            initializeHide( transition );
            transition->update = LTRIn_update;
            transition->_exp1 = 7;
            break;

        case PrismTransitionType_RTLOut:
            transition->_exp1 = 0;
            initializeShow( transition );
            transition->update = RTLOut_update;
            break;
        case PrismTransitionType_RTLIn:
            transition->_exp1 = 0;
            initializeHide( transition );
            transition->update = RTLIn_update;
            break;

        case PrismTransitionType_ShrinkToCenter:
            transition->_exp1 = 3;
            transition->_exp2 = 4;
            initializeShow( transition );
            transition->update = ShrinkToCenter_update;
            break;
        case PrismTransitionType_GrowFromCenter:
            transition->_exp1 = 3;
            transition->_exp2 = 4;
            initializeHide( transition );
            transition->update = GrowFromCenter_update;
            break;
        case PrismTransitionType_SlideRight:
            initializeShow( transition );
            transition->update = SlideRight_update;
            break;
        case PrismTransitionType_SlideLeft:
            initializeShow( transition );
            transition->update = SlideLeft_update;
            break;
        case PrismTransitionType_FadeIn:
            transition->_exp1 = 0;
            transition->_exp2 = 7;
            initializeHide( transition );
            transition->update = FadeIn_update;
            break;
        case PrismTransitionType_FadeOut:
            transition->_exp1 = 0;
            transition->_exp2 = 7;
            initializeShow( transition );
            transition->update = FadeOut_update;
            break;
        default:
            break;
    }

    transition->draw = drawTransition;

	return transition;

}

static void initializeShow( PrismTransition* transition ) {
    for( uint8_t i = 0; i < 16; i++ ) {
        transition->pattern[i] = PRISM_TRANSITION_MAX; // Initialize all bits to 0xFF
    }
}

static void initializeHide( PrismTransition* transition ) {
    for( uint8_t i = 0; i < 16; i++ ) {
        transition->pattern[i] = i < 8 ? 
            PRISM_TRANSITION_MIN      // Initialize bitmap bits to 0x00
            : PRISM_TRANSITION_MAX;   // Initialize mask bits to 0xFF
    }
}

static void deleteTransition( PrismTransition* transition ) {

    if( transition->_rendered != NULL ) {
        graphics->freeBitmap( transition->_rendered );
    }

    if( transition->mask != NULL ) {
        graphics->freeBitmap( transition->mask );
    }

    free( transition );
    transition = NULL;

}

static void playTransition( PrismTransition* transition, float delta ) {

    if( transition->update == NULL ) {
        prismaticLogger->error( "Transition playing without update function" );
        return;
    }

    if( transition->finished ) {
        return;
    }

    transition->elapsed += delta;
    transition->runTime += delta;

    if( transition->draw != NULL ) {
        cleanTransitionImages( transition );
        transition->draw( transition, delta );
    }

    if( transition->elapsed < transition->speed ) {
        return;
    }

    transition->update( transition, delta );

    transition->elapsed = 0;

}

static void drawTransition( PrismTransition* self, float delta ) {

    if( self->image == NULL ) {
        prismaticLogger->error( "Transition drawing without an image" );
        return;
    }

    int w = 0, h = 0;
    graphics->getBitmapData( self->image, &w, &h, NULL, NULL, NULL );

    self->_rendered = graphics->copyBitmap( self->image );
    self->mask = graphics->newBitmap( w, h, (LCDColor)self->pattern );

    graphics->setBitmapMask( self->_rendered, self->mask );
    graphics->clear( kColorWhite );
    graphics->drawBitmap( self->_rendered , self->x, self->y, self->flipped );

}

static void completeTransition( PrismTransition* transition, float delta ) {

    transition->completeElapsed += delta;
    transition->speed = 0.0f;

    if( transition->finished || transition->completeElapsed < transition->completeDelay ) {
        return;
    }

    transition->finished = true;

    cleanTransitionImages( transition );

    if( transition->complete != NULL ) {
        transition->complete( transition );
    }

}

static void cleanTransitionImages( PrismTransition* transition ) {

    if( transition->_rendered != NULL ) {
        graphics->freeBitmap( transition->_rendered );
        transition->_rendered = NULL;
    }

    if( transition->mask != NULL ) {
        graphics->freeBitmap( transition->mask );
        transition->mask = NULL;
    }

}

// built-in type update functions
static void LTRIn_update( PrismTransition* self, float delta ) {

    bool done = true;

    for( uint8_t i = 0; i < 8; i++ ) {
        if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
            self->pattern[i] = self->pattern[i] + ( prismaticUtils->uint8_pow( 2, self->_exp1 ) );

            if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
                done = false;
            }
        } else {
            self->pattern[i] = PRISM_TRANSITION_MAX;
        }
    }

    if( self->_exp1 > 0 ) {
        self->_exp1--;
    }

    if( done ) {
        completeTransition( self, delta );
    }

}

static void LTROut_update( PrismTransition* self, float delta ) {

    bool done = true;

    for( uint8_t i = 0; i < 8; i++ ) {

        if( self->pattern[i] > PRISM_TRANSITION_MIN ) {
            self->pattern[i] = self->pattern[i] - ( prismaticUtils->uint8_pow( 2, self->_exp1 ) );

            if( self->pattern[i] > PRISM_TRANSITION_MIN ) {
                done = false;
            }
        } else {
            self->pattern[i] = PRISM_TRANSITION_MIN;
        }

    }

    if( self->_exp1 > 0 ) {
        self->_exp1--;
    }

    if( done ) {
        completeTransition( self, delta );
    }

}

static void RTLIn_update( PrismTransition* self, float delta ) {

    bool done = true;

    for( uint8_t i = 0; i < 8; i++ ) {
        if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
            self->pattern[i] = self->pattern[i] + ( prismaticUtils->uint8_pow( 2, self->_exp1 ) );

            if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
                done = false;
            }
        } else {
            self->pattern[i] = PRISM_TRANSITION_MAX;
        }
    }

    if( self->_exp1 < 8 ) {
        self->_exp1++;
    }

    if( done ) {
        completeTransition( self, delta );
    }

}

static void RTLOut_update( PrismTransition* self, float delta ) {

    bool done = true;

    for( uint8_t i = 0; i < 8; i++ ) {

        if( self->pattern[i] > PRISM_TRANSITION_MIN ) {
            self->pattern[i] = self->pattern[i] - ( prismaticUtils->uint8_pow( 2, self->_exp1 ) );

            if( self->pattern[i] > PRISM_TRANSITION_MIN ) {
                done = false;
            }
        } else {
            self->pattern[i] = PRISM_TRANSITION_MIN;
        }

    }

    if( self->_exp1 < 8 ) {
        self->_exp1++;
    }

    if( done ) {
        completeTransition( self, delta );
    }

}

static void GrowFromCenter_update( PrismTransition* self, float delta ) {

    bool done = true;

    for( uint8_t i = 0; i < 8; i++ ) {
        
        if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
            self->pattern[i] += ( prismaticUtils->uint8_pow( 2, self->_exp1 ) + prismaticUtils->uint8_pow( 2, self->_exp2 ) );

            if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
                done = false;
            }
        } else {
            self->pattern[i] = PRISM_TRANSITION_MAX;
        }

    }

    if( self->_exp1 > 0 ) self->_exp1--;
    if( self->_exp2 < 7 ) self->_exp2++;

    if( done ) {
        completeTransition( self, delta );
    }

}

static void ShrinkToCenter_update( PrismTransition* self, float delta ) {

    bool done = true;

    for( uint8_t i = 0; i < 8; i++ ) {
        
        if( self->pattern[i] > PRISM_TRANSITION_MIN) {
            self->pattern[i] -= ( prismaticUtils->uint8_pow( 2, self->_exp1 ) + prismaticUtils->uint8_pow( 2, self->_exp2 ) );

            if( self->pattern[i] > PRISM_TRANSITION_MIN) {
                done = false;
            }
        } else {
            self->pattern[i] = PRISM_TRANSITION_MIN;
        }

    }

    if( self->_exp1 > 0 ) self->_exp1--;
    if( self->_exp2 < 7 ) self->_exp2++;

    if( done ) {
        completeTransition( self, delta );
    }

}

static void SlideRight_update( PrismTransition* self, float delta ) {
    
    self->x += self->moveSpeed;

    if( self->x >= pd->display->getWidth() ) {
        completeTransition( self, delta );
    }

}

static void SlideLeft_update( PrismTransition* self, float delta ) {
    
    self->x -= self->moveSpeed;

    if( abs( self->x ) >= pd->display->getWidth() ) {
        completeTransition( self, delta );
    }

}

static void FadeIn_update( PrismTransition* self, float delta ) {

    bool done = true;

    if( self->pattern[0] < PRISM_TRANSITION_MAX ) {

        for( uint8_t i = 0; i < 8; i += 2 ) {

            if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
                self->pattern[i] += prismaticUtils->uint8_pow( 2, self->_exp1 ) + prismaticUtils->uint8_pow( 2, self->_exp2 );
            } else {
                self->pattern[i] = PRISM_TRANSITION_MAX;
            }

        }

    } else {

        // Reset exponents and fade the odd bytes
        if( false == self->_pass ) {
            self->_pass = true;
            self->_exp1 = 0;
            self->_exp2 = 7;
        }

        for( uint8_t i = 1; i < 8; i += 2 ) {

            if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
                self->pattern[i] += prismaticUtils->uint8_pow( 2, self->_exp1 ) + prismaticUtils->uint8_pow( 2, self->_exp2 );
            } else {
                self->pattern[i] = PRISM_TRANSITION_MAX;
            }

        }

    }

    if( self->_exp1 < 8 ) {
        self->_exp1 += 2;
    }

    if( self->_exp2 > 1 ) {
        self->_exp2 -= 2;
    }

    for( uint8_t i = 0; i < 8; i++ ) {
        if( self->pattern[i] < PRISM_TRANSITION_MAX ) {
            done = false;
        }
    }

    if( done ) {
        completeTransition( self, delta );
    }

}

static void FadeOut_update( PrismTransition* self, float delta ) {

    bool done = true;

    if( self->pattern[0] > PRISM_TRANSITION_MIN ) {

        // First fade the even bytes
        for( uint8_t i = 0; i < 8; i += 2 ) {
            
            if( self->pattern[i] > PRISM_TRANSITION_MIN ) {
                self->pattern[i] = self->pattern[i] - prismaticUtils->uint8_pow( 2, self->_exp1 ) - prismaticUtils->uint8_pow( 2, self->_exp2 );
            } else {
                self->pattern[i] = PRISM_TRANSITION_MIN;
            }

        }

    } else {

        // Reset exponents and fade the odd bytes
        if( false == self->_pass ) {
            self->_pass = true;
            self->_exp1 = 0;
            self->_exp2 = 7;
        }

        for( uint8_t i = 1; i < 8; i += 2 ) {
            
            if( self->pattern[i] > PRISM_TRANSITION_MIN ) {
                self->pattern[i] = self->pattern[i] - prismaticUtils->uint8_pow( 2, self->_exp1 ) - prismaticUtils->uint8_pow( 2, self->_exp2 );
            } else {
                self->pattern[i] = PRISM_TRANSITION_MIN;
            }

        }

    }

    if( self->_exp1 < 8 ) {
        self->_exp1 += 2;
    } 
    
    if( self->_exp2 > 1 ) {
        self->_exp2 -= 2;
    }

    for( uint8_t i = 0; i < 8; i++ ) {
        if( self->pattern[i] > PRISM_TRANSITION_MIN ) {
            done = false;
        }
    }

    if( done ) {
        completeTransition( self, delta );
    }

}

const TransitionFn* prismaticTransition = &(TransitionFn) {
    .new = newTransition,
    .delete = deleteTransition,
    .play = playTransition,
    .complete = completeTransition,
};