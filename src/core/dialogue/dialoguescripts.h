#ifndef DIALOGUESCRIPTS_H

#ifndef PRISMATIC_INCLUDED
    #include "../../prismatic/prismatic.h"
    #define PRISMATIC_INCLUDED
#endif

#define DIALOGUESCRIPTS_H

void bootstrapDialogue( void );
void teardownDialogue( void );
DialogueScript* getScript( string id );

#endif