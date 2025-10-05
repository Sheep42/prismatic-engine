#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dialogue.h"
#include "../prismatic.h"

static Dialogue* newDialogue( float x, float y );
static void setText( Dialogue* self, string text );
static void setPosition( Dialogue* self, float x, float y );
static void setAutoHide( Dialogue* self, bool autoHide, float duration );
static void setBox( Dialogue* self, float width, float height, LCDColor boxColor );
static void setBorder( Dialogue* self, float width, float height, LCDColor borderColor );
static void setFont( Dialogue* self, LCDFont* font, LCDBitmapDrawMode drawMode );
static void setDrawOffset( Dialogue* self, float x, float y );
static void setTextPadding( Dialogue* self, float x, float y );
static void setSpeed( Dialogue* self, float speed );
static void setScript( Dialogue* self, DialogueScript* script );
static void setPitch( Dialogue* self, float pitch );
static void setAlignment( Dialogue* self, PDTextAlignment alignment );
static void setWrapMode( Dialogue* self, PDTextWrappingMode wrapMode );
static void enableSound( Dialogue* self );
static void disableSound( Dialogue* self );
static void setType( Dialogue* self, DialogueType type );
static void advance( Dialogue* self );
static void show( Dialogue* dialogue );
static void hide( Dialogue* dialogue );
static void finish( Dialogue* self );
static void draw( Dialogue* self, float delta );
static void update( Dialogue* self, float delta );
static void destroy( Dialogue* self );

static void drawText( Dialogue* self, string text );
static void buildText( Dialogue* self );
static void executeLine( Dialogue* self );

// DialogueLine
static DialogueLine* newDialogueLine( DialogueLineType type );
static void deleteDialogueLine( DialogueLine* line );

// DialogueScript
static DialogueScript* newDialogueScript( string id );
static void addDialogueLine( DialogueScript* script, DialogueLine* line );
static void removeDialogueLine( DialogueScript* script, DialogueLine* line );
static void removeDialogueLineIndex( DialogueScript* script, size_t index );
static void resetDialogueScript( DialogueScript* script );
static void deleteDialogueScript( DialogueScript* script );

static const float BASE_PITCH = 261.63f;
static const float BASE_VOLUME = 0.25f;
static const float SYNTH_LENGTH = 0.05f;
static const float DIALOGUE_HIDE_BUFFER = 0.05f;

// Gets or creates a Dialogue
static Dialogue* newDialogue( float x, float y ) {

    Dialogue* d = calloc( 1, sizeof( Dialogue ) );
    d->x = x;
    d->y = y;

    const string FONT_PATH_SYSTEM = "/System/Fonts/Asheville-Sans-14-Bold.pft";

    LCDFont* sysFont = NULL;
    const char** err = NULL;
    sysFont = graphics->loadFont( FONT_PATH_SYSTEM, err );
    if( sysFont == NULL ) {
        prismaticLogger->errorf( "Could not load system font. err: %s", err );
    }

    d->_innerX = x;
    d->_innerY = y;
    d->textPaddingX = 6;
    d->textPaddingY = 4;
    d->_drawDialoguePointer = 0;
    d->speed = 0.0f;
    d->_drawDialogueTimer = 0;
    d->_lineFinished = false;
    d->pitch = BASE_PITCH;
    d->font = sysFont;
    d->fontDrawMode = kDrawModeCopy;
    d->borderColor = kColorBlack;
    d->boxColor = kColorWhite;
    d->alignment = kAlignTextLeft;
    d->wrapMode = kWrapWord;

    d->_canvas = graphics->newBitmap( pd->display->getWidth(), pd->display->getHeight(), kColorClear );
    d->_overlay = graphics->newBitmap( pd->display->getWidth(), pd->display->getHeight(), kColorClear );

    d->state = D_Hide;
    d->type = DT_Typewriter;

    return d;

}

static void setText( Dialogue* self, string text ) {

    if( self->text != NULL ) {
        prismaticString->delete( self->text );
        self->text = NULL;
    }

    self->text = prismaticString->new( text );

}

static void setPosition( Dialogue* self, float x, float y ) {

    self->x = x;
    self->y = y;

}

static void setAutoHide( Dialogue* self, bool autoHide, float duration ) {

    self->autoHide = autoHide;
    self->showDuration = autoHide ? duration : 0;

}

static void setBox( Dialogue* self, float width, float height, LCDColor boxColor ) {

    self->boxWidth = width;
    self->boxHeight = height;
    self->boxColor = boxColor;

}

static void setBorder( Dialogue* self, float width, float height, LCDColor borderColor ) {

    self->_innerX = self->x + ( width / 2 );
    self->_innerY = self->y + ( height / 2);

    self->borderWidth = width;
    self->borderHeight = height;
    self->borderColor = borderColor;

}

static void setFont( Dialogue* self, LCDFont* font, LCDBitmapDrawMode drawMode ) {

    self->font = font;
    self->fontDrawMode = drawMode;

}

static void setDrawOffset( Dialogue* self, float x, float y ) {

    self->camX = x;
    self->camY = y;

}

static void setTextPadding( Dialogue* self, float x, float y ) {

    self->textPaddingX = x;
    self->textPaddingY = y;

}

static void setSpeed( Dialogue* self, float speed ) {
    self->speed = speed;
}

static void setScript( Dialogue* self, DialogueScript* script ) {

    self->_script = script;
    self->_drawDialoguePointer = 0;
    self->_drawDialogueTimer = 0;
    self->_lineFinished = false;
    self->_finished = false;

    if( self->_script != NULL ) {
        executeLine( self );
    }

}

static void setPitch( Dialogue* self, float pitch ) {
    self->pitch = pitch;
}

static void setAlignment( Dialogue* self, PDTextAlignment alignment ) {
    self->alignment = alignment;
}

static void setWrapMode( Dialogue* self, PDTextWrappingMode wrapMode ) {
    self->wrapMode = wrapMode;
}

static void enableSound( Dialogue* self ) {

    if( self->synth != NULL ) {
        return;
    }

    self->synth = sound->synth->newSynth();

}

static void disableSound( Dialogue* self ) {

    if( self->synth == NULL ) {
        return;
    }

    sound->synth->freeSynth( self->synth );
    self->synth = NULL;

}

static void setType( Dialogue* self, DialogueType type ) {
    self->type = type;
}

static void show( Dialogue* self ) {
    self->state = D_Show;
}

static void hide( Dialogue* self ) {
    self->state = D_Hide;
    self->_justHidden = true;
}

static void draw( Dialogue* self, float delta ) {

    if( self->state != D_Show ) {
        return;
    }

    LCDBitmapDrawMode oldDrawMode = graphics->setDrawMode( kDrawModeCopy );

    graphics->pushContext( self->_canvas );
    graphics->clear( kColorClear );

    // Draw the outer dialogue box
    if( self->borderWidth > 0 && self->borderHeight > 0 ) {

        graphics->fillRect( 
            self->x, 
            self->y, 
            self->boxWidth + self->borderWidth, 
            self->boxHeight + self->borderHeight, 
            self->borderColor 
        );

    }

    // -- Draw the inner dialogue box
    graphics->fillRect( 
        self->_innerX, 
        self->_innerY, 
        self->boxWidth, 
        self->boxHeight, 
        self->boxColor 
    );

    if( self->_drawDialogueTimer < self->speed ) {
        self->_drawDialogueTimer += delta;
    } else {
        self->_drawDialogueTimer = 0;
    }
    
    if( self->text != NULL ) {

        if( self->type == DT_Instant ) {
            self->_lineFinished = true;
        }

        if( self->_lineFinished ) {
            drawText( self, self->text );
        } else {
            buildText( self );
        }

    }

    graphics->popContext();

    graphics->drawBitmap( self->_canvas, self->camX, self->camY, kBitmapUnflipped );
    graphics->setDrawMode( oldDrawMode );

}

static void update( Dialogue* self, float delta ) {

    if( self->_justHidden ) {
        
        self->_hiddenTimer += delta;
        if( self->_hiddenTimer >= DIALOGUE_HIDE_BUFFER ) {
            self->_hiddenTimer = 0.0f;
            self->_justHidden = false;
        }

    }

    if( self->state != D_Show ) {
        return;
    }

    sys->getButtonState( &self->_input_current, &self->_input_pressed, &self->_input_released );

    if( self->handleInput != NULL ) {
        self->handleInput( self, delta );
    }

}

static void drawText( Dialogue* self, string text ) {

    if( text == NULL ) {
        return;
    }

    string toDraw = prismaticString->new( text );

    graphics->setFont( self->font );
    LCDBitmapDrawMode previousDM = graphics->setDrawMode( self->fontDrawMode );
    graphics->drawTextInRect( 
        toDraw, 
        strlen( toDraw ), 
        kUTF8Encoding, 
        self->_innerX + self->textPaddingX, 
        self->_innerY + self->textPaddingY,
        self->boxWidth - (self->textPaddingX * 2),
        self->boxHeight - (self->textPaddingY * 2),
        self->wrapMode,
        self->alignment
    );

    graphics->setDrawMode( previousDM );
    prismaticString->delete( toDraw );

}

static void buildText( Dialogue* self ) {

    if( strlen( self->text ) <= 0 ) {
        self->_lineFinished = true;
        return;
    }

    string toDraw = prismaticString->substr( self->text, 0, self->_drawDialoguePointer );

    if( self->_drawDialoguePointer < strlen( self->text ) - 1 ) {
        if( self->_drawDialogueTimer >= self->speed ) {
            self->_drawDialoguePointer++;
        }
    } else {
        self->_lineFinished = true;
    }

    if( self->synth != NULL && !prismaticString->equals( toDraw, " " ) && !sound->synth->isPlaying( self->synth ) ) {
        float pitch = self->pitch + prismaticUtils->randBetween( -10, 10 );
        sound->synth->playNote( self->synth, pitch, BASE_VOLUME, SYNTH_LENGTH, 0 );
    }

    drawText( self, toDraw );

    prismaticString->delete( toDraw );

}

static void advance( Dialogue* self ) {

    if( self->_script == NULL ) {
        prismaticLogger->info( "dialogueController->advance: Dialogue does not have a script to advance!" );
        return;
    }

    if( self->_script->_linePointer >= (self->_script->_lineCount - 1) ) {
        dialogueController->finish( self );
        return;
    }

    self->_script->_linePointer++;
    executeLine( self );

}

static void executeLine( Dialogue* self ) {

    DialogueLine* line = self->_script->_lines[self->_script->_linePointer];

    if( line == NULL ) {
        prismaticLogger->error( "dialogueController->advance: Got NULL line!" );
        return;
    }

    self->_lineFinished = false;
    self->_drawDialoguePointer = 0;
    self->_drawDialogueTimer = 0;

    switch( line->type ) {
    case DialogueLineType_String:

        if( line->text != NULL ) {
            dialogueController->setText( self, line->text );
        } else {
            dialogueController->advance( self );
        }

        break;
    case DialogueLineType_Fn:

        if( line->fn != NULL ) {
            line->fn();
        }

        dialogueController->advance( self );

        break;
    default:
        break;
    }

}

static void finish( Dialogue* self ) {

    if( self->_finished ) {
        return;
    }

    self->_finished = true;

    if( self->onFinishedCallback != NULL ) {
        self->onFinishedCallback( self );
    }

}

static void destroy( Dialogue* self ) {

    if( self->text != NULL ) {
        prismaticString->delete( self->text );
        self->text = NULL;
    }

    if( self->synth != NULL ) {
        dialogueController->disableSound( self );
    }

    self->font = NULL;

    graphics->freeBitmap( self->_canvas );
    graphics->freeBitmap( self->_overlay );

    free( self );

}

//////////////////
// DialogueLine //
//////////////////
static DialogueLine* newDialogueLine( DialogueLineType type ) {

    DialogueLine* line = calloc( 1, sizeof( DialogueLine ) );
    line->type = type;

    return line;

}

static void deleteDialogueLine( DialogueLine* line ) {

    if( line->fn != NULL ) {
        line->fn = NULL;
    }

    if( line->text != NULL ) {
        line->text = NULL;
    }

    free( line );

}

////////////////////
// DialogueScript //
////////////////////
static DialogueScript* newDialogueScript( string id ) {

    DialogueScript* script = calloc( 1, sizeof( DialogueScript ) );
    script->_lineCount = 0;
    script->_linePointer = 0;
    script->id = id;

    return script;

}

static void addDialogueLine( DialogueScript* script, DialogueLine* line ) {

    script->_lineCount++;
    script->_lines = sys->realloc( script->_lines, (script->_lineCount + 1) * sizeof( DialogueLine* ) );

    script->_lines[script->_lineCount - 1] = line;
    script->_lines[script->_lineCount] = NULL;

}

static void removeDialogueLine( DialogueScript* script, DialogueLine* line ) {

    if( script == NULL ) {
        prismaticLogger->error( "dialogueScriptManager->remove: Cannot remove line from NULL script" );
        return;
    }

    if( line == NULL ) {
        prismaticLogger->error( "dialogueScriptManager->remove: Cannot remove NULL line from script" );
        return;
    }

    size_t i = 0;
    for( i = 0; script->_lines[i] != NULL; i++ ) {

        if( script->_lines[i] != line ) {
            continue;
        }

        break;

    }

    if( script->_lines[i] == NULL ) {
        prismaticLogger->info( "dialogueScriptManager->remove: Line not found in script" );
        return;
    }
 
    for( size_t j = i; script->_lines[j] != NULL; j++ ) {
        script->_lines[j] = script->_lines[j + 1];
    }

    script->_lineCount--;

    if( script->_linePointer >= script->_lineCount ) {
        resetDialogueScript( script );
    }

}

static void removeDialogueLineIndex( DialogueScript* script, size_t index ) {

    if( script == NULL ) {
        prismaticLogger->error( "dialogueScriptManager->removeIndex: Cannot remove line from NULL script" );
        return;
    }

    if( index >= script->_lineCount || index < 0 ) {
        prismaticLogger->error( "dialogueScriptManager->removeIndex: Index out of bounds" );
        return;
    }

    if( script->_lines[index] == NULL ) {
        prismaticLogger->info( "dialogueScriptManager->removeIndex: Line not found in script" );
        return;
    }

    for( size_t j = index; script->_lines[j] != NULL; j++ ) {
        script->_lines[j] = script->_lines[j + 1];
    }

    script->_lineCount--;

    if( script->_linePointer >= script->_lineCount ) {
        resetDialogueScript( script );
    }

}

static void resetDialogueScript( DialogueScript* script ) {
    script->_linePointer = 0;
}

static void deleteDialogueScript( DialogueScript* script ) {

    if( script->_lines != NULL ) {

        for( size_t i = 0; script->_lines[i] != NULL; i++ ) {
            deleteDialogueLine( script->_lines[i] );
        }

        script->_lines = sys->realloc( script->_lines, 0 );
        script->_lines = NULL;
    }

    free( script );

}

const DialogueController* dialogueController = &(DialogueController) {
    .setText = setText,
    .setPosition = setPosition,
    .setAutoHide = setAutoHide,
    .setBox = setBox,
    .setBorder = setBorder,
    .setFont = setFont,
    .setDrawOffset = setDrawOffset,
    .setTextPadding = setTextPadding,
    .setSpeed = setSpeed,
    .setPitch = setPitch,
    .setAlignment = setAlignment,
    .setWrapMode = setWrapMode,
    .setScript = setScript,
    .enableSound = enableSound,
    .disableSound = disableSound,
    .setType = setType,
    .advance = advance,
    .show = show,
    .hide = hide,
    .finish = finish,
    .draw = draw,
    .update = update,
    .new = newDialogue,
    .destroy = destroy,
};

const DialogueScriptManager* dialogueScriptManager = &(DialogueScriptManager) {
    .new = newDialogueScript,
    .add = addDialogueLine,
    .remove = removeDialogueLine,
    .removeIndex = removeDialogueLineIndex,
    .reset = resetDialogueScript,
    .delete = deleteDialogueScript,
};

const DialogueLineManager* dialogueLineManager = &(DialogueLineManager) {
    .new = newDialogueLine,
    .delete = deleteDialogueLine,
};