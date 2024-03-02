#ifndef GAME_H

#define GAME_H

typedef struct Game {
	void ( *update )( float );
	void ( *init )( void );
	void ( *draw )( float );
	void ( *destroy )( void );
} Game;

Game* newGame( void );

#endif // GAME_H