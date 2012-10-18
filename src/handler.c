/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "handler.h"
#include "hash.h"
#include "util.h"
#include "log.h"
#include "fm.h"
#include "screen.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <sys/socket.h>

extern struct hash **station;

extern struct hash **rc;

static void handle_client(int fd);

static void execmd(char *line, char *reply, int size);

void handle(int listenfd)
{
	int fd;

	if(0 == listenfd)
	{
		openlog(value((const struct hash **)rc, "log"), value((const struct hash **)rc, "err"));

		handle_screen();
	}
	else
	{
		while(1)
		{
			/* single mode */
			if(-1 == (fd = accept(listenfd, NULL, 0)))
			{
				_ERROR("failed to accept client: %s", strerror(errno));
			}

			handle_client(fd);
		}

		shutdown(listenfd, SHUT_RDWR);
	}
}

int tcpsock(unsigned short port)
{
	int listenfd;
	struct sockaddr_in server;

	if(-1 == (listenfd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		_ERROR("failed to creat socket: %s", strerror(errno));

		return -1;
	}

	int option = 1;
	if(-1 == (setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &option, sizeof option)))
	{
		_ERROR("failed to make socket re-usable: %s\n", strerror(errno));

		return -1;
	}

	memset(&server, 0, sizeof(struct sockaddr_in));

	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr.s_addr = htonl(INADDR_ANY);

	if(bind(listenfd, (struct sockaddr *)&server, sizeof(struct sockaddr)))
	{
		_ERROR("failed to bind socket: %s", strerror(errno));

		return -1;
	}

	listen(listenfd, 5);

	return listenfd;
}

static void handle_client(int fd)
{
	FILE *fp = fdopen(fd, "rw");

	if(NULL == fp)
	{
		_ERROR("failed to open fd: %s", strerror(errno));

		shutdown(fd, SHUT_RDWR);
		
		return;
	}

	while(1)
	{
		char *line = NULL, reply[16] = { 0 };
		size_t size = 0;
		int disconnect = 0;

		if(getline(&line, &size, fp) > 0)
		{
			*(strstr(line, "\r\n")) = 0;

			execmd(line, reply, 16);

			if(*reply)
			{
				//fprintf(fp, "%s", reply);
				//fflush(fp);
				send(fd, reply, 16, 0);
			}
		}
		else
		{
			disconnect = !disconnect;

			shutdown(fd, SHUT_RDWR);
			fclose(fp);
		}

		free(line);

		if(disconnect)
		{
			break;
		}
	}
}

static void execmd(char *line, char *reply, int size)
{
	const char *cmds[] = { "next", "skip", "love", "unlove", "ban", "channel", "kill", NULL };

	char *cmd, *arg;

	arg = strchr(line, 0x20);

	if(arg)
	{
		*arg++ = 0;
	}
	
	cmd = line;

	int idx = arridx(cmds, cmd);

	switch(idx)
	{
		case 0:
			fm_next();
			break;

		case 1:
			fm_skip();
			break;

		case 2:
			snprintf(reply, size, 0 == fm_love() ? "ok" : "err");
			break;

		case 3:
			snprintf(reply, size, 0 == fm_love() ? "ok" : "err");
			break;

		case 4:
			fm_ban();
			break;

		case 5:
			if(NULL != value((const struct hash **)station, arg))
			{
				reset(&rc, "channel", arg);

				fm_channel();
			}
			break;

		case 6:
			exit(EXIT_SUCCESS);

	}
}

