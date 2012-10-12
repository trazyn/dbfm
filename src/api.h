/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef API_H
#define API_H

#include "hash.h"

#define API 			"http://www.douban.com/j/app/radio/people?app_name=radio_desktop_win&version=100&user_id=%s&expire=%s&token=%s&channel=%d&sid=%d&type=%c"

enum cmd_type
{	
	/* ban and refresh */
	CMD_BAN 	= 0x62,

	/* next song */
	CMD_NEXT 	= 0x65,

	/* init playlist */
	CMD_INITPL 	= 0x6E,

	/* refresh playlist */
	CMD_REFRESH 	= 0x70,

	/* song love */
	CMD_LOVE 	= 0x72,

	/* skip and refresh playlist */
	CMD_SKIP 	= 0x73,

	/* song unlove */
	CMD_UNLOEV 	= 0x75,


	CMD_UNKNOW,
};

char **api_send_request(enum cmd_type type, int sid, char *history);

#endif
