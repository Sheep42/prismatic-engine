typedef struct Logger {
	void ( *info )( const char* );
	void ( *debug )( const char* );
	void ( *error )( const char* );
	void ( *infof )( const char*, ... );
	void ( *debugf )( const char*, ... );
	void ( *errorf )( const char*, ... );
} Logger;

extern const Logger* prismaticLogger;