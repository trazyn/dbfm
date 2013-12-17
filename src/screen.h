/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef SCREEN_H
#define SCREEN_H

#include <ncurses.h>
#include <signal.h>

#define ASCII_HEART 				"heart.text"
#define ASCII_HEARTNO 				"heartno.text"

#define SCREEN_UPDATE() 			raise(SIGWINCH), raise(SIGUSR1)

/* ++++ */
void scr_track(void *args);

void scr_trackprgs(int enable);
/* ++++ */

/* ==== */
void scr_playlist(void *args);

void scr_plscrl(int i);
/* ==== */

/* ---- */
void scr_help(void *args);

void scr_helpscrl(int i);
/* ---- */

struct progress
{
	int line;
	int min;
	int max;
	int position;

	char *done;
	char *undone;

	int attr;
	int color;
};

void scr_start();

void scr_end();

void scr_progress(const struct progress *pgrs);

void scr_putline(unsigned line, char *s, unsigned start, unsigned end, int attr, int color);

void handle_screen();

#endif
