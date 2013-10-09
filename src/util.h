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

#include <stdio.h>

#define EQUAL( a, b ) 					( 0 == strcmp( (a), (b) ) )
#define STARTWITH( string, start ) 			( 0 == strncmp( (string), (start), strlen( (start) ) ) )
#define ENDWITH( string, end ) 				EQUAL( ( (string) + strlen( ( string ) ) - strlen( (end) ) ), (end) )

#define MAX(x, y) 					( (x) > (y) )
#define ARRLEN(arr) 					sizeof (arr) / sizeof *(arr)

#include "hash.h"

enum input_type
{
	INPUT_TEXT = 0,
	INPUT_PASSWORD,
	INPUT_UNKNOW,
};

void duration(void *data, void (*func)(void *), int nsec);

void prompt(const char *text, char *save, int size, enum input_type type);

void canon(int enable);

char *trackinfo(const struct hash **track, const char *fmt, char *buf, int size);

int arridx(const char **arr, char *value);

void str_replace ( char * const str, size_t size, const char *need, const char *to );

#endif
