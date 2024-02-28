#include <stdlib.h>
#include <stdio.h>

#include "state.h"

static State* changeStateByName( StateMachine* stateMachine, char* name );
static State* changeState( StateMachine* stateMachine, State* state );
static State* changeToPrevious( StateMachine* stateMachine );
static State* changeToDefault( StateMachine* stateMachine );
static void addState( StateMachine* stateMachine, State* state );

static State* changeStateByName( StateMachine* stateMachine, char* name ) {

	State* state = NULL;

	for( int i = 0; i < stateMachine->totalStates; i++ ) {
		State* s = &stateMachine->states[i];
		if( s->name == name ) {
			state = changeState( stateMachine, s );
		}
	}

	return state;

}

static State* changeState( StateMachine* stateMachine, State* state ) {

	if( state == NULL ) {
		return stateMachine->currentState != NULL 
			? stateMachine->currentState 
			: stateMachine->defaultState;
	}

	if( stateMachine->currentState != NULL ) {
		stateMachine->currentState->exit();
		stateMachine->previousState = stateMachine->currentState;
	}

	stateMachine->currentState = state;
	stateMachine->currentState->enter();

}

static State* changeToPrevious( StateMachine* stateMachine ) {
	return NULL;
}

static State* changeToDefault( StateMachine* stateMachine ) {
	return NULL;
}

static void addState( StateMachine* stateMachine, State* state ) {

	// TODO: Add State

}

const StateMachineFn* prismaticStateMachine = &(StateMachineFn){
	.changeStateByName = changeStateByName,
	.changeState = changeState,
	.changeToPrevious = changeToPrevious,
	.changeToDefault = changeToDefault,
	.addState = addState,
}; 