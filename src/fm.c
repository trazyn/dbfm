/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "fm.h"
#include "api.h"
#include "play.h"
#include "playlist.h"
#include "log.h"
#include "util.h"
#include "http.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <fcntl.h>
#include <stdio.h>
#include <signal.h>
#include <sys/wait.h>
#include <pthread.h>

extern struct hash **rc;

static struct playlist *list;

static struct hash **current;

static pid_t playproc = 0;

static char ack = CMD_NEXT;

#define SID 						atoi(value((const struct hash **)current, "sid"))
#define URL 						value((const struct hash **)current, "url")
#define LOVE 						atoi(value((const struct hash **)current, "like"))
#define TITLE 						value((const struct hash **)current, "title")
#define ARTIST 						value((const struct hash **)current, "artist")

static void *dl_thread(void *data);

void fm_run(struct playlist *pl)
{
	int status;

	/* make sure only one instance of play process */
	if(playproc > 0)
	{
		signal(SIGCHLD, SIG_DFL);

		kill(playproc, SIGUSR1);

		while(playproc != waitpid(playproc, &status, 0))
		{
			if(ESRCH == errno || ECHILD == errno)
			{
				break;
			}

			exit(EXIT_FAILURE);
		}

		if(WIFEXITED(status) && EXIT_SUCCESS == WEXITSTATUS(status))
		{
			pl_history(list, ack, SID);

			ack = CMD_NEXT;

			fm_recording();
		}
		else
		{
			pl_history(list, CMD_SKIP, SID);
		}
	}

	list = pl;
	current = (struct hash **)pl_current(list);

	switch((playproc = fork()))
	{
		case -1:
			die("failed to fork process");

		case 0:
			signal(SIGINT, SIG_IGN);
			signal(SIGUSR1, SIG_DFL);

			close(STDIN_FILENO);

			play(URL);

			exit(EXIT_SUCCESS);
		default:
			break;
	}

	signal(SIGCHLD, fm_next);
}

void fm_next()
{
	list->position++;

	fm_run(list);
}

void fm_skip()
{
	list->position = -1;

	fm_run(list);
}

int fm_love(struct playlist *pl)
{
	char **resp = NULL;

	if(!LOVE)
	{
		ack = CMD_LOVE;

		resp = api_send_request(CMD_LOVE, SID, NULL);

		if(NULL == resp)
		{
			return -1;
		}

		reset(&current, "like", "1");

		free_response(resp);
	}

	return 0;
}

int fm_unlove()
{
	char **resp = NULL;

	if(LOVE)
	{
		ack = CMD_UNLOEV;

		resp = api_send_request(CMD_UNLOEV, SID, NULL);

		if(NULL == resp)
		{
			return -1;
		}

		reset(&current, "like", "0");

		free_response(resp);
	}

	return 0;
}

void fm_ban()
{
	pl_history(list, CMD_BAN, SID);

	list->position = -1;

	fm_run(list);
}

int fm_recording()
{
	char **resp = api_send_request(CMD_NEXT, SID, NULL);

	if(NULL == resp)
	{
		return -1;
	}

	if(!EQUAL(*resp, "ok"))
	{
		free_response(resp);

		return -1;
	}

	free_response(resp);

	return 0;
}

void fm_download()
{
	pthread_t thread;

	pthread_create(&thread, NULL, dl_thread, NULL);
}

static void *dl_thread(void *data)
{
	char *dlpath = (char *)value((const struct hash **)rc, "download");

	char filename[FILENAME_MAX] = { 0 }, buf[BUFSIZ << 2];

	FILE *handle = NULL, *fp;

	if(NULL == dlpath)
	{
		return NULL;
	}

	snprintf(filename, FILENAME_MAX, "%s/%s - %s.mp3", dlpath, ARTIST, TITLE);

	fetch(URL, &handle, NULL, NULL);

	if(NULL == handle)
	{
		return NULL;
	}

	if(fp = fopen(filename, "w+"), NULL == fp)
	{
		return NULL;
	}

	register int nbyte = 0;

	while(nbyte = timeout_read(fileno(handle), buf, sizeof buf, 5), nbyte > 0)
	{
		if(1 != fwrite(buf, nbyte, 1, fp))
		{
			_ERROR("failed to download file: %s", strerror(errno));
			unlink(filename);
			break;
		}
	}

	fclose(handle);
	fclose(fp);

	return NULL;
}

void fm_stop()
{
	signal(SIGCHLD, SIG_DFL);

	if(playproc)
	{
		kill(playproc, SIGKILL);

		waitpid(playproc, NULL, 0);
	}

	cleanup(list->history);
	list->history = NULL;

	pl_destroy(list);
	list = NULL;
}

void fm_list()
{
	pl_preview(list);
}

void fm_channel()
{
	list->position = -1;

	fm_run(list);
}

const struct playlist *fm_playlist()
{
	return list;
}

const struct hash **fm_track()
{
	return (const struct hash **)current;
}
