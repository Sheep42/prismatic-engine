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
    // Sets text for a Dialogue box
    // 
    // --- 
    // 
    // Dialogue* dialogue 
    // 
    // string text
    void (*setText)( Dialogue* dialogue, string text );

    // Sets a Dialogue box's position
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // float x - The dialogue box's x position
    // 
    // float y - The dialogue box's y position
    void (*setPosition)( Dialogue* dialogue, float x, float y );

    // Sets autohide value for a Dialogue box
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // bool autoHide - True: On, False: Off
    // 
    // float duration - How long to show the Dialogue before hiding
    void (*setAutoHide)( Dialogue* dialogue, bool autoHide, float duration );

    // Sets a Dialogue box width, height, and background color (LCDPatterns are supported)
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // float width
    // 
    // float height
    // 
    // LCDColor boxColor
    void (*setBox)( Dialogue* dialogue, float width, float height, LCDColor boxColor );

    // Sets a Dialogue box border width, height, and color (LCDPatterns are supported)
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // float width
    // 
    // float height
    // 
    // LCDCOlor borderColor
    void (*setBorder)( Dialogue* dialogue, float width, float height, LCDColor borderColor );

    // Sets a Dialogue box's font and text draw mode
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // LCDFont* font
    // 
    // LCDBitmapDrawMode drawMode
    void (*setFont)( Dialogue* dialogue, LCDFont* font, LCDBitmapDrawMode drawMode );

    // Sets a Dialogue box's draw offset
    // 
    // All Dialogue boxes are drawn to their own context which covers the entire screen.
    // Setting the draw offset will draw the box and text at the box's position, relative
    // to the offset x and y values, without affecting anything else on the screen.
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // float x - The x offset
    // 
    // float y - The y offset
    void (*setDrawOffset)( Dialogue* dialogue, float x, float y );

    // Sets padding around text in a Dialogue box
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // float x - The x padding (left and right)
    // 
    // float y - The y padding (top and bottom)
    void (*setTextPadding)( Dialogue* dialogue, float x, float y );

    // Set a Dialogue box's text speed
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // float speed - Speed is the amount of seconds between character advancement. 
    // Default: 0.0f 
    void (*setSpeed)( Dialogue* dialogue, float speed );

    // Sets a Dialogue box's active script
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // DialogueScript* script
    void (*setScript)( Dialogue* dialogue, DialogueScript* script );

    // Sets a Dialogue box's audio pitch
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // float pitch
    void (*setPitch)( Dialogue* dialogue, float pitch );

    // Sets a Dialogue box's text alignment
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // PDTextAlignment alignment - Default: kAlignTextLeft
    void (*setAlignment)( Dialogue* dialogue, PDTextAlignment alignment );

    // Sets a Dialogue box's wrap mode
    // 
    // ---
    // 
    // Dialogue* dialogue
    // 
    // PDTextWrappingMode wrapMode - Default: kWrapWord
    void (*setWrapMode)( Dialogue* dialogue, PDTextWrappingMode wrapMode );

    // Enables audio for a Dialogue box - Disabled by default
    // 
    // ---
    // 
    // Dialogue* dialogue
    void (*enableSound)( Dialogue* dialogue );

    // Disables audio for a Dialogue box
    // 
    // ---
    // 
    // Dialogue* dialogue
    void (*disableSound)( Dialogue* dialogue );

    // Advances a Dialogue box's script to the next line
    // 
    // ---
    // 
    // Dialogue* dialogue
    void (*advance)( Dialogue* dialogue );

    // Shows a Dialogue box
    // 
    // ---
    // 
    // Dialogue* dialogue
    void (*show)( Dialogue* dialogue );

    // Hides a Dialogue box
    // 
    // ---
    // 
    // Dialogue* dialogue
    void (*hide)( Dialogue* dialogue );

    // Flags a Dialogue box as finished
    // 
    // ---
    //
    // Dialogue* dialogue
    void (*finish)( Dialogue* dialogue );

    // Draws a Dialogue box
    //
    // Generally, should be called from inside of a Scene's or a Game's draw method
    //  
    // ---
    // 
    // Dialogue* self
    // 
    // float delta
    void (*draw)( struct Dialogue* self, float delta );

    // Updates a Dialogue box
    //
    // Generally, should be called from inside of a Scene's or a Game's update method
    //  
    // ---
    // 
    // Dialogue* self
    // 
    // float delta
    void (*update)( struct Dialogue* self, float delta );

    // Creates a new Dialogue box
    // 
    // ---
    // 
    // float x - The Dialogue box's starting X position
    // 
    // float y - The Dialogue box's starting Y position
    Dialogue* (*new)( float x, float y );

    // Destroys a Dialogue box
    // 
    // DOES NOT free any DialogueScript* or DialogueLine* that are currently attached 
    // to the Dialogue box. It is up to the caller to delete a script and/or lines
    // separately.
    // 
    // ---
    // 
    // Dialogue* self
    void (*destroy)( struct Dialogue* self );
} DialogueController;

typedef struct DialogueScriptManager {
    // Creates a new Dialogue Script
    // 
    // ---
    // 
    // string id - A machine-friendly unique identifier for the script
    DialogueScript* (*new)( string id );

    // Adds a dialogue line to a script
    // 
    // --- 
    // 
    // DialogueScript* script
    // 
    // DialogueLine* line
    void (*add)( DialogueScript* script, DialogueLine* line );

    // Removes a dialogue line from a script
    // 
    // Does not free the dialogue line being removed - That is the responsibility of the caller
    // 
    // --- 
    // 
    // DialogueScript* script
    // 
    // DialogueLine* line
    void (*remove)( DialogueScript* script, DialogueLine* line );

    // Removes the dialogue line at a specific index in a script
    // 
    // Does not free the dialogue line being removed - That is the responsibility of the caller
    // 
    // --- 
    // 
    // DialogueScript* script
    // 
    // size_t index
    void (*removeIndex)( DialogueScript* script, size_t index );

    // Resets a dialogue script's internal pointer to 0
    // 
    // ---
    // 
    // DialogueScript* script
    void (*reset)( DialogueScript* script );

    // Delete's a script
    // 
    // Deleting a script DOES free any lines that are added to the script at the 
    // time of its deletion. The caller should refrain from referencing any script
    // lines after the script is deleted. If a line needs to remain active, remove 
    // it from the script prior to deletion.
    // 
    // ---
    // 
    // DialogueScript* script
    void (*delete)( DialogueScript* script );
} DialogueScriptManager;

typedef struct DialogueLineManager {
    // Create a new dialogue line
    // 
    // DialogueLineType type - The dialogue line type 
    DialogueLine* (*new)( DialogueLineType type );

    // Delete a dialogue line
    //  
    // DialogueLine* line - The dialogue line to delete
    void (*delete)( DialogueLine* line );
} DialogueLineManager;

extern const DialogueController* dialogueController;
extern const DialogueScriptManager* dialogueScriptManager;
extern const DialogueLineManager* dialogueLineManager;

#endif