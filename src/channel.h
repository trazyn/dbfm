/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef CHANNEL_H
#define CHANNEL_H

#include "hash.h"

#define CHANNEL_DEF 					0
#define CHANNEL_API 					"http://www.douban.com/j/app/radio/channels"
#define CHANNEL_FILE 					"channels"

void channels(struct hash ***arr);

#endif
