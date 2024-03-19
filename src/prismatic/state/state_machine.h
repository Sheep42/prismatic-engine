// States
#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H

#ifndef STDDEF_INCLUDED
	#include <stddef.h>
	#define STDDEF_INCLUDED
#endif

typedef struct State {
	const char* name;
	void (*enter)( void );
	void (*exit)( void );
	void (*tick)( float );
} State;

// State Machines
typedef struct StateMachine {
	State** states;
	size_t totalStates;
	State* defaultState;
	State* previousState;
	State* currentState;
} StateMachine;

typedef struct StateMachineFn {
	// Create a new State Machine
	// 
	// ---
	// 
	// State* defaultState
	StateMachine* ( *new )( State* );
	// Delete a State Machine
	// 
	// ---
	// 
	// StateMachine* stateMachine
	void ( *delete )( StateMachine* );
	// Update the State Machine
	// 
	// Generally you will want to call this from inside of the game's main
	// update loop.
	// 
	// ---
	// 
	// StateMachine* stateMachine
	// 
	// float delta
	void ( *update )( StateMachine*, float );
	// Change the State Machine's current state to the named State
	// 
	// ---
	// 
	// StateMachine* stateMachine
	// 
	// string stateName
	State* ( *changeStateByName )( StateMachine*, char* );
	// Change the State Machine's current state to the provided State
	// 
	// ---
	// 
	// StateMachine* stateMachine
	// 
	// State* state
	State* ( *changeState )( StateMachine*, State* );
	// Change the State Machine's current State to its previous State
	// 
	// ---
	// 
	// StateMachine* stateMachine
	State* ( *changeToPrevious )( StateMachine* );
	// Change the State Machine's current State to its default State
	// 
	// ---
	// 
	// StateMachine* stateMachine
	State* ( *changeToDefault )( StateMachine* );
	// Add a State to the State Machine
	// 
	// ---
	// 
	// StateMachine* stateMachine
	// 
	// State* state
	void ( *addState )( StateMachine*, State* );
} StateMachineFn;

typedef struct StateFn {
	// Create a new State
	// 
	// ---
	// 
	// string stateName - A unique identifier for this State
	State* (*new)( const char* );
	// Delete a State
	// 
	// ---
	// 
	// State* state
	void (*delete)( State* );
} StateFn;

extern const StateMachineFn* prismaticStateMachine;
extern const StateFn* prismaticState;

#endif  // STATE_MACHINE_H