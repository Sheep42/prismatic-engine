#include <stdlib.h>
#include <stdio.h>

#include "../prismatic.h"

static State* changeStateByName( StateMachine* stateMachine, char* name );
static State* changeState( StateMachine* stateMachine, State* state );
static State* changeToPrevious( StateMachine* stateMachine );
static State* changeToDefault( StateMachine* stateMachine );
static void addState( StateMachine* stateMachine, State* state );

StateMachine* newStateMachine( State* defaultState ) {
	
	StateMachine* stateMachine = calloc( 1, sizeof(StateMachine) );
	if( stateMachine == NULL ) {
		prismaticLogger->error( "Could not allocate memory for new state machine" );
		return NULL;
	}

	if( defaultState != NULL ) {
		stateMachine->defaultState = defaultState;
	}

	return stateMachine;

}

void deleteStateMachine( StateMachine* stateMachine ) {
	
	if( stateMachine == NULL )
		return;

	free( stateMachine->states );
	free( stateMachine );

}

static State* changeStateByName( StateMachine* stateMachine, char* name ) {

	State* state = NULL;

	for( int i = 0; i < stateMachine->totalStates; i++ ) {
		State* s = &stateMachine->states[i];
		if( strcmp( s->name, name ) == 0 ) {
			state = changeState( stateMachine, s );
		}
	}

	return state;

}

static State* changeState( StateMachine* stateMachine, State* state ) {

	if( stateMachine == NULL ) {
		prismaticLogger->error( "stateMachine cannot be NULL" );
		return NULL;
	}

	if( state == NULL ) {
		prismaticLogger->error( "state cannot be NULL" );
		return NULL;
	}

	if( stateMachine->currentState != NULL ) {
		stateMachine->currentState->exit();
		stateMachine->previousState = stateMachine->currentState;
	}

	stateMachine->currentState = state;
	stateMachine->currentState->enter();

	return stateMachine->currentState;

}

static State* changeToPrevious( StateMachine* stateMachine ) {
	
	if( stateMachine->previousState == NULL ) {
		return prismaticStateMachine->changeToDefault( stateMachine );
	}

	return prismaticStateMachine->changeState( stateMachine, stateMachine->previousState );

}

static State* changeToDefault( StateMachine* stateMachine ) {
	return prismaticStateMachine->changeState( stateMachine, stateMachine->defaultState );
}

static void addState( StateMachine* stateMachine, State* state ) {

	if( state == NULL ) {
		prismaticLogger->info( "Not adding NULL state to state machine" );
		return;
	}

	stateMachine->totalStates += 1;
	stateMachine->states = realloc( stateMachine->states, stateMachine->totalStates * sizeof(State) );

	if (stateMachine->states == NULL) {
        prismaticLogger->error( "Memory allocation failed for adding state.\n" );
        return;
    }
    
    stateMachine->states[stateMachine->totalStates - 1] = *state;

}

const StateMachineFn* prismaticStateMachine = &(StateMachineFn){
	.changeStateByName = changeStateByName,
	.changeState = changeState,
	.changeToPrevious = changeToPrevious,
	.changeToDefault = changeToDefault,
	.addState = addState,
}; 