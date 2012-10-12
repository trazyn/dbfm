/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef CONFIG_H
#define CONFIG_H

#include "hash.h"

#define CFG_PATH 				"dbfm"
#define CFG_FILE 				"dbfm.conf"

struct user
{
	struct hash ***session;

	char email[32], password[32];
};

void loadcfg(struct hash ***arr, const char *file);

void mkcfg(const struct hash **arr, const char *filename);

#endif
