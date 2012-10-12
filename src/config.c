/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#include "config.h"
#include "log.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>

extern struct hash **rc;

void loadcfg(struct hash ***arr, const char *file)
{
	FILE *fp;
	unsigned nline;
	size_t size;
	char *line = NULL, key[16] = { 0 }, value[256] = { 0 };

	if(file && access(file, F_OK | R_OK) == -1)
	{
		die("failed load config file '%s'", file);
	}

	fp = fopen(file, "r");

	nline = 0;

	while(!feof(fp))
	{
		++nline;

		line = NULL;
		size = 0;

		if(getline(&line, &size, fp) < 4)
		{
			if(size)
			{
				free(line);
			}

			continue;
		}

		/* skip comment line */
		if('#' == *line)
		{
			if(size)
			{
				free(line);
			}

			continue;
		}
		
		int valid = sscanf(line, "%15[^=\t]=%63[^\r\n]", key, value);

		if(2 != valid)
		{
			die("invalid config line:%d, %s", nline, line);
		}

		set(arr, key, value);

		free(line);
	}

	fclose(fp);
}

void mkcfg(const struct hash **arr, const char *filename)
{
	char cfgfile[FILENAME_MAX], tmpfile[FILENAME_MAX], *ptr = tmpfile;
	struct stat st;

	snprintf(cfgfile, FILENAME_MAX, "%s/%s", getenv("XDG_CONFIG_HOME"), CFG_PATH);
	
	if(stat(cfgfile, &st) < 0 || !S_ISDIR(st.st_mode))
	{
		mkdir(cfgfile, 0755);
	}

	/* enter to config dir */
	chdir(cfgfile);
	
	if('/' != *filename && '\\' != *filename)
	{
		
		strncpy(tmpfile, filename, FILENAME_MAX);

		while(ptr = strchr(tmpfile, '/'), ptr)
		{
			*ptr++ = 0;

			if(access(tmpfile, F_OK))
			{
				mkdir(tmpfile, 0755);
			}

			chdir(tmpfile);

			strcpy(tmpfile, ptr);
		}

		/* back to config dir */
		chdir(cfgfile);
	}

	FILE *fp = fopen(filename, "w+");
	struct hash *item = NULL;

	while(arr && (item = (struct hash *)*arr++))
	{
		fprintf(fp, "%s=%s", item->key, item->value);

		fprintf(fp, "\r\n\r\n");
	}

	fclose(fp);
}
