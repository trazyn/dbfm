/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/stat.h>
#include <signal.h>

#include "handler.h"
#include "config.h"
#include "session.h"
#include "channel.h"
#include "fm.h"
#include "log.h"
#include "playlist.h"
#include "util.h"
#include "daemon.h"
#include "hash.h"

static void usage ( void );

static void sig_exit ( int signo );

static void safe_exit ( void );

struct hash **rc;

struct hash **token;

struct hash **station;

static struct user user = { .session = &token, .email = { 0 }, .password = { 0 } };

int main ( int argc, char **argv )
{
	char filename[FILENAME_MAX] = { 0 };
	const char short_options[] = "hnxdDe:p:";

	const struct option long_options[] = 
	{
		{ "help", 	no_argument, 		NULL, 'h' },
		{ "email", 	required_argument, 	NULL, 'e' },
		{ "password", 	required_argument, 	NULL, 'p' },
		{ "daemon", 	no_argument, 		NULL, 'd' },
		{ "exit", 	no_argument, 		NULL, 'x' },
		{ "debug", 	no_argument, 		NULL, 'D' },
		{ NULL, 	0, 			NULL,  0  }
	};

	int background = 0, debug = 0, listenfd = 0;

	struct playlist playlist = { .list = NULL, .position = 0, .length = 0, .history = NULL };

	opterr = 0;

	register int opt_next;

	while ( opt_next = getopt_long ( argc, (char * const *)argv, short_options, long_options, NULL ), -1 != opt_next )
	{
		switch ( opt_next )
		{
			case 'h':
				usage ();
				break;

			case 'd':
				background = !background;
				break;

			case 'e':
				if ( optarg )
				{
					strncpy ( user.email, optarg, 32 );
				}
				break;

			case 'D':
				debug = !debug;
				break;

			case 'x':
				daemonize_kill ();
				break;

			case 'p':
				if ( optarg )
				{
					strncpy ( user.password, optarg, 32 );
				}
				break;
		}
	}

	session ( &user );
	channels ( &station );
	
	/** Apply defaults */
	set ( &rc, "channel", "0" );
	set ( &rc, "port", "7000" );
	set ( &rc, "log", ( snprintf ( filename, FILENAME_MAX, "%s/dbfm.log", rcpath () ), filename ) );
	set ( &rc, "err", ( snprintf ( filename, FILENAME_MAX, "%s/dbfm.err", rcpath () ), filename ) );

	/** If configuration file is not exists then create it */
	mkrc ( NULL, RC_FILENAME );

	if ( loadrc ( &rc, RC_FILENAME ) < 0 ) exit( EXIT_FAILURE );

	if ( background && !debug )
	{
		const char *port = value ( ( const struct hash ** )rc, "port" );
		unsigned short nport = port ? atoi ( port ) : 7000;

		if ( -1 == ( listenfd = tcpsock ( nport ) ) )
		{
			exit ( EXIT_FAILURE );
		}

		signal ( SIGUSR1, SIG_IGN );

		/* run as daemon */
		daemonize ( value ( (const struct hash **)rc, "log" ), value ( (const struct hash **)rc, "err" ) );
	}
	else
	{
		signal ( SIGINT, sig_exit );

		canon ( 0 );

		info ( "Waiting..." );
		info ( "Will be running..." );
	}

	fm_run ( &playlist );
	
	info ( "Running..." );

	atexit ( safe_exit );

	if ( debug )
	{
		signal ( SIGUSR1, SIG_IGN );

		register int ch = 0;

		while( 1 )
		{
			ch = getchar ();

			switch ( ch )
			{
			case 's':
				fm_skip ();
				break;

			case 'n':
				fm_next ();
				break;

			case 'l':
				fm_love ();
				break;

			case 'u':
				fm_unlove ();
				break;

			case 'b':
				fm_ban ();
				break;

			case 'Q':
				exit ( EXIT_SUCCESS );
			}
		}
	}

	/*openlog ( value ( ( const struct hash ** )rc, "log" ), value ( ( const struct hash ** )rc, "err" ) );*/
	handle ( listenfd );
}

static void usage ( void )
{
	fprintf ( stderr, 
	  	"\n"
	  	"-e, --email, 	 Email for login to www.douban.fm\n"
	  	"-p, --password, Password\n"
	  	"-d, --daemon,   Run as daemon\n"
	  	"-x, --exit, 	 Exit"
	  	"-h, --help,     Help\n"
	  	"\n"
	  	);

	exit ( EXIT_SUCCESS );
}


static void sig_exit ( int signo )
{
	fprintf ( stderr, "press 'Q' to exit, %d\n", getpid () );

	exit ( EXIT_SUCCESS );
}

static void safe_exit ( void )
{
	fm_stop ();

	if ( NULL != rc )
	{
		mkrc ( (const struct hash**)rc, RC_FILENAME );
	}

	exit ( EXIT_SUCCESS );
}
