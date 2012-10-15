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
#include <locale.h>

static screen_t *current = scr_track;

void scr_setup()
{
	atexit(scr_destroy);

	setlocale(LC_ALL, "");

	initscr();

	cbreak();

	noecho();

	curs_set(FALSE);

	refresh();

	if(TRUE == has_colors())
	{
		start_color();

		init_pair(1, COLOR_GREEN, 0);
		init_pair(2, COLOR_RED, 0);
		init_pair(3, COLOR_BLUE, 0);
		init_pair(4, COLOR_CYAN, 0);
		init_pair(5, COLOR_MAGENTA, 0);
		init_pair(6, COLOR_YELLOW, 0);
	}

	scr_load(current, NULL);
}

void scr_destroy()
{
	clear();

	endwin();
}

void scr_load(screen_t *screen, void *args)
{
	current = screen;

	clear();

	screen(args);
}

void scr_putline(unsigned line, char *s, unsigned start, unsigned end, int attr, int color)
{
	attron(attr | COLOR_PAIR(color));

	while(start < end)
	{
		mvprintw(line, start, "%s", s);

		++start;
	}

	attroff(attr | COLOR_PAIR(color));

	refresh();
}

void handle_screen()
{
	atexit(scr_destroy);

	scr_setup();

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
					scr_scroll_help(1);
				}
				else if(scr_playlist == current)
				{
					scr_scroll_pl(1);
				}
				break;

			case 'k':
				if(scr_help == current)
				{
					scr_scroll_help(-1);
				}
				else if(scr_playlist == current)
				{
					scr_scroll_pl(-1);
				}
				break;

			case 'i':
				scr_load(scr_track, NULL);
				break;

			case 'p':
				scr_scroll_pl(-10000);
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

			case 'u':
				if(0 == fm_unlove())
				{
					scr_load(scr_track, NULL);
				}
				break;

			case 'b':
				fm_ban();
				scr_load(scr_track, NULL);
				break;
				
			case '?':
				scr_scroll_help(-10000);
				scr_load(scr_help, NULL);
				break;

			default:
				break;
		}
	}
}

