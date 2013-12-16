/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef SESSION_H
#define SESSION_H

#include "config.h"

#define SESSION_API 						"http://www.douban.com/j/app/login"
#define SESSION_POST 						"email=%s&password=%s&app_name=radio_desktop_win&version=100"
#define SESSION_FILE 						"session"

void session(void *data);
  
#endif
