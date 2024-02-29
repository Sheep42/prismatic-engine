// States
#include <stddef.h>
typedef struct State {
	char* name;
	void (*enter)(void);
	void (*exit)(void);
	void (*tick)(void);
} State;

// State Machines
typedef struct StateMachine {
	size_t totalStates;
	State* states;
	State* defaultState;
	State* previousState;
	State* currentState;
} StateMachine;

typedef struct StateMachineFn {
	StateMachine* (*new)(State*);
	void (*delete)(StateMachine*);
	void (*update)(StateMachine*, float);
	State* (*changeStateByName)( StateMachine*, char* );
	State* (*changeState)( StateMachine*, State* );
	State* (*changeToPrevious)( StateMachine* );
	State* (*changeToDefault)( StateMachine* );
	void (*addState)( StateMachine*, State* );
} StateMachineFn;

extern const StateMachineFn* prismaticStateMachine;