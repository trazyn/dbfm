/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef FM_H
#define FM_H

#include "playlist.h"

#include <unistd.h>

void fm_run(struct playlist *pl);

void fm_next();

void fm_skip();

void fm_ban();

int fm_love();

int fm_unlove();

int fm_recording();

void fm_download();

void fm_list();

void fm_channel();

void fm_stop();

const struct playlist *fm_playlist();

const struct hash **fm_track();

#endif
