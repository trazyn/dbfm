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

#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <locale.h>

typedef void (screen_t)(void *);

static screen_t *current = scr_track;

static void scr_load(screen_t *screen, void *args);

void scr_start()
{
	setlocale(LC_ALL, "");

	initscr();

	raw();

	noecho();

	curs_set(FALSE);

	refresh();

	if(TRUE == has_colors())
	{
		start_color();

		use_default_colors();

		init_pair(0, COLOR_WHITE, -1);
		init_pair(1, COLOR_GREEN, -1);
		init_pair(2, COLOR_RED, -1);
		init_pair(3, COLOR_BLUE, -1);
		init_pair(4, COLOR_CYAN, -1);
		init_pair(5, COLOR_MAGENTA, -1);
		init_pair(6, COLOR_YELLOW, -1);
	}

	scr_load(current, NULL);
}

void scr_end()
{
	endwin();

	clear();
}

void scr_putline(unsigned line, char *s, unsigned start, unsigned end, int attr, int color)
{
	attron(attr | color);

	while(start < end)
	{
		mvprintw(line, start, "%s", s);

		++start;
	}

	attroff(attr | color);

	refresh();
}

void scr_progress(const struct progress *prgs)
{
	if(prgs->position > prgs->max || prgs->position < prgs->min)
	{
		return;
	}

	/* done */
	scr_putline(prgs->line, prgs->done, prgs->min, prgs->position, prgs->attr, prgs->color);

	/* undone */
	scr_putline(prgs->line, prgs->undone, prgs->position + strlen(prgs->done), prgs->max, prgs->attr, prgs->color);
}

void handle_screen()
{
	scr_start();

	atexit(scr_end);

	while(1)
	{
		register int ch = getch();

		switch(ch)
		{
			case 'Q':
				exit(EXIT_SUCCESS);

			case 'j':
				if(scr_help == current)
				{
					scr_helpscrl(1);
				}
				else if(scr_playlist == current)
				{
					scr_plscrl(1);
				}
				break;

			case 'k':
				if(scr_help == current)
				{
					scr_helpscrl(-1);
				}
				else if(scr_playlist == current)
				{
					scr_plscrl(-1);
				}
				break;

			case 'i':
				scr_load(scr_track, NULL);
				break;

			case 'p':
				scr_plscrl(-10000);
				scr_load(scr_playlist, NULL);
				break;

			case 'n':
				fm_next();
				scr_load(scr_track, NULL);
				break;

			case 's':
				fm_skip();
				scr_load(scr_track, NULL);
				break;

			case 'l':
				if(0 == fm_love())
				{
					scr_load(scr_track, NULL);
				}
				break;
				
			case 'd':
				fm_download();
				break;

			case 'u':
				if(0 == fm_unlove())
				{
					scr_load(scr_track, NULL);
				}
				break;

			case 'b':
				fm_ban();
				SCREEN_UPDATE();
				scr_load(scr_track, NULL);
				break;
				
			case '?':
				scr_helpscrl(-10000);
				scr_load(scr_help, NULL);
				break;

			default:
				break;
		}
	}
}

static void scr_load(screen_t *screen, void *args)
{
	current = screen;

	clear();

	if(scr_track != current)
	{
		scr_trackprgs(0);
	}
	else
	{
		scr_trackprgs(1);
	}

	screen(args);
}
