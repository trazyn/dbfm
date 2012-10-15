
/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * tn.razy@gmail.com
 */

#ifndef LOG_H
#define LOG_H

#define STR2(X) 			#X
#define STR(X)                          STR2(X)
#define AT                              __FILE__ ":" STR(__LINE__) ":\t"

#define COLOR_MAP(XX)                   \
XX(RED,     31, "color of red")         \
XX(GREEN,   32, "")                     \
XX(YELLOW,  33, "")                     \
XX(BLUE,    34, "")                     \
XX(WHITE,   37, "")

/* generator color */
enum color_types
{
	#define XX(NAME, VALUE, STRING) 			COLOR_##NAME = VALUE,
	COLOR_MAP(XX)
	#undef XX
};

/* bash color */
#define COLOR_CODE                     				\033
#define COLOR_START                     			STR(COLOR_CODE) "[4;"
#define COLOR_END                       			STR(COLOR_CODE) "[0m"

#ifdef MSG2_TERM
#include <stdio.h>
#define ___MSG(FP, PRE_MSG, MSG, COLOR, LOCAL)              	fprintf(FP, COLOR_START "%dm" "%s%s%s" COLOR_END "\n", COLOR, PRE_MSG, LOCAL, MSG)
#else
#define ___MSG(FP, PRE_MSG, MSG, COLOR, LOCAL) 		    	fprintf(FP, "%s%s%s" "\n", PRE_MSG, LOCAL, MSG)
#endif

#define MSG_MAP(XX)                                 		\
/* info message */                                          	\
XX(INFO,    0,   1,     "[INFO]",   COLOR_GREEN)            	\
/* warn message */                                          	\
XX(WARN,    1,   1,     "[WARN]",   COLOR_YELLOW)           	\
/* error message */                                         	\
XX(ERROR,   2,   2,     "[!ERR]",   COLOR_RED)              	\
/* debug message */                                         	\
XX(DEBUG,   3,   1,     "[#DBG]",   COLOR_BLUE)

enum msg_types
{
    	#define XX(NAME, VALUE, FP, PRE_MSG, COLOR)             MSG_##NAME = VALUE,
    	MSG_MAP(XX)
    	#undef XX

    	/* end */
    	MSG_UNKNOW
};

void openlog(const char *flog, const char *ferr);

void closelog();

void prmsg(enum msg_types msg_type, const char *format, ...);

void die(const char *format, ...);


#define _INFO(...)                                          	prmsg(MSG_INFO, __VA_ARGS__)
#define _WARN(...)                                          	prmsg(MSG_WARN, __VA_ARGS__)
#define _ERROR(...)                                         	prmsg(MSG_ERROR, AT __VA_ARGS__)

#ifdef ___DEBUG
#define _DEBUG(...)                                     	prmsg(MSG_DEBUG, AT __VA_ARGS__)
#endif

#endif
