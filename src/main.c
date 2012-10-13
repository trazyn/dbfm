/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "handler.h"
#include "config.h"
#include "session.h"
#include "channel.h"
#include "fm.h"
#include "log.h"
#include "playlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <getopt.h>
#include <sys/stat.h>
#include <signal.h>

#define PID_FILE 						"/tmp/dbfm.pid"
	
struct hash **rc;
struct hash **token;
struct hash **station;

static void safe_exit();

static struct user user = { .session = &token, .email = { 0 }, .password = { 0 } };

static void usage();

static int pid_save();

static void pid_clean();

static void daemonize(const char *file_log, const char *file_err);

static void sig_exit(int signo);

int main(int argc, char **argv)
{
	int background = 0, listenfd = 0;
	char cfgfile[FILENAME_MAX];

	struct playlist playlist = { .list = NULL, .position = 0,  .history = NULL };

	const char short_options[] = "hde:p:";
	const struct option long_options[] = 
	{
		{"help", 	no_argument, 		NULL, 'h'},
		{"email", 	required_argument, 	NULL, 'e'},
		{"password", 	required_argument, 	NULL, 'p'},
		{"daemon", 	no_argument, 		NULL, 'd'},
		{NULL, 		0, 			NULL, 	0}
	};

	opterr = 0;

	register int opt_next;

	while(opt_next = getopt_long(argc, (char * const *)argv, short_options, long_options, NULL), -1 != opt_next)
	{
		switch(opt_next)
		{
			case 'h':
				usage();
				break;
			case 'd':
				background = !background;
				break;
			case 'e':
				if(optarg)
				{
					strncpy(user.email, optarg, 32);
				}
				break;
			case 'p':
				if(optarg)
				{
					strncpy(user.password, optarg, 32);
				}
				break;
		}
	}

	if(-1 == pid_save())
	{
		die("dbfm already running...");
	}

	session(&user);
	channels(&station);
	
	snprintf(cfgfile, FILENAME_MAX, "%s/%s/%s", getenv("XDG_CONFIG_HOME"), CFG_PATH, CFG_FILE);
	loadcfg(&rc, cfgfile);

	if(background)
	{
		const char *port = value((const struct hash **)rc, "port");
		unsigned short nport = port ? atoi(port) : 7000;

		if(-1 == (listenfd = tcpsock(nport)))
		{
			exit(EXIT_FAILURE);
		}

		/* run as daemon */
		daemonize(value((const struct hash **)rc, "log"), value((const struct hash **)rc, "err"));
	}
	else
	{
		signal(SIGINT, sig_exit);
	}

	atexit(safe_exit);

	fm_run(&playlist);
	
	handle(listenfd);
}

static void usage()
{
	fprintf(stderr, 
	  	"\n"
	  	"-e, --email, 	 email for login to www.douban.fm\n"
	  	"-p, --password, password\n"
	  	"-d, --daemon,   run as daemon\n"
	  	"-h, --help,     printf help\n"
	  	"\n"
	  );
}

static void daemonize(const char *log, const char *err)
{
	signal(SIGHUP, SIG_IGN);

	switch(fork())
	{
		case -1:
			die("failed to create process: %s", strerror(errno));
		case 0:
			break;
		default:
			exit(EXIT_SUCCESS);
	}

	setsid();

	chdir("/");

	close(STDIN_FILENO);

	stdout = freopen(log ? log : "/dev/null", "a+", stdout);
	stderr = freopen(err ? err : "/dev/null", "a+", stderr);
}

static void safe_exit()
{
	fm_stop();

	/* save config */
	mkcfg((const struct hash **)rc, CFG_FILE);
}

static int pid_save()
{
	if(0 == access(PID_FILE, F_OK))
	{
		return -1;
	}

	FILE *fp = fopen(PID_FILE, "w+");

	chmod(PID_FILE, 0700);

	fprintf(fp, "%d\n", getpid());

	fclose(fp);

	atexit(pid_clean);

	return 0;
}

static void pid_clean()
{
	unlink(PID_FILE);
}

static void sig_exit(int signo)
{
	fprintf(stderr, "press 'Q' to exit, %d\n", getpid());

	exit(EXIT_SUCCESS);
}
