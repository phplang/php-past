/* 
   +----------------------------------------------------------------------+
   | PHP HTML Embedded Scripting Language Version 3.0                     |
   +----------------------------------------------------------------------+
   | Copyright (c) 1997,1998 PHP Development Team (See Credits file)      |
   +----------------------------------------------------------------------+
   | This program is free software; you can redistribute it and/or modify |
   | it under the terms of the GNU General Public License as published by |
   | the Free Software Foundation; either version 2 of the License, or    |
   | (at your option) any later version.                                  |
   |                                                                      |
   | This program is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the        |
   | GNU General Public License for more details.                         |
   |                                                                      |
   | You should have received a copy of the GNU General Public License    |
   | along with this program; if not, write to the Free Software          |
   | Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.            |
   +----------------------------------------------------------------------+
   | Authors: Andi Gutmans <andi@php.net>                                 |
   |          Zeev Suraski <bourbon@netvision.net.il>                     |
   +----------------------------------------------------------------------+
 */


/* $Id: datetime.c,v 1.27 1998/01/23 01:29:42 zeev Exp $ */


#ifdef THREAD_SAFE
#include "tls.h"
#endif
#include "parser.h"
#include "internal_functions.h"
#include "operators.h"
#include "datetime.h"
#include "snprintf.h"

#include <time.h>
#include <stdio.h>

char *mon_full_names[] =
{
	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December"
};
char *mon_short_names[] =
{
	"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
};
char *day_full_names[] =
{
	"Sunday", "Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday"
};
char *day_short_names[] =
{
	"Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat"
};

static int phpday_tab[2][12] =
{
	{31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
	{31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
};

void php3_time(INTERNAL_FUNCTION_PARAMETERS)
{
	return_value->value.lval = (long) time(NULL);
	return_value->type = IS_LONG;
}

void php3_mktime(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *arguments[6];
	struct tm ta, *tn;
	time_t t;
	int i, arg_count = ARG_COUNT(ht);


	if (arg_count > 6 || getParametersArray(ht, arg_count, arguments) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	/* convert supplied arguments to longs */
	for (i = 0; i < arg_count; i++) {
		convert_to_long(arguments[i]);
	}
	t=time(NULL);
	tn = localtime(&t);
	memcpy(&ta,tn,sizeof(struct tm));	
	ta.tm_isdst = -1;

	switch(arg_count) {
	case 6:
		ta.tm_year = arguments[5]->value.lval - ((arguments[5]->value.lval > 1000) ? 1900 : 0);
		/* fall-through */
	case 5:
		ta.tm_mday = arguments[4]->value.lval;
		/* fall-through */
	case 4:
		ta.tm_mon = arguments[3]->value.lval - 1;
		/* fall-through */
	case 3:
		ta.tm_sec = arguments[2]->value.lval;
		/* fall-through */
	case 2:
		ta.tm_min = arguments[1]->value.lval;
		/* fall-through */
	case 1:
		ta.tm_hour = arguments[0]->value.lval;
	case 0:
		break;
	}
	return_value->value.lval = mktime(&ta);
	return_value->type = IS_LONG;
}

static void
_php3_date(INTERNAL_FUNCTION_PARAMETERS, int gm)
{
	YYSTYPE *format, *timestamp;
	time_t the_time;
	struct tm *ta;
	int i, size = 0, length, h;
	char tmp_buff[16];
	TLS_VARS;

	switch(ARG_COUNT(ht)) {
	case 1:
		if (getParameters(ht, 1, &format) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		the_time = time(NULL);
		break;
	case 2:
		if (getParameters(ht, 2, &format, &timestamp) == FAILURE) {
			WRONG_PARAM_COUNT;
		}
		convert_to_long(timestamp);
		the_time = timestamp->value.lval;
		break;
	default:
		WRONG_PARAM_COUNT;
	}
	convert_to_string(format);

	if (gm) {
		ta = gmtime(&the_time);
	} else {
		ta = localtime(&the_time);
	}

	if (!ta) {			/* that really shouldn't happen... */
		php3_error(E_WARNING, "unexpected error in date()");
		RETURN_FALSE;
	}
	for (i = 0; i < format->strlen; i++) {
		switch (format->value.strval[i]) {
			case 'U':		/* seconds since the epoch */
				size += 10;
				break;
			case 'F':		/* month, textual, full */
			case 'l':		/* day (of the week), textual */
				size += 9;
				break;
			case 'Y':		/* year, numeric, 4 digits */
				size += 4;
				break;
			case 'M':		/* month, textual, 3 letters */
			case 'D':		/* day, textual, 3 letters */
			case 'z':		/* day of the year */
				size += 3;
				break;
			case 'y':		/* year, numeric, 2 digits */
			case 'm':		/* month, numeric */
			case 'd':		/* day of the month, numeric */
			case 'H':		/* hour, numeric, 24 hour format */
			case 'h':		/* hour, numeric, 12 hour format */
			case 'i':		/* minutes, numeric */
			case 's':		/* seconds, numeric */
			case 'A':		/* AM/PM */
			case 'a':		/* am/pm */
			case 'S':		/* standard english suffix for the day of the month (e.g. 3rd, 2nd, etc) */
				size += 2;
				break;
			case 'w':		/* day of the week, numeric */
			default:
				size++;
				break;
		}
	}

	return_value->value.strval = (char *) emalloc(size + 1);
	return_value->value.strval[0] = '\0';

	for (i = 0; i < format->strlen; i++) {
		switch (format->value.strval[i]) {
			case 'U':		/* seconds since the epoch */
				sprintf(tmp_buff, "%ld", the_time); /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 'F':		/* month, textual, full */
				strcat(return_value->value.strval, mon_full_names[ta->tm_mon]);
				break;
			case 'l':		/* day (of the week), textual, full */
				strcat(return_value->value.strval, day_full_names[ta->tm_wday]);
				break;
			case 'Y':		/* year, numeric, 4 digits */
				sprintf(tmp_buff, "%d", ta->tm_year + 1900);  /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 'M':		/* month, textual, 3 letters */
				strcat(return_value->value.strval, mon_short_names[ta->tm_mon]);
				break;
			case 'D':		/* day (of the week), textual, 3 letters */
				strcat(return_value->value.strval, day_short_names[ta->tm_wday]);
				break;
			case 'z':		/* day (of the year) */
				sprintf(tmp_buff, "%d", ta->tm_yday);  /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 'y':		/* year, numeric, 2 digits */
				sprintf(tmp_buff, "%02d", ((ta->tm_year)%100));  /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 'm':		/* month, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_mon + 1);  /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 'd':		/* day of the month, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_mday);  /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 'H':		/* hour, numeric, 24 hour format */
				sprintf(tmp_buff, "%02d", ta->tm_hour);  /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 'h':		/* hour, numeric, 12 hour format */
				h = ta->tm_hour % 12; if (h==0) h = 12;
				sprintf(tmp_buff, "%02d", h);  /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 'i':		/* minutes, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_min);  /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 's':		/* seconds, numeric */
				sprintf(tmp_buff, "%02d", ta->tm_sec);  /* SAFE */ 
				strcat(return_value->value.strval, tmp_buff);
				break;
			case 'A':		/* AM/PM */
				strcat(return_value->value.strval, (ta->tm_hour >= 12 ? "PM" : "AM"));
				break;
			case 'a':		/* am/pm */
				strcat(return_value->value.strval, (ta->tm_hour >= 12 ? "pm" : "am"));
				break;
			case 'S':		/* standard english suffix, e.g. 2nd/3rd for the day of the month */
				if (ta->tm_mday >= 10 && ta->tm_mday <= 19) {
					strcat(return_value->value.strval, "th");
				} else {
					switch (ta->tm_mday % 10) {
						case 1:
							strcat(return_value->value.strval, "st");
							break;
						case 2:
							strcat(return_value->value.strval, "nd");
							break;
						case 3:
							strcat(return_value->value.strval, "rd");
							break;
						default:
							strcat(return_value->value.strval, "th");
							break;
					}
				}
				break;
			case 'w':		/* day of the week, numeric EXTENSION */
				sprintf(tmp_buff, "%01d", ta->tm_wday);  /* SAFE */
				strcat(return_value->value.strval, tmp_buff);
				break;
			default:
				length = strlen(return_value->value.strval);
				return_value->value.strval[length] = format->value.strval[i];
				return_value->value.strval[length + 1] = '\0';
				break;
		}
	}
	return_value->strlen = strlen(return_value->value.strval);
	return_value->type = IS_STRING;
}

void php3_date(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 0);
}

void php3_gmdate(INTERNAL_FUNCTION_PARAMETERS)
{
	_php3_date(INTERNAL_FUNCTION_PARAM_PASSTHRU, 1);
}

void php3_getdate(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *timestamp;
	struct tm *ta;

	if (ARG_COUNT(ht) != 1 || getParameters(ht, 1, &timestamp) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(timestamp);

	ta = localtime((time_t *) & timestamp->value.lval);
	if (!ta) {
		php3_error(E_WARNING, "Cannot perform date calculation");
		return;
	}
	if (array_init(return_value) == FAILURE) {
		php3_error(E_ERROR, "Unable to initialize array");
		return;
	}
	add_assoc_long(return_value, "seconds", ta->tm_sec);
	add_assoc_long(return_value, "minutes", ta->tm_min);
	add_assoc_long(return_value, "hours", ta->tm_hour);
	add_assoc_long(return_value, "mday", ta->tm_mday);
	add_assoc_long(return_value, "wday", ta->tm_wday);
	add_assoc_long(return_value, "mon", ta->tm_mon + 1);
	add_assoc_long(return_value, "year", ta->tm_year + 1900);
	add_assoc_long(return_value, "yday", ta->tm_yday);
	add_assoc_string(return_value, "weekday", day_full_names[ta->tm_wday], 1);
	add_assoc_string(return_value, "month", mon_full_names[ta->tm_mon], 1);
	add_index_long(return_value, 0, timestamp->value.lval);
}

/* Return date string in standard format for http headers */
char *php3_std_date(time_t t)
{
	struct tm *tm1;
	char *str;

	tm1 = gmtime(&t);
	str = emalloc(81);
	snprintf(str, 80, "%s, %02d-%s-%02d %02d:%02d:%02d GMT",
			day_full_names[tm1->tm_wday],
			tm1->tm_mday,
			mon_short_names[tm1->tm_mon],
			((tm1->tm_year)%100),
			tm1->tm_hour, tm1->tm_min, tm1->tm_sec);
	str[79]=0;
	return (str);
}

/* 
 * CheckDate(month, day, year);
 *  returns True(1) if it is valid date
 *
 */
#define isleap(year) (((year%4) == 0 && (year%100)!=0) || (year%400)==0)
void php3_checkdate(INTERNAL_FUNCTION_PARAMETERS)
{
	YYSTYPE *month, *day, *year;
	int m, d, y;
	TLS_VARS;
	
	if (ARG_COUNT(ht) != 3 ||
		getParameters(ht, 3, &month, &day, &year) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_long(day);
	convert_to_long(month);
	convert_to_long(year);
	y = year->value.lval;
	m = month->value.lval;
	d = day->value.lval;

	if (y < 100)
		y += 1900;

	if (y < 0 || y > 32767) {
		RETURN_FALSE;
	}
	if (m < 1 || m > 12) {
		RETURN_FALSE;
	}
	if (d < 1 || d > phpday_tab[isleap(y)][m - 1]) {
		RETURN_FALSE;
	}
	RETURN_TRUE;				/* True : This month,day,year arguments are valid */
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 */
