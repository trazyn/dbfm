/*
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 *
 * Copyright (C) 2013 <tn.razy@gmail.com>
 *
 */

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <sys/stat.h>


/** For PATH_MAX */
#ifdef __APPLE__
	#include <sys/syslimits.h>
#elif __linux__
	#include <linux/limits.h>
#endif

#include "log.h"
#include "config.h"
#include "util.h"

int loadrc ( struct hash *** const rc, const char *filename )
{
	FILE *fp;
	char fullname[FILENAME_MAX] = { 0 };

	/** Failed to build rc path */
	if ( NULL == rcpath () )
	{
		exit ( EXIT_FAILURE );
	}

	snprintf ( fullname, FILENAME_MAX, "%s/%s", rcpath (), filename );

	fp = fopen ( fullname, "r" );

	if ( NULL == fp )
	{
		error ( "Failed to open file '%s': %s", fullname );
		return -1;
	}

	/** Parse the configuration file */
	size_t size = 0;
	char *line = NULL, *ptr, key[16] = { 0 }, value[256] = { 0 };

	register unsigned nline = 0;

	while ( !feof ( fp ) )
	{
		++nline;

		/** Too short */
		if ( getline ( &line, &size, fp ) < 4 )
		{
			goto ignore;
		}

		/** For free */
		ptr = line;

		/** Trim the blank */
		while ( isspace ( *ptr++ ) );

		/** Skip comment line */
		if ( '#' == *--ptr )
		{
			goto ignore;
		}

		/** Parse key value */
		if ( 2 != sscanf ( ptr, "%15[^= \t] = %255[^\r\n]", key, value ) )
		{
			warn ( "Skip invalid line: %d, %s", nline, line );
			goto ignore;
		}

		/** Store the key value */
		set ( rc, key, value );

ignore:
		if ( size ) 
		{
			free ( line );

			size = 0;
			line = NULL;
		}
	}

	info ( "Load rc(%s) success!", fullname );

	return 0;
}

int mkrc ( const struct hash ** rc, const char *filename )
{
#ifdef __APPLE__
	char *string = NULL;
#define strdupa(s)			( string = alloca ( strlen ( (s) ) + 1 ), strcpy ( string, (s) ), string )
#endif

	char fullname[FILENAME_MAX] = { 0 }, *s, *p;

	snprintf ( fullname, FILENAME_MAX, "%s", rcpath () );

	s = strdupa ( filename );

	while ( p = strchr ( s, '/' ), p )
	{
		/** Skip 1th '/' */
		if ( p - s )
		{
			/** Mark the child directory name */
			*p = 0;

			/** Build the child directory */
			snprintf ( fullname, FILENAME_MAX, "%s/%s", ( strdupa ( fullname ) ), s );

			if ( mkdir ( fullname, RC_PATH_MODE ) < 0 && EEXIST != errno )
			{
				error ( "Failed to create directory '%s': %s", fullname, strerror ( errno ) );
				return -1;
			}
		}

		/** Do next */
		s = ++p;
	}

	/** Generate the filename */
	snprintf ( fullname, FILENAME_MAX, "%s/%s", ( strdupa ( fullname ) ), s );

	if ( !ENDWITH ( filename, "/" ) )
	{
		FILE *fp = fopen ( fullname, rc ? "w+" : "a" );
		struct hash *item = NULL;
		struct stat st;

		if ( NULL == fp )
		{
			error ( "Failed to open '%s': %s", fullname, strerror ( errno ) );
			return -1;
		}

		fstat ( fileno ( fp ), &st );

		if ( !S_ISREG ( st.st_mode ) )
		{
			fclose ( fp );
			error ( "File '%s' is not an regular file.", fullname );

			return -1;
		}

		if ( rc )
		{
			while ( item = ( struct hash * )*rc++, item )
			{
				fprintf ( fp, "%s = %s", item->key, item->value );

				fputs ( "\r\n\r\n", fp );
			}
		}
		fclose ( fp );
	}

	return 0;
}

const char *rcpath ( void )
{
	static char path[PATH_MAX] = { 0 };

	/** Whether has been initialization */
	if ( !*path )
	{
		/** Avoid the directory of "(null)" has been created */
		char *cwd = getcwd ( NULL, 0 ), *fallback = NULL;

		/** Check XDG_CONFIG_HOME directory */
		snprintf ( path, PATH_MAX, "%s", getenv ( "XDG_CONFIG_HOME" ) );
		
		/** Change working directory to $XDG_CONFIG_HOME */
		if ( chdir ( path ) < 0 )
		{
			error ( "Failed to change working directory to XDG_CONFIG_HOME." );

			goto fallback;
		}

		if ( mkdir ( RC_XDG_PATH, RC_PATH_MODE ) < 0 && EEXIST != errno )
		{
			error ( "Failed to create directory '%s': %s", RC_XDG_PATH, strerror ( errno ) );

			goto fallback;
		}

		snprintf ( path, PATH_MAX, "%s/%s", getenv ( "XDG_CONFIG_HOME" ), RC_XDG_PATH );

		fallback = path;
fallback:
		chdir ( cwd );
		free ( cwd );

		/** Default use $HOME path */
		if ( !fallback )
		{
			snprintf ( path, PATH_MAX, "%s/%s", getenv ( "HOME" ), RC_HOME_PATH );

			if ( mkdir ( path, RC_PATH_MODE ) < 0 && EEXIST != errno )
			{
				error ( "Failed to create directory '%s': %s", RC_HOME_PATH, strerror ( errno ) );

				return NULL;
			}
		}
	}

	return path;
}
