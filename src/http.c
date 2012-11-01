/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#define ___DEBUG

#include "http.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <netdb.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netinet/in.h>

static FILE *ropen(const char *host, unsigned short port, int timeout);

static char **read_response(FILE *fp);

static char to_hex(char ch);

static char from_hex(char ch);

static FILE *ropen(const char *host, unsigned short port, int timeout)
{
	int fd, ret;

	struct hostent *entry;
	struct sockaddr_in server;

	if(entry = gethostbyname(host), NULL == entry)
	{
		_ERROR("failed to get host(%s) info\n", host);

		return NULL;
	}

	memset(&server, 0, sizeof server);
	server.sin_family = AF_INET;
	server.sin_port = htons(port);
	server.sin_addr = *((struct in_addr *)entry->h_addr);

	if(-1 == (fd = socket(AF_INET, SOCK_STREAM, 0)))
	{
		_ERROR("failed to create socket: %s\n", strerror(errno));
		return NULL;
	}

	fcntl(fd, F_SETFL, O_NONBLOCK);

	while(connect(fd, (struct sockaddr *)&server, sizeof(struct sockaddr)) < 0)
	{
		if(EINTR == errno)
		{
			continue;
		}

		break;
	}

	struct timeval tv, *tvp;
	fd_set fdset;

	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);

	if(timeout <= 0 || timeout > 10)
	{
		tvp = NULL;
	}
	else
	{
		tv.tv_usec = 0;
		tv.tv_sec = timeout;
		tvp = &tv;
	}

	ret = select(fd + 1, &fdset, &fdset, &fdset, tvp);

	fcntl(fd, F_SETFL, fcntl(fd, F_GETFL) & ~O_NONBLOCK);

	if(0 == ret)
	{
		_ERROR("connect to '%s' timeout\n", host);
		return NULL;
	}

	if(ret < 0 && EINPROGRESS != errno)
	{
		_ERROR("failed to connect '%s': %s", host, strerror(errno));
		return NULL;
	}

	return fdopen(fd, "w+");
}


static char **read_response(FILE *fp)
{
	char *line, **resp = NULL;
	size_t size;
	unsigned nline = 0;

	while(!feof(fp))
	{
		line = NULL;
		size = 0;
		errno = 0;

		if(getline(&line, &size, fp) > 0)
		{
			resp = realloc(resp, (nline + 2) * sizeof *resp);

			resp[nline++] = line;
			resp[nline] = NULL;
		}
		else
		{
			free(line);

			if(EINTR == errno)
			{
				continue;
			}
		}
	}

	return resp;
}

char **fetch(const char *url, FILE **handle, const char *post, const char *type)
{
	char *host, *port, *status = NULL, *file = NULL, *line = NULL, urlcpy[512];
	unsigned short nport = 80, nstatus = 0;
	size_t size = 0;
	FILE *fp;

	_DEBUG("URL: %s", url);

	strncpy(urlcpy, url, sizeof urlcpy);

	host = &urlcpy[START_WITH(urlcpy, "http://") ? 7 : 0];
	port = strchr(host, ':');
	file = strchr(host, '/');
	
	if(port && port < file)
	{
		*port++ = 0;
		nport = (unsigned short)atoi(port);
	}

	if(file)
	{
		*file++ = 0;
	}
	else
		file = "";

	if(NULL == (fp = ropen(host, nport, 5)))
	{
		return NULL;
	}

	fprintf(fp, HEADER_FORMAT, post ? "POST" : "GET", file, host);

	if(post)
	{
		fprintf(fp, "Content-type: %s\r\n", type ? type : "application/x-www-form-urlencoded");
		fprintf(fp, "Content-length: %ld\r\n\r\n%s\r\n", strlen(post), post);
	}

	fputs("\r\n", fp);
	fflush(fp);

	if(getline(&status, &size, fp) >= 12)
	{
		sscanf(status, "HTTP/%*f %u", (unsigned int *)&nstatus);
	}
	free(status);

	if(200 != nstatus && 301 != nstatus && 302 != nstatus)
	{
		shutdown(fileno(fp), SHUT_RDWR);
		fclose(fp);

		_ERROR("error response status: %s\n", status);

		return NULL;
	}

	/* skip response header */
	while(1)
	{
		line = NULL;
		size = 0;
		errno = 0;

		if(getline(&line, &size, fp) < 3)
		{
			if(EINTR == errno)
			{
				continue;
			}

			break;
		}
	}

	free(line);

	if(handle)
	{
		*handle = fp;
		return NULL;
	}
	_DEBUG("URL: %s", NULL == fp ? "ERR" : "OK");

	return read_response(fp);
}

void free_response(char **ptr)
{
	if(NULL == ptr)
	{
		return;
	}

	char **p = ptr;

	for(char *line; line = *p, line;)
	{
		free(line);

		++p;
	}

	free(ptr);
}

size_t timeout_read(int fd, char *buf, size_t size, int nsec)
{
	fd_set fdset;
	struct timeval tv, *tvp = &tv;

	FD_ZERO(&fdset);
	FD_SET(fd, &fdset);

	if(nsec > 0)
	{
		tv.tv_usec = 0;
		tv.tv_sec = nsec;
	}
	else
	{
		tvp = NULL;
	}

	switch(select(fd + 1, &fdset, NULL, NULL, tvp))
	{
		case -1:
			/* error */
		case 0:
			/* timeout */
			return -1;
	}

	return read(fd, buf, size);
}

char *url_encode(const char *str)
{
	char *s = calloc(strlen(str) * 3 + 1, 1), *ptr = s;

	while(*str)
	{
		if(*str == '-' || *str == '~' || *str == '_' || *str == '.'
		  	/* isalnum */
		  	|| (*str >= 0 && *str <= 9)
		  	|| (*str >= 'a' && *str <= 'z')
		  	|| (*str >= 'A' && *str <= 'Z'))
			*s++ = *str;

		else if(*str == ' ')
		{
			*s++ = '+';
		}
		else
		{
			/* %H4L4 */
			*s++ = '%', *s++ = to_hex(*str >> 4), *s++ = to_hex(*str & 0XF);
		}

		++str;
	}

	*s = '\0';

	return ptr;
}

char *url_decode(const char *str)
{
	char *s = calloc(strlen(str) + 1, 1), *ptr = s;

	while(*str)
	{
		if(*str == '%')
		{
			if(str[1] && str[2])
			{
				/* H4 | L4 */
				*s++ = from_hex(str[1]) << 4 | from_hex(str[2]);

				/* skip H4 and L4 */
				str += 2;
			}
		}
		else if(*str == '+')
		{
			*s++ = ' ';
		}

		else
		{
			*s++ = *str;
		}

		++str;
	}

	*s = '\0';

	return ptr;
}

static char to_hex(char ch)
{
	static char hex[] = "0123456789abcdef";
	return hex[ch & 15];
}

static char from_hex(char ch)
{
	if(ch > 0 && ch < 9)
		return ch - '0';
	else
	{ 
		if(ch > 'A' && ch < 'Z')
			ch = ch + 'a' - 'A';
	}

	return ch - 'a' + 10;
}

