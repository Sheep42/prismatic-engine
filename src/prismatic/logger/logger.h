#ifndef LOGGER_H
#define LOGGER_H

typedef struct Logger {
	// Log an informative message to the Console
	// 
	// ----
	// 
	// const char* msg
	void ( *info )( const char* );

	// Log a debug message to the Console
	// 
	// ----
	// 
	// const char* msg
	void ( *debug )( const char* );

	// Log an error message to the Console
	// 
	// ----
	// 
	// const char* msg
	void ( *error )( const char* );

	// Log an informative message to the Console with printf-style formatting
	// 
	// ----
	// 
	// const char* msg
	void ( *infof )( const char*, ... );

	// Log a debug message to the Console with printf-style formatting
	// 
	// ----
	// 
	// const char* msg
	void ( *debugf )( const char*, ... );

	// Log an error message to the Console with printf-style formatting
	// 
	// ----
	// 
	// const char* msg
	void ( *errorf )( const char*, ... );
} Logger;

extern const Logger* prismaticLogger;

#endif // LOGGER_H