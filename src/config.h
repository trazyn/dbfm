/*
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 *
 * Copyright (C) 2013 <tn.razy@gmail.com>
 *
 */


#ifndef _CONFIG_H
#define _CONFIG_H

#include "hash.h"

#define RC_XDG_PATH 				"doubanFM"
#define RC_HOME_PATH 				".doubanFM"
#define RC_PATH_MODE 				0755

#define RC_FILENAME 				"dbfm.rc"

struct user
{
	struct hash ***session;

	char email[32], password[32];
};

int loadrc ( struct hash *** const rc, const char *filename );

int mkrc ( const struct hash ** rc, const char *filename );

void destroyrc ( struct hash ** const rc );

const char *rcpath ( void );

#endif
