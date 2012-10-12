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
#include "playlist.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <getopt.h>
	
struct hash **rc;
struct hash **token;
struct hash **station;

static struct user user = { .session = &token, .email = { 0 }, .password = { 0 } };

static void usage();

int main(int argc, char **argv)
{
	int background = 0, listenfd = 0;
	char cfgfile[FILENAME_MAX];
	struct playlist playlist;

	const char short_options[] = "hde:p:";
	const struct option long_options[] = 
	{
		{"help", 	no_argument, 		NULL, 'h'},
		{"email", 	required_argument, 	NULL, 'e'},
		{"password", 	required_argument, 	NULL, 'p'},
		{"daemon", 	no_argument, 		NULL, 'd'}
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

	session(&user);
	channels(&station);
	
	snprintf(cfgfile, FILENAME_MAX, "%s/%s/%s", getenv("XDG_CONFIG_HOME"), CFG_PATH, CFG_FILE);
	loadcfg(&rc, cfgfile);

	if(background)
	{
	
	}

	memset(&playlist, 0, sizeof playlist);

	fm_run(&playlist);
	
	handler(listenfd);
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
