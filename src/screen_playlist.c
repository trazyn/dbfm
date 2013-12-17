/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "screen.h"
#include "playlist.h"
#include "fm.h"
#include "util.h"

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>

#define FMT 					"%t"

#define COLOR_EXPIRE 				COLOR_YELLOW
#define COLOR_PLAYING 				COLOR_GREEN
#define COLOR_WAITING 				COLOR_CYAN
#define COLOR_SEPARATE 				COLOR_PAIR(3)

static int position;

static WINDOW *win;

static struct playlist opl, *pl = &opl;

static void sig_resize(int signo);

void scr_playlist(void *args)
{
	struct tracknode *nodes;

	signal(SIGWINCH, sig_resize);

	memcpy(pl, fm_plinfo(), sizeof(struct playlist));

	nodes = pl->list;

	scr_putline(2, "=", 4, COLS - 4, A_BOLD, COLOR_SEPARATE);
	scr_putline(LINES - 3, "=", 4, COLS - 4, A_BOLD, COLOR_SEPARATE);

	if(NULL == win)
	{
		win = newwin(LINES - 6, COLS - 8, 3, 4);
		box(win, 0, 0);
	}
	
	wclear(win);

	char text[64] = { 0 };
	int line = 1, idx = 0, attr = 0, color = 0;

	for(struct tracknode *node = nodes; node; node = node->next, ++idx)
	{
		if(idx < position)
		{
			continue;
		}

		memset(&text, 0, 64);

		trackinfo((const struct hash **)node->track, FMT, text, 64);

		mvwprintw(win, line, 4, "%s", text);

		if(idx == pl->position)
		{
			attr = (attr & 0) | A_BOLD;
			color = COLOR_PLAYING;

			mvwaddstr(win, line, COLS - 12 - 9, "[PLAYING]");
		}
		else if(idx < pl->position)
		{
			attr = (attr & 0) | A_UNDERLINE;
			color = COLOR_EXPIRE;

			mvwaddstr(win, line, COLS - 12 - 9, "[EXPIRE ]");
		}
		else
		{
			attr = (attr & 0) | A_BOLD;
			color = COLOR_WAITING;

			mvwaddstr(win, line, COLS - 12 - 9, "[WAITING]");
		}

		mvwchgat(win, line, 4, COLS - 16, attr, color, NULL);

		++line;
		++line;
	}

	wrefresh(win);
}

void scr_plscrl(int i)
{
	int active = pl->length - (LINES - 6) / 2;

	position += i;

	if(position >= 0 && position <= active)
	{
		scr_playlist(NULL);
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

	scr_end();

	scr_start();
}
