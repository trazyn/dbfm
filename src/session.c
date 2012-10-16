/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "log.h"
#include "util.h"
#include "http.h"
#include "session.h"
#include "config.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <json/json.h>
#include <pthread.h>
#include <sys/stat.h>

static void session_parse(struct hash ***arr, struct json_object *obj);

static pthread_mutex_t session_mutex = PTHREAD_MUTEX_INITIALIZER;

void session(void *data)
{
	pthread_mutex_lock(&session_mutex);

	struct user *user = (struct user *)data;

	char post[512], sessionfile[FILENAME_MAX], **resp = NULL;
	struct json_object *obj;
	struct stat st;
	
	static int keep = 0;

	snprintf(sessionfile, FILENAME_MAX, "%s/%s/%s", getenv("XDG_CONFIG_HOME"), CFG_PATH, SESSION_FILE);

	/* session file is exists and session not expire */
	if(!access(sessionfile, F_OK | R_OK))
	{
		loadcfg(user->session, sessionfile);

		if(atol(value((const struct hash **)*user->session, "expire")) > time(NULL))
		{
			/* session file is exists and sesison not expire */
			goto keeping;
		}
		else
		{
			cleanup(*user->session);
			*user->session = NULL;
		}
	}
	
login:
	if(user->email && user->password)
	{
		snprintf(post, 512, SESSION_POST, user->email, user->password);

		resp = fetch(SESSION_API, NULL, post, NULL);

		if(NULL == resp)
		{
			_ERROR("failed to get session");

			return;
		}

		obj = json_tokener_parse(*resp);

		free_response(resp);
	}

	const char *err = json_object_get_string(json_object_object_get(obj, "err"));

	if(!EQUAL("ok", err))
	{
		fputs("\r\n", stderr);

		_ERROR("failed to get sesison: %s", err);

		/* if in background exit */
		if(fstat(STDIN_FILENO, &st) || !S_ISCHR(st.st_mode))
		{
			die("no console");
		}

		/* get email and password from input */
		prompt("Please input your email and password.\n", NULL, 0, 0);

		prompt("Email: ", user->email, sizeof user->email, INPUT_TEXT);
		prompt("Password:", user->password, sizeof user->password, INPUT_PASSWORD);

		goto login;
	}

	session_parse(user->session, obj);
	mkcfg((const struct hash **)*user->session, sessionfile);

keeping:
	if(!keep)
	{
		/* keep session valid */
		keep = !keep;
		duration(data, session, 30 * 60);
	}

	pthread_mutex_unlock(&session_mutex);
}

static void session_parse(struct hash ***arr, struct json_object *obj)
{
	set(arr, "user_id", json_object_get_string(json_object_object_get(obj, "user_id")));
	set(arr, "token", json_object_get_string(json_object_object_get(obj, "token")));
	set(arr, "expire", json_object_get_string(json_object_object_get(obj, "expire")));
	set(arr, "user_name", json_object_get_string(json_object_object_get(obj, "user_name")));
}

