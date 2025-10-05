#include <stddef.h>
#include "../../prismatic/prismatic.h"
#include "./dialoguescripts.h"

static void pushScript( DialogueScript* script );

static DialogueScript* intro( void );
static void introFn( void );

static size_t scriptCount = 0;
static DialogueScript** scripts;

void bootstrapDialogue() {

    pushScript( intro() );

}

void teardownDialogue() {

    if( scripts == NULL ) {
        return;
    }

    for( size_t i = 0; scripts[i] != NULL; i++ ) {
        dialogueScriptManager->delete( scripts[i] );
    }

    scripts = sys->realloc( scripts, 0 );
    scripts = NULL;

}

DialogueScript* getScript( string id ) {

    for( size_t i = 0; scripts[i] != NULL; i++ ) {
        
        if( !prismaticString->equals( id, scripts[i]->id ) ) {
            continue;
        }

        dialogueScriptManager->reset( scripts[i] );

        return scripts[i];

    }

    prismaticLogger->infof( "No script found with ID '%s'.", id );

    return NULL;

}

static void pushScript( DialogueScript* script ) {

    scriptCount++;
    scripts = sys->realloc( scripts, (scriptCount + 1) * sizeof( DialogueScript* ) );

    scripts[scriptCount - 1] = script;
    scripts[scriptCount] = NULL;

}

static DialogueScript* intro() {

    DialogueScript* script = dialogueScriptManager->new( "intro" );
    DialogueLine* lines[2];

    lines[0] = dialogueLineManager->new( DialogueLineType_String );
    lines[0]->text = "Welcome to Prismatic Engine!\n\nThis is sample dialogue...";
    dialogueScriptManager->add( script, lines[0] );

    lines[1] = dialogueLineManager->new( DialogueLineType_Fn );
    lines[1]->fn = introFn;
    dialogueScriptManager->add( script, lines[1] );

    return script;

}

static void introFn() {

    prismaticLogger->debug( "Run functions in dialogue!" );

}