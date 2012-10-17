/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

//#define ___DEBUG

#include "api.h"
#include "log.h"
#include "http.h"
#include "hash.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <string.h>

extern struct hash **rc;
extern struct hash **token;

static struct hash ***session = &token;

char **api_send_request(enum cmd_type type, int sid, char *history)
{
	char url[1024] = { 0 }, *channel;

	if(NULL == *session)
	{
		die("session is null");
	}

	if(NULL == rc)
	{
		die("config is null");
	}

	channel = (char *)value((const struct hash **)rc, "channel");

	snprintf(url, 1024, API, 	value((const struct hash **)*session, "user_id"),
	  				value((const struct hash **)*session, "expire"),
	  				value((const struct hash **)*session, "token"),
					channel ? atoi(channel) : 0, sid, type
	  	);

	switch((int)type)
	{
		case CMD_INITPL:
			strcat(url, "&h=");
			break;

		case CMD_BAN:
		case CMD_SKIP:
		case CMD_REFRESH:
			strcat(url, "&h=");
			strcat(url, history ? history : "");
			break;
	}

	_DEBUG("URL: %s\n", url);

	return fetch(url, NULL, NULL, NULL);
}
