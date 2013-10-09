/*
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 *
 * Copyright (C) 2013 <tn.razy@gmail.com>
 *
 */


#ifndef _LOG_H
#define _LOG_H

#include <stdio.h>

#define STR2(X) 			#X
#define STR(X) 				STR2(X)
#define AT 				STR(__LINE__) "," __FILE__

/**
 * Default log prefix, eg:
 * 	05-10-2013 22:11:11, [22L, utils.c]
 * */
#define DEFAULT_LOG_FMT 		"$date{%d-%m-%y} $time{%H:%M:%S}, [$file{%lL, %f}] "

/**
 * eg: 
 * 	05-10-2013 22:10:11, [17L, main.c] ERROR: There has an error be occurred.
 * */
#define DEFAULT_LOG_FMT_ERROR 		DEFAULT_LOG_FMT "ERROR: "

#define DEFAULT_LOG_FMT_DEBUG 		DEFAULT_LOG_FMT "DEBUG: "

#define DEFAULT_LOG_FMT_WARN 		DEFAULT_LOG_FMT "WARN : "

#define DEFAULT_LOG_FMT_INFO 		DEFAULT_LOG_FMT "INFO : "

typedef enum
{
	L_ERROR = 0,
	L_DEBUG,
	L_WARN,
	L_INFO
} log_type_t;

void openlog ( const char *log, const char *err );

void _log ( log_type_t type, const char *fmt, ... );

#define error( ... ) 		_log( L_ERROR, AT " |" __VA_ARGS__ )
#define debug( ... ) 		_log( L_DEBUG, AT " |" __VA_ARGS__ )
#define warn( ... ) 		_log( L_WARN,  AT " |" __VA_ARGS__ )
#define info( ... ) 		_log( L_INFO,  AT " |" __VA_ARGS__ )

#define die( ... ) 		_log( L_ERROR, __VA_ARGS__ ); 		\
				exit ( EXIT_FAILURE )

#endif
