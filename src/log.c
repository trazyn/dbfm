/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "log.h"

#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <stdio.h>

int msg_out[] =
{
    	#define XX(NAME, VALUE, FP, PRE_MSG, COLOR)             (FP & 1) + 1,
    	MSG_MAP(XX)
    	#undef XX

    	/* end */
    	0
};

int msg_color[] = 
{
    	#define XX(NAME, VALUE, FP, PRE_MSG, COLOR)             COLOR,
    	MSG_MAP(XX)
    	#undef XX

    	/* end */
    	0
};

char *msg_prefix[] =
{
    	#define XX(NAME, VALUE, FP, PRE_MSG, COLOR)             PRE_MSG,
    	MSG_MAP(XX)
    	#undef XX

    	NULL
};


static FILE *log = NULL;

static FILE *err = NULL;

void openlog(const char *flog, const char *ferr)
{
	if(flog)
	{
		log = fopen(flog, "a+");

		log = log ? log : stdout;
		
		if(flog && ferr && 0 == strncasecmp(flog, ferr, strlen(ferr)))
		{
			err = log;

			return;
		}

	}
	else if(NULL == log)
		log = stdout;

	if(ferr)
	{
		err = fopen(ferr, "a+");

		err = err ? err : stderr;
	}
	else if(NULL == err)
		err = stderr;
}

void closelog()
{
	if(log)
	{
		fclose(log);
		log = NULL;
	}

	if(err)
	{
		fclose(err);
		err = NULL;
	}
}

void prmsg(enum msg_types msg_type, const char *format, ...)
{
	char buf[BUFSIZ] = { 0 };
	va_list argv;

	openlog(NULL, NULL);

	if(msg_type < 0 || msg_type >= MSG_UNKNOW)
	{
		msg_type = MSG_INFO;
	}
	
	va_start(argv, format);
	vsnprintf(buf, sizeof buf, format, argv);
	va_end(argv);

	___MSG( 	msg_out[msg_type] == 2 ? err : log,
	  		msg_prefix[msg_type],
	  		buf, 
	  		msg_color[msg_type], 
	  		"");

	fflush(log);
	fflush(err);
}

void die(const char *format, ...)
{
	char buf[512] = { 0 };
	va_list argv;

	openlog(NULL, NULL);

	va_start(argv, format);
	vsnprintf(buf, sizeof buf, format, argv);
	va_end(argv);

	___MSG( 	err,
			msg_prefix[MSG_ERROR],
			buf, 
			msg_color[MSG_ERROR], 
			"");

	exit(EXIT_FAILURE);
}

