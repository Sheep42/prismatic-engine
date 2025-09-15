#ifndef DIALOGUE_H
#define DIALOGUE_H

#ifndef STDDEF_INCLUDED
    #define STDDEF_INCLUDED
    #include <stddef.h>
#endif

#ifndef PD_API_INCLUDED
    #define PD_API_INCLUDED
    #include "pd_api.h"
#endif

#ifndef TEXT_INCLUDED
    #define TEXT_INCLUDED
    #include "../text/text.h"
#endif

typedef enum DialogueState {
    D_Hide = 0,
    D_Show = 1,
} DialogueState;

typedef enum DialogueType {
    DT_Instant = 0,
    DT_Typewriter = 1,
} DialogueType;

typedef enum DialogueLineType {
    DialogueLineType_String,
    DialogueLineType_Fn,
} DialogueLineType;

typedef struct DialogueLine {
    DialogueLineType type;
    string text;
    void (*fn)( void );
} DialogueLine;

typedef struct DialogueScript {
    string id;
    size_t _linePointer;
    DialogueLine** _lines;
    size_t _lineCount;
} DialogueScript;

typedef struct Dialogue {
    string text;
    float x;
    float y;
    float _innerX;
    float _innerY;
    float camX;
    float camY;
    bool autoHide;
    float showDuration;
    float boxWidth;
    float boxHeight;
    float borderWidth;
    float borderHeight;
    float textPaddingX;
    float textPaddingY;
    float speed;
    float pitch;
    PDSynth* synth;
    DialogueState state;
    DialogueType type;
    LCDBitmap* _canvas;
    LCDBitmap* _overlay;
    LCDFont* font;
    LCDColor borderColor;
    LCDColor boxColor;
    LCDBitmapDrawMode fontDrawMode;
    PDTextWrappingMode wrapMode;
    PDTextAlignment alignment;
    int _drawDialoguePointer;
    float _drawDialogueTimer;
    bool _lineFinished;
    PDButtons _input_current;
    PDButtons _input_pressed;
    PDButtons _input_released;
    bool _finished;
    float _hiddenTimer;
    bool _justHidden;
    DialogueScript* _script;
    void (*handleInput)( struct Dialogue* dialogue, float delta );
    void (*onFinishedCallback)( struct Dialogue* dialogue );
} Dialogue;

typedef struct DialogueController {
    void (*setText)( Dialogue* dialogue, string text );
    void (*setPosition)( Dialogue* dialogue, float x, float y );
    void (*setAutoHide)( Dialogue* dialogue, bool autoHide, float duration );
    void (*setBox)( Dialogue* dialogue, float width, float height, LCDColor boxColor );
    void (*setBorder)( Dialogue* dialogue, float width, float height, LCDColor borderColor );
    void (*setFont)( Dialogue* dialogue, LCDFont* font, LCDBitmapDrawMode drawMode );
    void (*setDrawOffset)( Dialogue* dialogue, float x, float y );
    void (*setTextPadding)( Dialogue* dialogue, float x, float y );
    void (*setSpeed)( Dialogue* dialogue, float speed );
    void (*setScript)( Dialogue* dialogue, DialogueScript* script );
    void (*setPitch)( Dialogue* dialogue, float pitch );
    void (*setAlignment)( Dialogue* dialogue, PDTextAlignment alignment );
    void (*setWrapMode)( Dialogue* dialogue, PDTextWrappingMode wrapMode );
    void (*enableSound)( Dialogue* dialogue );
    void (*disableSound)( Dialogue* dialogue );
    void (*advance)( Dialogue* dialogue );
    void (*show)( Dialogue* dialogue );
    void (*hide)( Dialogue* dialogue );
    void (*finish)( Dialogue* dialogue );
    void (*draw)( struct Dialogue* self, float delta );
    void (*update)( struct Dialogue* self, float delta );
    void (*destroy)( struct Dialogue* self );
} DialogueController;

typedef struct DialogueScriptManager {
    DialogueScript* (*new)( string id );
    void (*add)( DialogueScript* script, DialogueLine* line );
    void (*remove)( DialogueScript* script, DialogueLine* line );
    void (*removeIndex)( DialogueScript* script, size_t index );
    void (*reset)( DialogueScript* script );
    void (*delete)( DialogueScript* script );
} DialogueScriptManager;

typedef struct DialogueLineManager {
    DialogueLine* (*new)( DialogueLineType type );
    void (*delete)( DialogueLine* line );
} DialogueLineManager;

// Gets or creates a Dialogue
Dialogue* newDialogue( float x, float y );

extern const DialogueController* dialogueController;
extern const DialogueScriptManager* dialogueScriptManager;
extern const DialogueLineManager* dialogueLineManager;

#endif