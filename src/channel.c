/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "channel.h"
#include "config.h"
#include "http.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <json/json.h>

static void channels_parse(struct hash ***arr, struct json_object *obj);

void channels(struct hash ***arr)
{
	char **resp;

	if(resp = fetch(CHANNEL_API, NULL, NULL, NULL), NULL == resp)
	{
		die("failed to get channels");
	}

	channels_parse(arr, json_tokener_parse(*resp));

	free_response(resp);

	mkrc((const struct hash **)*arr, CHANNEL_FILE);
}

static void channels_parse(struct hash ***arr, struct json_object *obj)
{
	array_list *channels = json_object_get_array(json_object_object_get(obj, "channels"));

	for(int i = channels->length - 1; i >= 0; i--)
	{
		struct json_object *o = (struct json_object *)array_list_get_idx(channels, i);

		const char *key = json_object_get_string(json_object_object_get(o, "seq_id"));
		const char *value = json_object_get_string(json_object_object_get(o, "name"));

		set(arr, key, value);
	}

	set(arr, "-3", "heart radio");
}

