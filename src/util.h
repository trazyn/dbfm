/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef UTIL_H
#define UTIL_H

#define BEGIN_WITH(str, s)                          	( strncmp((str), (s), strlen((s))) == 0 )
#define END_WITH(str, s) 			  	( strncmp((str) + strlen((str)) - strlen((s)), (s), strlen((s))) == 0 )
#define EQUAL(str, s) 					( BEGIN_WITH((str), (s)) && strlen((str)) == strlen((s)) )
#define MAX(x, y) 					( (x) > (y) )

#include "hash.h"

enum input_type
{
	INPUT_TEXT = 0,
	INPUT_PASSWORD,
	INPUT_UNKNOW,
};

void duration(void *data, void (*func)(void *), int nsec);

void prompt(const char *text, char *save, int size, enum input_type type);

void echo(int enable);

char *meta(const struct hash **track, const char *fmt, char *buf);

int arridx(const char **arr, char *value);

#endif
