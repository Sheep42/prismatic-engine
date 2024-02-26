typedef struct Game {
	void ( *update )( float );
} Game;

Game* initGame( void );