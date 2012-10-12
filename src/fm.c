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
#include <pthread.h>
#include <sys/wait.h>

extern struct hash **rc;

static struct playlist *list;

static char ack = CMD_NEXT;

/* current track */
static struct hash **current;

static pid_t playproc = 0;

#define SID 						atoi(value((const struct hash **)current, "sid"))
#define URL 						value((const struct hash **)current, "url")
#define LOVE 						atoi(value((const struct hash **)current, "like"))
#define TITLE 						value((const struct hash **)current, "title")
#define ARTIST 						value((const struct hash **)current, "artist")

static void sig_waitplay(int signo);

static void *dl_thread(void *data);

void fm_run(struct playlist *pl)
{
	char text[BUFSIZ] = { 0 };

	list = pl;
	current = (struct hash **)pl_current(pl);

	signal(SIGCHLD, sig_waitplay);

	/* make sure only one instance of play process */
	if(playproc)
	{
		kill(playproc, SIGKILL);
		waitpid(playproc, NULL, 0);
	}

	switch((playproc = fork()))
	{
		case -1:
			die("failed to fork process");

		case 0:
			fprintf(stderr, meta((const struct hash **)current, "\nPLAYING: [%a - %t]\n", text));

			play(URL, value((const struct hash **)rc, "driver"));
			_exit(EXIT_SUCCESS);

		default:
			break;
	}
}

void fm_next()
{
	kill(playproc, SIGUSR1);
}

void fm_skip()
{
	pl_history(list, CMD_SKIP, SID);

	list->position = -1;

	fm_run(list);
}

void fm_love(struct playlist *pl)
{
	if(!LOVE)
	{
		ack = CMD_LOVE;

		free_response(api_send_request(CMD_LOVE, SID, NULL));

		erase(&current, "like");
		set(&current, "like", "1");
	}
}

void fm_unlove()
{
	if(LOVE)
	{
		ack = CMD_UNLOEV;

		free_response(api_send_request(CMD_UNLOEV, SID, NULL));

		erase(&current, "like");
		set(&current, "like", "0");
	}
}

void fm_ban()
{
	pl_history(list, CMD_BAN, SID);

	/* force refresh playlist */
	list->position = -1;

	fm_run(list);
}

void fm_recording()
{
	free_response(api_send_request(CMD_NEXT, SID, NULL));
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
	/* remove signal handle */
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
	fm_skip();
}

static void sig_waitplay(int signo)
{
	int status;

	waitpid(playproc, &status, 0);

	playproc = 0;

	if((WIFEXITED(status) && EXIT_SUCCESS == WEXITSTATUS(status))
	  		|| (WIFSIGNALED(status) && SIGUSR1 == WTERMSIG(status)))
	{
		list->position++;
		
		if(WIFEXITED(status) && EXIT_SUCCESS == WEXITSTATUS(status))
		{
			/* if play process is normal termination, append history */
			pl_history(list, ack, SID);

			fm_recording();

			ack = CMD_NEXT;
		}

		fm_run(list);
	}
	else if(WIFEXITED(status) && EXIT_FAILURE == WEXITSTATUS(status))
	{
		fm_stop();

		exit(EXIT_FAILURE);
	}
}
