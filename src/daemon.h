/*
 * Licensed under the LGPL v2.1, see the file COPYING in base directory.
 *
 * Copyright (C) 2013 <tn.razy@gmail.com>
 *
 */

#ifndef _H_DAEMON_H
#define _H_DAEMON_H

#define PID_FILE 			"/var/run/user/%d/doubanFM.pid"

void daemonize ( const char *log, const char *err );

void daemonize_kill ( void );

#endif
