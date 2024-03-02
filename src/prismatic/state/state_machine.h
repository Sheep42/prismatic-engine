// States
#include <stddef.h>
typedef struct State {
	char* name;
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