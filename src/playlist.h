/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef PLAYLIST_H
#define PLAYLIST_H

#define PLAYLIST_API 						"http://www.douban.com/j/app/radio/people?app_name=radio_desktop&version=100&user_id=%s&expire=%s&token=%s&channel=%d&sid=%d&type=%c&h=%s"

#include "hash.h"
#include "api.h"

struct tracknode
{
	struct hash **track;

	struct tracknode *next;
};

struct playlist
{
	struct tracknode *list;

	struct hash **history;

	int position;
	int length;
};

const struct hash **pl_current(struct playlist *pl);

void pl_destroy(struct playlist *pl);

void pl_history(struct playlist *pl, enum cmd_type type, int sid);

#endif
