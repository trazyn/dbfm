/*
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 *
 * Copyright (C) 2013 <tn.razy@gmail.com>
 *
 */

#define _GNU_SOURCE

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <time.h>
#include <errno.h>

#include "log.h"
#include "util.h"

typedef struct
{
	unsigned int line;
	char filename[FILENAME_MAX];
} location_t;

static void build_head ( log_type_t type, char * const head, size_t size, const location_t *location );

void openlog ( const char *log, const char *err )
{
	/** Reredict stdout and stderr */
	log = log ? log : "/dev/null";
	err = err ? err : "/dev/null";

	if ( stdout = freopen ( log, "w+", stdout ), NULL == stdout )
	{
		die ( "Failed to open '%s' as stdout: %s", log, strerror ( errno ) );
	}

	if ( EQUAL ( log, err ) )
	{
		stderr = stdout;
	} else
	{
		if ( stderr = freopen ( err, "w+", stderr ), NULL == stderr )
		{
			die ( "Failed to open '%s' as stderr: %s", log, strerror ( errno ) );
		}
	}

	setbuf ( stdout, NULL );
	setbuf ( stderr, NULL );
}

void _log ( log_type_t type, const char *format, ... )
{
	char message[BUFSIZ] = { 0 }, head[512] = { 0 };
	va_list args;
	location_t location;

	va_start ( args, format );
	vsnprintf ( message, BUFSIZ, format, args );
	va_end ( args );

	/** Get the location */
	sscanf ( message, "%d, %s |%*s", &location.line, location.filename );

	/** Skip the location */
	if ( strchr ( message, '|' ) ) 
	{
		snprintf ( message, BUFSIZ, "%s", strchr ( message, '|' ) + 1 );
	}

	build_head ( type, head, sizeof head, &location );

	fprintf ( L_ERROR == type ? stderr : stdout, "%s%s\n", head, message );
}

static void build_head ( log_type_t type, char * const head, size_t size, const location_t *location )
{
	const char *keys[] = { "$time{", "$date{", "$file{", NULL };
	char *begin, *walk, *format, buff[size];
	struct tm *tm;

	tm = localtime ( (time_t []) { time ( NULL ) } );
	tm->tm_year += 1900;
	tm->tm_mon += 1;

	switch ( type )
	{
		case L_ERROR:
			format = DEFAULT_LOG_FMT_ERROR;
			break;

		case L_DEBUG:
			format = DEFAULT_LOG_FMT_DEBUG;
			break;

		case L_WARN:
			format = DEFAULT_LOG_FMT_WARN;
			break;

		default:
			format = DEFAULT_LOG_FMT_INFO;
	}

#ifdef __APPLE__
#define strdupa(string) 		( string )
#endif

	for ( const char **key = keys; *key ; ++key )
	{
		memset ( buff, 0, size );

		begin = walk = format;

		while ( 1 )
		{
			walk = strcasestr ( begin, *key );

			if ( walk )
			{
				/** Copy the normal text */
				strncpy ( buff + strlen ( buff ), begin, walk - begin );

				/** Skip the keyword */
				begin = walk = walk + strlen ( *key );

				/** Validate the expression */
				if ( begin = strchr ( begin, '}' ), begin )
				{
					while ( walk < begin )
					{
						if ( '%' == *walk++ )
						{
							switch ( *walk++ )
							{
								if ( EQUAL ( "$time{" , *key ) )
								{
									case 'H':
										snprintf ( buff, size, "%s%02d", strdupa ( buff ), tm->tm_hour );
										break;

									case 'M':
										snprintf ( buff, size, "%s%02d", strdupa ( buff ), tm->tm_min );
										break;

									case 'S':
										snprintf ( buff, size, "%s%02d", strdupa ( buff ), tm->tm_sec );
										break;
								}
								else if ( EQUAL ( "$date{", *key ) )
								{
									case 'm':
										snprintf ( buff, size, "%s%02d", strdupa ( buff ), tm->tm_mon );
										break;

									case 'd':
										snprintf ( buff, size, "%s%02d", strdupa ( buff ), tm->tm_mday );
										break;

									case 'y':
										snprintf ( buff, size, "%s%d", strdupa ( buff ), tm->tm_year );
										break;
								} else if ( EQUAL ( "$file{", *key ) )
								{
									case 'l':
										snprintf ( buff, size, "%s%d", strdupa ( buff ), location->line );
										break;

									case 'f':
										snprintf ( buff, size, "%s%s", strdupa ( buff ), location->filename );
										break;
								}

								default:
									/** Append the normal text */
									strncat ( buff, walk - 2, 2 );
							}
						} else
							/** Append */
							strncat ( buff, walk - 1, 1 );
					}

					/** Skip "}" and reset walk */
					walk = ++begin;

					/** Continue scan current keyword */
					continue;
				}

				/** Invalid expression and scan the next keyword */
				snprintf ( buff, BUFSIZ, "%s%s%s", strdupa ( buff ), *key, walk );
				break;
			}

			/** Append the tail */
			snprintf ( buff, size, "%s%s", strdupa ( buff ), begin );

			/** End current keyword */
			break;
		}

		/** Next keyword and reset the pattern */
		format = strdupa ( buff );
	}

	snprintf ( head, size, "%s", buff );
}

