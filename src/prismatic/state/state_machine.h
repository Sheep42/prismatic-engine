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
	StateMachine* ( *new )( State* );
	void ( *delete )( StateMachine* );
	void ( *update )( StateMachine*, float );
	State* ( *changeStateByName )( StateMachine*, char* );
	State* ( *changeState )( StateMachine*, State* );
	State* ( *changeToPrevious )( StateMachine* );
	State* ( *changeToDefault )( StateMachine* );
	void ( *addState )( StateMachine*, State* );
} StateMachineFn;

typedef struct StateFn {
	State* (*new)( const char* );
	void (*delete)( State* );
} StateFn;

extern const StateMachineFn* prismaticStateMachine;
extern const StateFn* prismaticState;

#endif  // STATE_MACHINE_H