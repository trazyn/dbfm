/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef PLAY_h
#define PLAY_h

#include <stdio.h>
#include <ao/ao.h>

struct stream
{
	FILE *fp;

	int driver_id;
	ao_device *device;
	ao_sample_format fmt;
};

void play(const char *location);

#endif
