/*
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 *
 * Copyright (C) 2013 <tn.razy@gmail.com>
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

#include "log.h"
#include "util.h"
#include "daemon.h"

static inline char * pidfile_name ( void );

static int pidfile_register ( void );

static void pidfile_destroy ( void );

void daemonize ( const char *log, const char *err )
{
	/** Flush all the file */
	fflush ( NULL );

	/** Signal ignore */
	signal ( SIGHUP, SIG_IGN );

	switch ( fork () )
	{
		case -1:
			die ( "Failed to fork(): %s", strerror ( errno ) );

		case 0:
			break;

		default:
			/** Exit parent process */
			exit ( EXIT_SUCCESS );
	}

	if ( pidfile_register () < 0 )
	{
		exit ( EXIT_FAILURE );
	}

	/** Detach from current session */
	setsid ();

	/** Remove create permission mask */
	umask ( 0 );

	/** Change the current working directory to '/' */
	if ( chdir ( "/" ) < 0 )
	{
		die ( "Failed to change working directory to '/': %s", strerror ( errno ) );
	}

	close ( STDIN_FILENO );
	
	openlog ( log, err );
}

void daemonize_kill ( void )
{
	const char *filename = pidfile_name ();
	FILE *fp;
	pid_t pid;

	if ( fp = fopen ( filename, "r" ), NULL == fp )
	{
		die ( "Failed to open pid file '%s': %s", filename, strerror ( errno ) );
	}

	if ( 1 != fscanf ( fp, "%lu", ( unsigned long * )&pid ) )
	{
		die ( "Failed to read pid from pid file '%s': %s", filename, strerror ( errno ) );
	}

	if ( kill ( pid, SIGTERM ) < 0 )
	{
		die ( "Failed to send SIGTERM to %lu: %s", ( unsigned long )pid, strerror ( errno ) );
	}
	
	exit ( EXIT_SUCCESS );
}

static int pidfile_register ( void ) 
{
	char buff[20], *filename = pidfile_name ();
	struct flock lock;
	int fd;

	lock.l_type = F_WRLCK;
	lock.l_start = 0;
	lock.l_whence = SEEK_SET;
	lock.l_len = 0;

	if ( fd = open ( filename, O_CREAT | O_WRONLY, 0600 ), fd < 0 )
	{
		error ( "Failed to open PID file '%s': %s", filename, strerror ( errno ) );
		return -1;
	}

	/** Lock the pid file */
	if ( fcntl ( fd, F_SETLK, &lock ) < 0 )
	{
		if ( EACCES == errno || EAGAIN == errno )
		{
			if ( fcntl ( fd, F_GETLK, &lock ) < 0 )
			{
				error ( "Failed to read file lock from '%s': %s", filename, strerror ( errno ) );
			} else
			{
				error ( "Already has an instance of %lu", ( unsigned long )lock.l_pid );
			}
			goto failed;
		}

		error ( "Failed to lock pid file '%s': %s", filename, strerror ( errno ) );
		goto failed;
	}

	memset ( buff, 0, 20 );
	snprintf ( buff, 20, "%lu\n", ( unsigned long )getpid() );

	ftruncate ( fd, 0 );

	/** Write the pid */
	write ( fd, buff, strlen ( buff ) );

	/** Destroy pid file by SIGTERM or exit */
	signal ( SIGTERM, ( void * ) pidfile_destroy );
	atexit ( pidfile_destroy );

	return 0;

failed:
	close( fd );
	return -1;
}

static void pidfile_destroy ( void )
{
	unlink ( pidfile_name () );
}

static inline char * pidfile_name ( void )
{
	static char filename[FILENAME_MAX];

	snprintf ( filename, FILENAME_MAX, PID_FILE, getuid () );

	return filename;
}
