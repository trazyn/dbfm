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

#define COLOR_HEART 					COLOR_PAIR(2)
#define COLOR_TITLE 					COLOR_PAIR(5)
#define COLOR_ARTIST 					COLOR_PAIR(1)
#define COLOR_ALBUM 					COLOR_PAIR(1)
#define COLOR_DATE 					COLOR_PAIR(1)

static WINDOW *win;

static struct hash **track;

static void sig_resize(int sino);

void scr_track(void *args)
{
	int line = (LINES - 8) >> 1, attr = COLOR_PAIR(4);
	FILE *fp;
	char text[32], heart[FILENAME_MAX], *v;

	signal(SIGWINCH, sig_resize);

	track = (struct hash **)fm_track();
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

			snprintf(heart, FILENAME_MAX, "%s/%s/heart", getenv("XDG_CONFIG_HOME"), CFG_PATH);
		}
		else
			goto out;
	}
	else
	{
		out:
		snprintf(heart, FILENAME_MAX, "%s/%s/heartno", getenv("XDG_CONFIG_HOME"), CFG_PATH);
	}

	scr_putline(LINES - 1, "─╼ ", 0, COLS, 0, 3);

	/* draw heart */
	{
		wclear(win);

		attron(attr);
		fp = fopen(heart, "r");

		while(fp && !feof(fp))
		{
			if(fgets(text, 32, fp))
			{
				wprintw(win, "%s", text);
			}
		}

		fclose(fp);
		attroff(attr);

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

static void sig_resize(int signo)
{
	delwin(win);

	win = NULL;

	scr_destroy();

	scr_setup();
}
