/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "screen.h"
#include "fm.h"
#include "config.h"
#include "util.h"

#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <ctype.h>
#include <time.h>

#define COLOR_HEART 					COLOR_PAIR(2)
#define COLOR_TITLE 					COLOR_PAIR(5)
#define COLOR_ARTIST 					COLOR_PAIR(1)
#define COLOR_ALBUM 					COLOR_PAIR(1)
#define COLOR_DATE 					COLOR_PAIR(1)
#define COLOR_PRGS 					COLOR_PAIR(0)

static WINDOW *win;

static struct hash **track;

static void sig_resize(int sino);

static void scr_drawprgs(int ignore);

static void scr_resetprgs(int ignore);

static time_t tm = 0;

void scr_track(void *args)
{
	int line = (LINES - 8) >> 1, attr = COLOR_PAIR(4);
	FILE *fp;
	char text[32], heart[FILENAME_MAX], *v;

	signal(SIGWINCH, sig_resize);

	track = (struct hash **)fm_trackinfo();
	v = (char *)value((const struct hash **)track, "like");

	if(NULL == win)
	{
		win = newwin(13, 32, (LINES - 13) >> 1, 4);
	}

	if(v)
	{
		if(1 == atoi(v))
		{
			attr = (attr & 0) | COLOR_HEART | A_BOLD;

			snprintf(heart, FILENAME_MAX, "%s/%s/heart.text", getenv("XDG_CONFIG_HOME"), CFG_PATH);
		}
		else
			goto out;
	}
	else
	{
		out:
		snprintf(heart, FILENAME_MAX, "%s/%s/heartno.text", getenv("XDG_CONFIG_HOME"), CFG_PATH);
	}

	refresh();

	/* draw heart */
	{
		wclear(win);
		wbkgdset(win, attr);

		fp = fopen(heart, "r");

		register int ch = 0;

		while(fp && !feof(fp))
		{
			ch = fgetc(fp);

			if(isascii(ch))
			{
				wprintw(win, "%c", ch);
			}
		}

		fclose(fp);
		wrefresh(win);
	}

	/* print track info */
	{
		/* ... title */
		attron(A_BOLD | COLOR_PAIR(5));
		memset(&text, 0, 32);
		mvprintw(line, COLS - 38, "%s", trackinfo((const struct hash **)track, "%32t", text, 32));
		attroff(A_BOLD | COLOR_PAIR(5));

		line += 2;

		attron(COLOR_PAIR(1));

		/* ... artist */
		memset(&text, 0, 32);
		mvprintw(line, COLS - 38, "%s", trackinfo((const struct hash **)track, "%32a", text, 32));

		line += 2;

		/* ... album */
		memset(&text, 0, 32);
		mvprintw(line, COLS - 38, "%s", trackinfo((const struct hash **)track, "%32b", text, 32));

		line += 2;

		/* ... date */
		memset(&text, 0, 32);
		mvprintw(line, COLS - 38, "%s", trackinfo((const struct hash **)track, "%32y", text, 32));

		attroff(COLOR_PAIR(1));
	}

	refresh();
}

void scr_trackprgs(int enable)
{
	struct sigaction draw;
	struct sigaction retm;

	if(enable)
	{
		sigemptyset(&draw.sa_mask);
		draw.sa_flags = SA_RESTART;
		draw.sa_handler = scr_drawprgs;

		sigemptyset(&retm.sa_mask);
		retm.sa_flags = SA_RESTART;
		retm.sa_handler= scr_resetprgs;

		sigaction(SIGALRM, &draw, NULL);
		sigaction(SIGUSR1, &retm, NULL);

		if(time(NULL) - tm > 10000000)
		{
			tm = time(NULL);
		}

		alarm(1);
	}
	else
	{
		signal(SIGALRM, SIG_IGN);
		alarm(0);
	}
}


static void scr_drawprgs(int ignore)
{
	struct progress prgs = { LINES - 1, 0, COLS, 0, "─╼", " ", A_BOLD, COLOR_PRGS };

	char length[8] = { 0 };

	trackinfo(fm_trackinfo(), "%l", length, 8);

	prgs.position = ((float)COLS / atoi(length)) * (time(NULL) - tm);

	scr_progress(&prgs);

	alarm(1);
}

static void scr_resetprgs(int ignore)
{
	tm = time(NULL);
}

static void sig_resize(int signo)
{
	delwin(win);

	win = NULL;

	scr_end();

	scr_start();
}

