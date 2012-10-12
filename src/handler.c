/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "handler.h"
#include "hash.h"
#include "util.h"
#include "fm.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

extern struct hash **station;
extern struct hash **rc;

static void help();

void handler(int listenfd)
{
	int ch = 0;
	char channel[16] = { 0 };

	if(0 == listenfd)
	{
		echo(0);

		/* register keyboard */
		while(1)
		{
			ch = getchar();

			switch(ch)
			{
				case 'n':
					fm_next();
					break;
				case 's':
					fm_skip();
					break;
				case 'l':
					fm_love();
					break;
				case 'u':
					fm_unlove();
					break;
				case 'b':
					fm_ban();
					break;
				case 'd':
					fm_download();
					break;
				case 'c':
					prompt("please type channel ID: ", channel, 16, INPUT_TEXT);
					if(NULL != value((const struct hash **)station, channel))
					{
						erase(&rc, "channel");
						set(&rc, "channel", channel);
						fm_channel();
					}
					echo(0);
					break;
				case 'p':
					fm_list();
					break;
				case 'Q':
					fm_stop();
					exit(EXIT_SUCCESS);
					break;
				
				default:
					help();
					break;
			}
		}
	}
}

static void help()
{
	fprintf(stderr, 
	  	"\n"
	  	"n - play next track \t\t s - skip track\n"
	  	"l - love this track \t\t u - unlove track\n"
	  	"b - baned this track\t\t d - download\n"
	  	"p - printf playlist \t\t c - change channel\n"
	  	"Q - exit            \t\t ? - help\n"
		"\n"
	  );
}
