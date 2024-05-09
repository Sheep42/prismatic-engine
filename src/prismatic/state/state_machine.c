#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "../prismatic.h"

// State Machine

static StateMachine* new( State* );
static void delete( StateMachine* );
static void freeStates( StateMachine* stateMachine );
static void update( StateMachine*, float );
static State* changeStateByName( StateMachine* stateMachine, char* name );
static State* changeState( StateMachine* stateMachine, State* state );
static State* changeToPrevious( StateMachine* stateMachine );
static State* changeToDefault( StateMachine* stateMachine );
static void addState( StateMachine* stateMachine, State* state );

static StateMachine* new( State* defaultState ) {
	
	StateMachine* stateMachine = calloc( 1, sizeof(StateMachine) );
	if( stateMachine == NULL ) {
		prismaticLogger->error( "Could not allocate memory for new state machine" );
		return NULL;
	}

	if( defaultState != NULL ) {
		stateMachine->defaultState = defaultState;
		addState( stateMachine, defaultState );
		changeState( stateMachine, defaultState );
	}

	return stateMachine;

}

static void delete( StateMachine* stateMachine ) {
	
	if( stateMachine == NULL )
		return;

	freeStates( stateMachine );

	free( stateMachine );
	stateMachine = NULL;

}

static void freeStates( StateMachine* stateMachine ) {
	
	for( int i = 0; i < stateMachine->totalStates; i++ ) {
		sys->realloc( stateMachine->states[i], 0 );
		stateMachine->states[i] = NULL;
	}

	stateMachine->totalStates = 0;

	sys->realloc( stateMachine->states, 0 );
	stateMachine->states = NULL;

}

static void update( StateMachine* stateMachine, float delta ) {

	if( stateMachine->currentState == NULL ) {
		return;
	}

	if( stateMachine->currentState->tick == NULL ) {
		prismaticLogger->errorf( "State '%s' has NULL tick function", stateMachine->currentState->name );
		return;
	}

	stateMachine->currentState->tick( delta );

}

static State* changeStateByName( StateMachine* stateMachine, char* name ) {

	State* state = NULL;

	for( int i = 0; i < stateMachine->totalStates; i++ ) {
		State* s = stateMachine->states[i];
		if( strcmp( s->name, name ) == 0 ) {
			state = changeState( stateMachine, s );
			break;
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
		
		if( stateMachine->currentState->exit != NULL ) {
			stateMachine->currentState->exit();
		}

		stateMachine->previousState = stateMachine->currentState;

	}

	stateMachine->currentState = state;

	if( stateMachine->currentState->enter != NULL ) {
		stateMachine->currentState->enter();
	}

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
	stateMachine->states = sys->realloc( stateMachine->states, stateMachine->totalStates * sizeof(State) + 1 );

	if (stateMachine->states == NULL) {
        prismaticLogger->error( "Memory allocation failed for adding state.\n" );
        return;
    }
    
    stateMachine->states[stateMachine->totalStates - 1] = state;
    stateMachine->states[stateMachine->totalStates] = NULL;

}

const StateMachineFn* prismaticStateMachine = &(StateMachineFn){
	.new = new,
	.delete = delete,
	.update = update,
	.changeStateByName = changeStateByName,
	.changeState = changeState,
	.changeToPrevious = changeToPrevious,
	.changeToDefault = changeToDefault,
	.addState = addState,
}; 

// State

static State* newState( const char* name ) {

	if( name == NULL ) {
		prismaticLogger->error( "Cannot create new state with NULL name" );
		return NULL;
	}

	State* state = calloc( 1, sizeof(State) );
	if( state == NULL ) {
		prismaticLogger->error( "Could not allocate memory for new state" );
		return NULL;
	}

	state->name = name;

	return state;

}

static void deleteState( State* state ) {
	free( state );
	state = NULL;
}

const StateFn* prismaticState = &(StateFn) {
	.new = newState,
	.delete = deleteState,
};