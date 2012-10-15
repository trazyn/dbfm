/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "screen.h"
#include "util.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

static int position;

static WINDOW *win;

const char *help[] = 
{
	"A simple client for www.douban.fm",
	"Github: www.github.com/tnrazy/dbfm",
	"\n\n",
	"Key map:",
	"\tn - Play next track",
	"\ts - Skip current playlist",
	"\tl - Mark track as loved",
	"\tu - Unlove track",
	"\tb - Banned current track",
	"\td - Download current track",
	"\tp - Print playlist",
	"\ti - Track info",
	"\tc - Change channel",
	"\tQ - Exit",
	"\t? - Print help"
};

static void sig_resize(int signo);

void scr_help(void *args)
{
	signal(SIGWINCH, sig_resize);

	scr_putline(2, "-", 4, COLS - 4, A_BOLD, 1);
	scr_putline(LINES - 3, "-", 4, COLS - 4, A_BOLD, 1);

	if(NULL == win)
	{
		win = newwin(LINES - 6, COLS - 8, 3, 4);
	}

	wclear(win);

	attron(A_BOLD);

	for(int i = position, line = 1; i < ARRLEN(help);)
	{
		mvwprintw(win, line, 4, "%s", help[i]);

		++i;
		++line;
		++line;
	}

	attroff(A_BOLD);

	wrefresh(win);
}

void scr_scroll_help(int i)
{
	int active = ARRLEN(help) - (LINES - 6) / 2;

	position += i;

	if(position >= 0 && position <= active)
	{
		scr_help(NULL);
	}
	
	if(position < 0)
	{
		position = 0;
	}
	else if(position > active)
	{
		position = active;
	}
}

static void sig_resize(int signo)
{
	delwin(win);

	win = NULL;

	scr_destroy();

	scr_setup();
}
