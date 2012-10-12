/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void set(struct hash ***arr, const char *key, const char *value)
{
	int nitem = length((const struct hash **)*arr) + 1;

	*arr = realloc(*arr, (nitem + 1) * sizeof **arr);

	struct hash *item = calloc(sizeof item, 1);
	
	item->key = strdup(key);
	item->value = strdup(value ? value : "");
	
	(*arr)[--nitem] = item;
	(*arr)[++nitem] = NULL;
}


void cleanup(struct hash **ptr)
{
	if(NULL == ptr)
	{
		return;
	}

	struct hash **p = ptr;

	while(*ptr)
	{
		struct hash *item = *ptr;

		if(item->key)
		{
			free(item->key);
		}

		if(item->value)
		{
			free(item->value);
		}

		free(item);
		++ptr;
	}

	free(p);
}

const char *value(const struct hash **arr, const char *key)
{
	struct hash *item;

	while( arr && (item = (struct hash *)*arr) )
	{
		if(0 == strcmp(item->key, key))
		{
			return item->value;
		}

		++arr;
	}

	return NULL;
}

int length(const struct hash **arr)
{
	if(NULL == arr)
	{
		return 0;
	}

	struct hash **ptr = (struct hash **)arr;

	register int nitem = 0;

	while(*ptr++)
	{
		++nitem;
	}

	return nitem;
}

void erase(struct hash ***arr, const char *key)
{
	register int idx = 0;
	struct hash *item = NULL, **ptr = *arr;

	while(*arr && (item = *ptr++))
	{
		if(0 == strncmp(key, item->key, strlen(key)))
		{
			free(item->key);
			free(item->value);
			free(item);

			break;
		}

		++idx;
	}

	while(item && *ptr++)
	{
		(*arr)[idx] = (*arr)[idx + 1];

		++idx;
	}

	if(idx)
	{
		(*arr)[idx] = 0;
	}
}
