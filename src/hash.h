/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef HASH_H
#define HASH_H

struct hash
{
	char *key;
	char *value;
};


void set(struct hash ***arr, const char *key, const char *value);

const char *value(const struct hash **hash, const char *key);

void erase(struct hash ***arr, const char *key);

void cleanup(struct hash **ptr);

int length(const struct hash **arr);

#endif
