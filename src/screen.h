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

typedef void (screen_t)(void *);

void scr_setup();

void scr_destroy();

void scr_track(void *args);

void scr_playlist(void *args);

void scr_help(void *args);

void scr_load(screen_t *screen, void *args);

void scr_putline(unsigned line, char *s, unsigned start, unsigned end, int attr, int color);

void scr_scroll_help(int i);

void scr_scroll_pl(int i);

void handle_screen();

#endif
